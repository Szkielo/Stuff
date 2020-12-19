#include "Graphs.h"
#include<iostream>
#include<exception>
#include<string>
#include<fstream>

using namespace std;

int area_size = 60;
int avg_parameter = 3; //3 forward, 3 backward, 7 altogether

void GPSPointsLoader::loadPointsFromDisk() {
    std::ifstream points_from_file;
    points_from_file.open("test.txt");
    if (points_from_file.is_open()) {
        while (!points_from_file.eof()) {
            GPSPoint temp;
            if (points_from_file >> temp.latitude >> temp.longitude) {
                //std::cout << std::setprecision(10) << temp.latitude << " " << temp.longitude << "\n";
                points.push_back(temp);
            }
            else std::cout << "Error @ reading points from file";
        }
    }
    else {
        std::cout << "\nError opening file";
    }

    points_from_file.close();
}

void GPSPointsLoader::findBorders() { //may crash on -180 -> 180 degrees
    top_latitude = points[0].latitude;
    right_longitude = points[0].longitude;
    bottom_latitude = points[0].latitude;
    left_longitude = points[0].longitude;
    for (int i = 1; i < points.size(); i++) {
        if (points[i].latitude > top_latitude) {
            top_latitude = points[i].latitude;
        }
        else if (points[i].latitude < bottom_latitude) {
            bottom_latitude = points[i].latitude;
        }
        if (points[i].longitude > right_longitude) {
            right_longitude = points[i].longitude;
        }
        else if (points[i].longitude < left_longitude) {
            left_longitude = points[i].longitude;
        }
    }
}

void GPSPointsLoader::savePointsOnDisk() {
    std::ofstream file;
    file.open("test_results.csv", std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << "latitude,longitude,index\n";
        for (int i = 0; i < points.size(); i++) {
            file << std::setprecision(10) << points[i].latitude << "," << points[i].longitude << "," << i << "\n";
        }
    }
    else {
        std::cout << "\nError opening file";
    }

    file.close();
}

std::vector<GPSPoint> GPSPointsLoader::movingAverageSimple() {
    std::vector<GPSPoint> output;
    double sum_lat;
    double sum_long;
    int count = avg_parameter * 2 + 1;
    for (int i = avg_parameter; i < points.size() - avg_parameter; i++) {
        sum_lat = 0;
        sum_long = 0;
        for (int j = i - avg_parameter; j < i + avg_parameter + 1; j++) {
            sum_lat += points[j].latitude;
            sum_long += points[j].longitude;
        }
        GPSPoint temp = { sum_lat / count, sum_long / count };
        output.push_back(temp);
    }
    return output;
}

std::vector<GPSPoint> GPSPointsLoader::selectEveryXth(int x) {
    std::vector<GPSPoint> temp;
    for (int i = 0; i < points.size(); i += x) {
        temp.push_back(points[i]);
    }
    return temp;
}

void MapPointsLoader::loadMap()
{
    ifstream map_file;
    string type;
    GPSPoint point;
    vector<GPSPoint> points;
    int count;
    map_file.open("map_test_2.txt");
    if (map_file.is_open()) {
        while (!map_file.eof()) {
            Segment s;
            getline(map_file, type, '[');
            map_file >> count;
            for (int i = 0; i < count; i++)
            {
                map_file >> point.latitude >> point.longitude;
                points.push_back(point);
            }
            map_file.ignore(3);
            s.type = type;
            s.points = points;
            segments.push_back(s);
            points.clear();
        }
    }
    else {
        std::cout << "\nError opening file";
    }
    map_file.close();
}


vector<vector<int>> MapPointsLoader::markSegmentsForSplitting() {
    vector<vector<int>> segment_marks; //for each index there is vector with splitting points
    vector<int> temp;
    for (int i = 0; i < segments.size(); i++) {
        segment_marks.push_back(temp);
    }
    for (int i = 0; i < segments.size(); i++) {
        for (int j = 1; j < segments[i].points.size() - 1; j++) {
            for (int k = 0; k < segments.size(); k++) {
                if (k != i) {
                    if (segments[i].points[j] == segments[k].points[0]) {
                        segment_marks[i].push_back(j);
                        break;
                    }
                    else if (segments[i].points[j] == segments[k].points[segments[k].points.size() - 1]) {
                        segment_marks[i].push_back(j);
                        break;
                    }
                }
            }
        }
    }
    return segment_marks;
}

void MapPointsLoader::makeSplit(vector<Segment>& temp, vector<int>& marks, int& seg_index) {
    int last_element = 0;

    for (int i = 0; i < marks.size(); i++) {
        Segment s;
        s.type = segments[seg_index].type;
        for (int j = last_element; j <= marks[i]; j++) {
            s.points.push_back(segments[seg_index].points[j]);
        }
        last_element = marks[i];
        temp.push_back(s);
    }
    Segment s;
    s.type = segments[seg_index].type;
    for (int j = last_element; j < segments[seg_index].points.size(); j++) {
        s.points.push_back(segments[seg_index].points[j]);
    }
    temp.push_back(s);
}

void MapPointsLoader::splitSegments() {
    vector<vector<int>> marks = markSegmentsForSplitting();
    vector<Segment> temp;

    for (int i = 0; i < segments.size(); i++) {
        if (marks[i].empty()) {
            temp.push_back(segments[i]);
        }
        else {
            makeSplit(temp, marks[i], i);
        }
    }
    segments = temp;
}



void MapPointsLoader::findConnections()
{
    for (int i = 0; i < segments.size(); i++) {
        GPSPoint i_start = segments[i].points[0];
        GPSPoint i_end = segments[i].points[segments[i].points.size() - 1];
        for (int j = 0; j < segments.size(); j++) {
            GPSPoint j_start = segments[j].points[0];
            GPSPoint j_end = segments[j].points[segments[j].points.size() - 1];
                if (i_start == j_start) {
                    if (i != j) {
                        segments[i].connected_segments.push_back(&segments[j]);
                    }
                }
                else if (i_start == j_end) {
                    if (i != j) {
                        segments[i].connected_segments.push_back(&segments[j]);
                    }
                }
                else if (i_end == j_start) {
                    if (i != j) {
                        segments[i].connected_segments.push_back(&segments[j]);
                    }
                }
                else if (i_end == j_end) {
                    if (i != j) {
                        segments[i].connected_segments.push_back(&segments[j]);
                    }
                }
        }
    }
}

void Map::divideMapIntoAreas() {
    double distance_vertically = calculateLatitude(gps_l.top_latitude) * (gps_l.top_latitude - gps_l.bottom_latitude);
    double distance_horizontally = calculateLongitude(gps_l.left_longitude) * (gps_l.right_longitude - gps_l.left_longitude);
    vertical_divisions = distance_vertically / area_size;
    horizontal_divisions = distance_horizontally / area_size;

    double vertical_step = (gps_l.top_latitude - gps_l.bottom_latitude) / vertical_divisions;
    double horizontal_step = (gps_l.right_longitude - gps_l.left_longitude) / horizontal_divisions;

    for (int i = 0; i < vertical_divisions; i++) {
        for (int j = 0; j < horizontal_divisions; j++) {
            MapArea a;
            a.left_longitude = gps_l.left_longitude + horizontal_step * j;
            a.right_longitude = gps_l.left_longitude + horizontal_step * (j + 1.0);
            a.top_latitude = gps_l.top_latitude - vertical_step * i;
            a.bottom_latitude = gps_l.top_latitude - vertical_step * (i + 1.0);
            a.index = i * horizontal_divisions + j;
            map_l.areas.push_back(a);
        }
    }
}

bool isPointInArea(GPSPoint& point, MapArea& area) {
    
    if (point.latitude <= area.top_latitude && point.latitude >= area.bottom_latitude) {
        if (point.longitude <= area.right_longitude && point.longitude >= area.left_longitude) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

bool isSegmentInArea(Segment& segment, MapArea& area) {
    for (int i = 0; i < segment.points.size(); i++) {
        if (isPointInArea(segment.points[i], area)) {
            return true;
        }
    }
    return false;
}

void Map::initializeSurrounding(int& index) {
    bool array[8];
    for (int i = 0; i < 8; i++) { array[i] = true; }
    
    if (index < horizontal_divisions) {
        array[0] = false;
        array[1] = false;
        array[2] = false;
    }
    if (index > horizontal_divisions * (vertical_divisions - 1) - 1) {
        array[4] = false;
        array[5] = false;
        array[6] = false;
    }
    if (index % horizontal_divisions == 0) {
        array[0] = false;
        array[7] = false;
        array[6] = false;
    }
    if (index % horizontal_divisions == horizontal_divisions - 1) {
        array[2] = false;
        array[3] = false;
        array[4] = false;
    }
        
    if (array[0] == true)
        map_l.areas[index].surrounding[0] = &map_l.areas[index - horizontal_divisions - 1];
    if (array[1] == true)
        map_l.areas[index].surrounding[1] = &map_l.areas[index - horizontal_divisions];
    if (array[2] == true)
        map_l.areas[index].surrounding[2] = &map_l.areas[index - horizontal_divisions + 1];
    if (array[3] == true)
        map_l.areas[index].surrounding[3] = &map_l.areas[index + 1];
    if (array[4] == true)
        map_l.areas[index].surrounding[4] = &map_l.areas[index + horizontal_divisions + 1];
    if (array[5] == true)
        map_l.areas[index].surrounding[5] = &map_l.areas[index + horizontal_divisions];
    if (array[6] == true)
        map_l.areas[index].surrounding[6] = &map_l.areas[index + horizontal_divisions - 1];
    if (array[7] == true)
        map_l.areas[index].surrounding[7] = &map_l.areas[index - 1];
}

void Map::findActiveAreas() {
    MapArea* last_area = nullptr;
    for (int i = 0; i < gps_l.points.size(); i++) {
        if (last_area != nullptr) {
            if (!isPointInArea(gps_l.points[i], *last_area)) {
                for (int j = 0; j < 8; j++) {
                    if (last_area->surrounding[j] == nullptr) continue;
                        if (isPointInArea(gps_l.points[i], *last_area->surrounding[j])) {
                            gps_l.points[i].matched_area = last_area->surrounding[j];
                            if (last_area->surrounding[j]->is_active == false) {
                                last_area->surrounding[j]->is_active = true;
                                main_areas.push_back(last_area->surrounding[j]);
                                initializeSurrounding(last_area->surrounding[j]->index);
                            }
                            last_area = last_area->surrounding[j];
                            break;
                        }
                }
            }
            else {
                gps_l.points[i].matched_area = last_area;
            }
        }
        else {
            for (int j = 0; j < map_l.areas.size(); j++) {
                if (isPointInArea(gps_l.points[i], map_l.areas[j])) {
                    gps_l.points[i].matched_area = &map_l.areas[j];
                    map_l.areas[j].is_active = true;
                    main_areas.push_back(&map_l.areas[j]);
                    last_area = &map_l.areas[j];
                    initializeSurrounding(last_area->index);
                    break;
                }
            }
        }
    }

    for (int i = 0; i < main_areas.size(); i++) {
        for (int j = 0; j < 8; j++) {
            if (main_areas[i]->surrounding[j] == nullptr) continue;
            main_areas[i]->surrounding[j]->is_active = true;
        }
    }

    for (int i = 0; i < map_l.areas.size(); i++) {
        if (map_l.areas[i].is_active == true) {
            active_areas.push_back(&map_l.areas[i]);
        }
    }
}

void Map::matchSegmentsToAreas() {
    for (int i = 0; i < map_l.segments.size(); i++) {
        for (int j = 0; j < active_areas.size(); j++) {
            if (isSegmentInArea(map_l.segments[i], *active_areas[j])) {
                active_areas[j]->segments_ptr.push_back(&map_l.segments[i]);
                map_l.segments[i].connected_areas.push_back(active_areas[j]);
            }
        }
    }
}

std::vector<Segment*> Map::generateSegmentPool(MapArea* area) {
    vector<Segment*> temp;
    for (int i = 0; i < area->segments_ptr.size(); i++) {
        temp.push_back(area->segments_ptr[i]);
    }
    for (int i = 0; i < 8; i++) {
        if (area->surrounding[i] == nullptr) continue;
        for (int j = 0; j < area->surrounding[i]->segments_ptr.size(); j++) {
            temp.push_back(area->surrounding[i]->segments_ptr[j]);
        }
    }
    return temp;
}

std::vector<Segment*> Map::generateSegmentPool(GPSPoint& point) {
    return generateSegmentPool(point.matched_area);
}

void MergeInstance::calculateProximity() {
    GPSPoint avg = { (gps_read1->latitude + gps_read2->longitude) / 2, (gps_read1->longitude + gps_read2->longitude) / 2 };
    for (int i = 0; i < segment_pool->size(); i++) {
        for (int j = 0; j < segment_pool[i].size() - 1; j++) {
            //calc dist gps1, gps2, seg[i], seg[i+1]
        }
    }
}

FoundSegmentInfo Map::findClosestSegment(GPSPoint point, std::vector<Segment*> segment_pool) {
    double distance = countDistance(point, segment_pool[0]->points[0], segment_pool[0]->points[1]);
    int seg_index = 0;
    int point_index = 0;
    for (int i = 1; i < segment_pool.size(); i++) {
        for (int j = 0; j < segment_pool[i]->points.size() - 1; j++) {
            double temp_dist = countDistance(point, segment_pool[i]->points[j], segment_pool[i]->points[j + 1]);
            if (temp_dist < distance) {
                distance = temp_dist;
                seg_index = i;
                point_index = j;
            }
        }
    }

    FoundSegmentInfo info = { seg_index, point_index };
    return info;
}

bool Map::checkIfPointInBoundaries(GPSPoint& point, GPSPoint& line_start, GPSPoint& line_finish){ 
    double a = countDistanceFromLastPoint(point, line_start);
    double b = countDistanceFromLastPoint(point, line_finish);
    double c = countDistanceFromLastPoint(line_start, line_finish);

    if (c > a && c > b) {
        return true;
    }
    else if (a > b && a > c) {
        if (pow(a, 2.0) > pow(b, 2.0) + pow(c, 2.0)){
            return false;
        }
        else {
            return true;
        }
    }
    else if (b > a && b > c) {
        if (pow(b, 2.0) > pow(a, 2.0) + pow(c, 2.0)) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return true;
    }
}



void Map::matchPointsToSegments() {
    std::vector<Segment*> segment_pool = generateSegmentPool(gps_l.points[0]);
    MapArea* last_matched_area = gps_l.points[0].matched_area;
    GPSPoint new_point;
    for (int i = 0; i < gps_l.points.size(); i++) {
        if (gps_l.points[i].matched_area != last_matched_area) {
            segment_pool = generateSegmentPool(gps_l.points[i]);
        }
        
        FoundSegmentInfo info = findClosestSegment(gps_l.points[i], segment_pool);
        if (checkIfPointInBoundaries(gps_l.points[i], segment_pool[info.seg_index]->points[info.point_index], segment_pool[info.seg_index]->points[info.point_index + 1])) {
            new_point = matchCoordinatesToLine(gps_l.points[i], segment_pool[info.seg_index]->points[info.point_index], segment_pool[info.seg_index]->points[info.point_index + 1]);
        }
        else {
            if (countDistanceFromLastPoint(gps_l.points[i], segment_pool[info.seg_index]->points[info.point_index]) < countDistanceFromLastPoint(gps_l.points[i], segment_pool[info.seg_index]->points[info.point_index + 1])) {
                new_point = segment_pool[info.seg_index]->points[info.point_index];
            }
            else {
                new_point = segment_pool[info.seg_index]->points[info.point_index + 1];
            }
        }
        gps_l.points[i] = new_point;
    }
}