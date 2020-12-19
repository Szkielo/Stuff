#pragma once
#include "MapMatching.h"
#include <string>
#include <unordered_map>



class GPSPointsLoader {
public:
	std::vector<GPSPoint> points;
	long int timestamp;

	double top_latitude; //map borders
	double right_longitude;
	double bottom_latitude;
	double left_longitude;

	void loadPointsFromDisk();
	void findBorders();
	void savePointsOnDisk();
	std::vector<GPSPoint> selectEveryXth(int x);
	std::vector<GPSPoint> movingAverageSimple();
	GPSPointsLoader() {
		loadPointsFromDisk();
		findBorders();
		//savePointsOnDisk();
		points = movingAverageSimple();
	}
};



struct Segment {
	double length = 0;
	std::string type;
	std::vector<GPSPoint> points;
	std::vector<Segment*> connected_segments;
	std::vector<MapArea*> connected_areas;
};

struct MapArea {
	std::vector<Segment*> segments_ptr;
	double top_latitude;
	double right_longitude;
	double bottom_latitude;
	double left_longitude;
	int index;
	MapArea* surrounding[8]{};
	bool is_active = false;
};

struct FoundSegmentInfo {
	int seg_index;
	int point_index;
};

class MapPointsLoader {
public:
	std::vector<Segment> segments;
	std::vector<MapArea> areas;


	void loadMap();
	std::vector<std::vector<int>> markSegmentsForSplitting();
	void splitSegments();
	void makeSplit(std::vector<Segment>& temp, std::vector<int>& marks, int& seg_index);
	void findConnections();
	MapPointsLoader() {
		loadMap();
		splitSegments();
		findConnections();
	}
};

bool isPointInArea(GPSPoint& point, MapArea& area);
bool isSegmentInArea(Segment& segment, MapArea& area);

class Map {
public:
	GPSPointsLoader gps_l;
	MapPointsLoader map_l;

	int vertical_divisions;
	int horizontal_divisions;
	
	std::vector<MapArea*> main_areas;
	std::vector<MapArea*> active_areas;

	void divideMapIntoAreas();
	Map() {
		divideMapIntoAreas();
		findActiveAreas();
		matchSegmentsToAreas(); 
		matchPointsToSegments();
		// smoothing mechanism
		gps_l.points = gps_l.movingAverageSimple();
		gps_l.savePointsOnDisk();
	}
	void initializeSurrounding(int& index);
	void findActiveAreas();
	void matchSegmentsToAreas();
	std::vector<Segment*> generateSegmentPool(MapArea* area);
	std::vector<Segment*> generateSegmentPool(GPSPoint& point);
	
	FoundSegmentInfo findClosestSegment(GPSPoint point, std::vector<Segment*> segment_pool);
	bool checkIfPointInBoundaries(GPSPoint& point, GPSPoint& line_start, GPSPoint& line_finish);

	void matchPointsToSegments();
};



class MergeInstance {
public:
	GPSPoint* gps_read1;
	GPSPoint* gps_read2;
	std::vector<Segment*>* segment_pool;

	class SortedSegments;
	
	Segment* picked_segment;
	int picked_index;
	double current_best_fit;
	
	void calculateProximity();

	MergeInstance(GPSPoint* gps_read1, GPSPoint* gps_read2, std::vector<Segment*>* segment_pool) {
		this->gps_read1 = gps_read1;
		this->gps_read2 = gps_read2;
		this->segment_pool = segment_pool;

		calculateProximity();
	}
};

bool checkIfPointInBoundaries(GPSPoint& point, GPSPoint line_start, GPSPoint line_finish);


