//
// Created by Ali on 12.08.2022.
//
/**
 * reading intial configurations and settings specially json file from radar
 * as well as projection and homography matrix (opencv free implementation)
 * Opencv exists for debugging and after that will be removed
 *
 * */
#ifndef DETTRACKPL_CONFIGURATIONS_H
#define DETTRACKPL_CONFIGURATIONS_H
#include <iostream>
#include <fstream>
#include <cstring>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <gst/gst.h>
#include <glib.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

typedef struct _point {
    float cx;
    float cy;
    float cz;
} cs_point;
typedef struct _shape {
    int width;
    int height;
    int channel;
} cs_shape;

typedef struct _rectangle{
    int frame_num;
    float xmin;
    float ymin;
    float xmax;
    float ymax;
    float get_width(){
        return xmax-xmin;
    }
    float get_height(){
        return ymax-ymin;
    }
    cs_point get_center(){
        cs_point center;
        center.cx = 0.5*(xmax + xmin);
        center.cy = 0.5*(ymax + ymin);
        return center;
    }
} cs_rectangle;

typedef struct targetVehicleRecord_{
    int id;
    std::vector<cs_rectangle> bbox_info;
    float target_iou_score;
    float avg_speed;
    float total_score;
}targetVehicleRecord;

// keeping candid vehicles. key: object track id , value vehicle information
extern std::unordered_map<int, targetVehicleRecord> cs_target_vehicle_candids;

namespace config{

    extern float cs_ProjectionMat[9];
    extern cs_rectangle cs_RadarPosition;
    extern float cs_target_speed;
    extern std::string cs_evidenceVideo;
    extern float cs_FPS;
    extern cs_shape radarFrameShape;

    class CalibrationConfigs{
    public:
        static void initialize(char * , char *, char *, char *);
        static void readSpeedFromJsonFile(const char *);
        static void readCalibrationMatrix(const char *, float Matrix[9]);
        static void readCorrespondingPoints(const char *, std::vector<cs_rectangle>&);
        static void convertCoordinates_plt2gg();
        static void checkCalibrationMatrix(std::vector<cs_rectangle> &);

    public:
        static char overviewImagePath[256];
        static float HomographyMat[9];
    };


}
void multiplyMatrixToVector(float  M[9], float vec[2], float d[2]);

/* add calibration control here */




#endif //DETTRACKPL_CONFIGURATIONS_H
