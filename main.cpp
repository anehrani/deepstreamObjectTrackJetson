#include <iostream>
#include "include/ds_speed.h"
#include "include/configurations.h"
#include <fstream>
#include <string>
#include <iostream>


/**
 * initialize config here
 * parameters related to calibration and homograpphy
 * */





int main(int argc, char* argv[]) {

    // todo: decide how to get FPS
    // path to initial homography and calibration matrix
    char* projectionMat = "/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/projection_mat.txt";
    char* homographyMat = "/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/plt-gg_homography_matrix.txt";
    char* jsonFilePath = "/media/sd/speedEstimationFiles/AZMES_ME06EKIN_2023-03-29T15_35_03_160_MAX_SPEED/c690ef43-feda-40b7-8f6d-ad22722b3069_j.json"; // Note this could be temporary
    char* correspondings = "/media/sd/speedEstimationFiles/ekinfixedpointspeedestimation/calibration_3/data/corresponding_points.txt";

    if(argc > 1)
    {
        projectionMat = argv[1];
        homographyMat = argv[2];
        jsonFilePath = argv[3];
        correspondings = argv[4];
    }
    else
    {
        std::cout << "Something wrong from args, check and try again" << std::endl;
        exit(1);
    }

    config::CalibrationConfigs::initialize(projectionMat, homographyMat, jsonFilePath, correspondings);


    float speed = ds_speed_estimation(argc, argv);
    // std::string input = std::to_string(speed);
    // std::cin >> input;
    // std::ofstream out("output.txt");
    // out << input;
    // out.close();
    std::cout << "vehicle Speed is :" << speed  << std::endl;
    return 0;
}

