#include "MapMatching.h"
#define M_PI 3.14159265358979323846

double distance_condition = 0.7;
double distance_condition_last_point = 0.00005;

double calculateLatitude(double latitude) {
    return 111132.954 - 559.822 * cos(2 * latitude * M_PI / 180) + 1.175 * cos(4 * latitude * M_PI / 180);
}

double calculateLongitude(double longitude) {
    double earth_radius_eq = 6378137.0; //in meters
    double e2 = 0.00669437999014;
    
    return M_PI * earth_radius_eq * cos(longitude * M_PI / 180) / (180 *
        sqrt(1 - e2 * sin(longitude * M_PI / 180) * sin(longitude * M_PI / 180)));
}

double distanceInMeters(GPSPoint& start, GPSPoint& finish) {
    double R = 6378.137; // Earth radius in km
    double d_lat = finish.latitude * M_PI / 180 - start.latitude * M_PI / 180;
    double d_long = finish.longitude * M_PI / 180 - start.longitude * M_PI / 180;
    double a = sin(d_lat / 2) * sin(d_lat / 2) + cos(start.latitude * M_PI / 180) * cos(finish.latitude * M_PI / 180) * 
        sin(d_long / 2) * sin(d_long / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = R * c;
    return d * 1000; //meters
}

double distanceInMeters(double& lat1, double& long1, double& lat2, double& long2) {
    GPSPoint start = { lat1, long1 };
    GPSPoint finish = { lat2, long2 };
    return distanceInMeters(start, finish);
}

bool GPSPoint::operator==(const GPSPoint& rhs) {
    if (this->latitude == rhs.latitude && this->longitude == rhs.longitude)
        return true;
    else
        return false;
}

void MapSegment::loadLinesFromDisk() {
    std::ifstream lines_from_file;
    lines_from_file.open("map_test.txt");
    if (lines_from_file.is_open()) {
        while (!lines_from_file.eof()) {
            MapLinePair temp;
            if (lines_from_file >> temp.begin.latitude >> temp.begin.longitude >> temp.end.latitude >> temp.end.longitude) {
                //std::cout << std::setprecision(10) << temp.begin.latitude << " " << temp.begin.longitude << " " << temp.end.latitude << " " << temp.end.longitude << "\n";
                line_collection.push_back(temp);
            }
            else std::cout << "Error @ reading lines from file";
        }
    }
    else {
        std::cout << "\nError opening file";
    }
    lines_from_file.close();
}

double countDistance(GPSPoint& gps_read, GPSPoint& line_begin, GPSPoint& line_end) {
    double x0 = gps_read.latitude;
    double y0 = gps_read.longitude;
    double x1 = line_begin.latitude;
    double y1 = line_begin.longitude;
    double x2 = line_end.latitude;
    double y2 = line_end.longitude;

    double distance;
    distance = (pow((x1 - x0), 2.0) + pow((y1 - y0), 2.0) + pow((x2 - x0), 2.0) + pow((y2 - y0), 2.0)) / (pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0));
    //elipse-like
    return distance;
}
double countDistance(GPSPoint& gps_read, MapLinePair& line) {
    return countDistance(gps_read, line.begin, line.end);
}


int findMinimalDistanceIndex(GPSPoint& gps_read, MapSegment& map, double& distance_condition) {
    int pair_index = -1;
    double current_min = INT_MAX;
    for (int i = 0; i < map.line_collection.size(); i++) {
        double temp = countDistance(gps_read, map.line_collection[i]);
        if (temp < distance_condition && temp < current_min) {
            pair_index = i;
            current_min = temp;
        }
    }

    
    //std::cout << current_min << "\n";
    //std::ofstream file;
    //file.open("test_dist.txt", std::ios::out| std::ios::app);
    //if (file.is_open()) {
    //    file << current_min << "\n";
    //}
    //else {
    //    std::cout << "\nError opening file";
    //}
    return pair_index;
}

double dotProduct2d(const Vec2d& v1, const Vec2d& v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

GPSPoint matchCoordinatesToLine(GPSPoint& gps_read, MapLinePair& line) {
    GPSPoint matched_point;
    Vec2d line_vector = { line.end.latitude - line.begin.latitude, line.end.longitude - line.begin.longitude };
    Vec2d read_vector = { gps_read.latitude - line.begin.latitude, gps_read.longitude - line.begin.longitude };

    double scale = dotProduct2d(read_vector, line_vector) / dotProduct2d(line_vector, line_vector);
    matched_point = { line_vector.x * scale + line.begin.latitude, line_vector.y * scale + line.begin.longitude };
    return matched_point;
}

GPSPoint matchCoordinatesToLine(GPSPoint& gps_read, GPSPoint& line1, GPSPoint& line2) {
    MapLinePair line = { line1, line2 };
    return matchCoordinatesToLine(gps_read, line);
}

double countDistanceFromLastPoint(GPSPoint& last, GPSPoint& current) {
    return sqrt(pow((current.latitude - last.latitude), 2.0) + pow((current.longitude - last.longitude), 2.0));
}

//void findAndMatch(TrackInstance& trasa, MapSegment& map) {
//    int last_line_found_index = -1;
//    for (int i = 0; i < trasa.points.size(); i++) {
//        if (last_line_found_index == -1) {
//            int found_line_index = -1;
//            found_line_index = findMinimalDistanceIndex(trasa.points[i], map, distance_condition);
//            if (found_line_index != -1) {
//                GPSPoint new_read = matchCoordinatesToLine(trasa.points[i], map.line_collection[found_line_index]);
//                trasa.points[i] = new_read;
//                last_line_found_index = found_line_index;
//            }
//        }
//        else {
//            if (countDistanceFromLastPoint(trasa.points[i - 1], trasa.points[i]) < distance_condition_last_point) {
//                //std::cout << countDistanceFromLastPoint(trasa.points[i - 1], trasa.points[i]) << "\n";
//                GPSPoint new_read = matchCoordinatesToLine(trasa.points[i], map.line_collection[last_line_found_index]);
//                trasa.points[i] = new_read;
//            }
//            else {
//                int found_line_index = -1;
//                found_line_index = findMinimalDistanceIndex(trasa.points[i], map, distance_condition);
//                if (found_line_index != -1) {
//                    GPSPoint new_read = matchCoordinatesToLine(trasa.points[i], map.line_collection[found_line_index]);
//                    trasa.points[i] = new_read;
//                }
//                last_line_found_index = found_line_index;
//            }
//        }
//    }
//}

