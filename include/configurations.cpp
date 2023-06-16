//
// Created by ek on 4.05.2023.
//

#include "configurations.h"
#include <math.h>





std::unordered_map<int, targetVehicleRecord> cs_target_vehicle_candids;

namespace config{
    float cs_ProjectionMat[9];
    float CalibrationConfigs::HomographyMat[9];
    std::string cs_evidenceVideo = "";
    cs_rectangle cs_RadarPosition;
    float cs_target_speed = 0.0;
    float cs_FPS = 20;
    cs_shape radarFrameShape = {0,0};
    char CalibrationConfigs::overviewImagePath[256] = "";
    // clean target vehicles



    void CalibrationConfigs::initialize(char * projectionFilePath,
                                        char * homographyFilePath,
                                        char * jsonFilePath,
                                        char * correspoindingPointsFile){
        /**
         * Read initial configs ...
         * and Parse json
         * */
        readCalibrationMatrix(projectionFilePath, cs_ProjectionMat);
        readCalibrationMatrix(homographyFilePath, HomographyMat);
        // now parse jason and get the position of the vehicle with radar
        readSpeedFromJsonFile(jsonFilePath);
        // convert radar coordinate to general cam coordinates
        convertCoordinates_plt2gg();

        //
        std::vector<cs_rectangle> controlPoints;
        readCorrespondingPoints( correspoindingPointsFile, controlPoints);

        checkCalibrationMatrix( controlPoints );


    }

    void CalibrationConfigs::readSpeedFromJsonFile(const char *configPath) {

        std::ifstream file(configPath );
        // Read the file into a string
        std::string json_str( (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>() );

        // Parse the JSON string
        rapidjson::Document doc;
        doc.Parse(json_str.c_str());

        // Get the speed value from the snapshot object
        cs_evidenceVideo =  doc["evidenceVideo"].GetString();
        cs_target_speed = doc["snapshot"]["speed"].GetDouble();
        radarFrameShape.width = doc["snapshot"][ "imageWidth"].GetFloat();
        radarFrameShape.height = doc["snapshot"][ "imageHeight"].GetFloat();
        std::string overviewImg = doc["snapshot"][ "overviewImagePath"].GetString();
        cs_RadarPosition.xmin = std::abs( doc["snapshot"]["rectangle"]["upperLeft"]["x"].GetFloat() );
        cs_RadarPosition.ymin = std::abs( doc["snapshot"]["rectangle"]["upperLeft"]["y"].GetFloat() );
        cs_RadarPosition.xmax = std::abs( doc["snapshot"]["rectangle"]["lowerRight"]["x"].GetFloat() );
        cs_RadarPosition.ymax = std::abs( doc["snapshot"]["rectangle"]["lowerRight"]["y"].GetFloat() );

    }


    void CalibrationConfigs::readCalibrationMatrix(const char *filename, float scMatric[9]){
        FILE * poCamParamFl = std::fopen( filename, "r");
        if ( poCamParamFl == NULL ) { std::fputs("Error: camera parameters not loaded\n", stderr); exit(1); }
        std::ifstream ifsCamParam;
        ifsCamParam.open( filename );
        while (!ifsCamParam.eof())
        {
            char acBuf[256] = { 0 };

            ifsCamParam.getline(acBuf, 256);
            std::sscanf(acBuf, "%f %f %f %f %f %f %f %f %f ",
                        &scMatric[0],  &scMatric[1], &scMatric[2], &scMatric[3], &scMatric[4],
                        &scMatric[5],  &scMatric[6], &scMatric[7], &scMatric[8]);
        }
        ifsCamParam.close();

    };

    void CalibrationConfigs::convertCoordinates_plt2gg(){
        float mvec[2] = {0,0};
        float ded[2] = {0,0};
        // upper left
        mvec[0] = config::cs_RadarPosition.xmin;
        mvec[1] = config::cs_RadarPosition.ymin;
        multiplyMatrixToVector ( HomographyMat, mvec, ded);
        config::cs_RadarPosition.xmin = ded[0];
        config::cs_RadarPosition.ymin = ded[1];
        // lower right
        mvec[0] = config::cs_RadarPosition.xmax;
        mvec[1] = config::cs_RadarPosition.ymax;
        multiplyMatrixToVector ( HomographyMat, mvec, ded);
        config::cs_RadarPosition.xmax = ded[0];
        config::cs_RadarPosition.ymax = ded[1];
    };

    void CalibrationConfigs::readCorrespondingPoints(const char * filename, std::vector<cs_rectangle>& controlPoints){

        FILE * poCamParamFl = std::fopen( filename, "r");
        if ( poCamParamFl == NULL ) { std::fputs("Error: camera parameters not loaded\n", stderr); exit(1); }
        std::ifstream ifsCamParam;
        ifsCamParam.open( filename );

        float rowPoints[4];

        char acBuf[256] = { 0 };
        while (ifsCamParam.getline(acBuf, 256))
        {
            //ifsCamParam.getline(acBuf, 256);
            std::sscanf(acBuf, "%f %f %f %f",
                        &rowPoints[0],  &rowPoints[1], &rowPoints[2], &rowPoints[3] );

            cs_rectangle tmpRowPoints;
            tmpRowPoints.xmin = rowPoints[0];
            tmpRowPoints.ymin = rowPoints[1];
            tmpRowPoints.xmax = rowPoints[2];
            tmpRowPoints.ymax = rowPoints[3];

            controlPoints.push_back( tmpRowPoints );

        }
        ifsCamParam.close();



    };


    void CalibrationConfigs::checkCalibrationMatrix(std::vector<cs_rectangle> & points){

        float errors = 0;
        for (auto iter = points.begin(); iter!=points.end(); iter++){
            float x1 = config::cs_ProjectionMat[0] * iter->xmin +  config::cs_ProjectionMat[1] * iter->ymin +  config::cs_ProjectionMat[2];
            float y1 = config::cs_ProjectionMat[3] * iter->xmin +  config::cs_ProjectionMat[4] * iter->ymin +  config::cs_ProjectionMat[5];
            float z = config::cs_ProjectionMat[6] * iter->xmin +  config::cs_ProjectionMat[7] * iter->ymin +  config::cs_ProjectionMat[8];
            float x2 = iter->xmax;
            float y2 = iter->ymax;

            x1 /= z; y1 /= z;
            errors += std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));

        }

        errors /= points.size();

        std::cout<< "calibration matrix error: " << errors << std::endl;

    }

}

void multiplyMatrixToVector(float  M[9], float vec[2], float d[2]){
    float v1 =  vec[0] * M[0] + vec[1] * M[1] +  M[2];
    float v2 =  vec[0] * M[3] + vec[1] * M[4] +  M[5];
    float v3 =  vec[0] * M[6] + vec[1] * M[7] +  M[8];
    d[0] = v1/(v3 + 1e-9);
    d[1] = v2/(v3 + 1e-9);
}
