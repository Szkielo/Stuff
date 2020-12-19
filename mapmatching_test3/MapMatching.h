#pragma once
#include <vector>
#include <string>
#include "math.h"
#include <fstream>
#include <iostream>
#include <iomanip>

double calculateLatitude(double latitude);
double calculateLongitude(double longitude);

struct MapArea;

struct GPSPoint {
    double latitude; 
    double longitude;

    MapArea* matched_area;

    bool operator==(const GPSPoint& rhs);
};

struct ExtendedGPSPoint {
	unsigned int seconds_from_start;
	double latitude;
	double longitude;
    //MapArea* area_ptr;
};

struct MapLinePair {
    GPSPoint begin;
    GPSPoint end;
};

struct Vec2d {
    double x;
    double y;
};


class MapSegment {
public:
    std::vector<MapLinePair> line_collection;

    void loadLinesFromDisk();
};

double distanceInMeters(GPSPoint& start, GPSPoint& finish);
double distanceInMeters(double& lat1, double& long1, double& lat2, double& long2);
//void findAndMatch(TrackInstance& trasa, MapSegment& map);
double countDistance(GPSPoint& gps_read, GPSPoint& line_begin, GPSPoint& line_end);
double countDistance(GPSPoint& gps_read, MapLinePair& line);

GPSPoint matchCoordinatesToLine(GPSPoint& gps_read, GPSPoint& line1, GPSPoint& line2);
double countDistanceFromLastPoint(GPSPoint& last, GPSPoint& current);