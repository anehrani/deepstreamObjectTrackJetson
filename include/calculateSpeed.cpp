//
// Created by ek on 11.05.2023.
//

#include <algorithm>
#include <math.h>
#include "calculateSpeed.h"

/*
 * for sorting candid target vehicles based on iou score
 * and also speed differences
 */
 bool cmp(targetVehicleRecord &a, targetVehicleRecord &b){
    return a.total_score > b.total_score;
}



float calculateSpeed::calculate_speed(){

    // sort candids based on score:
    std::vector<targetVehicleRecord> best_matches;
    for (auto iter=cs_target_vehicle_candids.begin(); iter!=cs_target_vehicle_candids.end(); iter++ ){
        int trackSize = iter->second.bbox_info.size();
        if (trackSize <5) continue;

        iter->second.id = iter->first;

        float dist_x = 0, dist_y = 0, dist = 0;
        int timediff = 0;
        int diff = 1;



        for (int i = 0; i < trackSize - diff; i++)
            for (int j = i+diff; j < trackSize; j += 1) {
                // convert boxes
                cs_point vehiclebox_A = iter->second.bbox_info.at(i).get_center();
                cs_point vehiclebox_B = iter->second.bbox_info.at(j).get_center();
                map_position_2d_to_3d_b(vehiclebox_A);
                map_position_2d_to_3d_b(vehiclebox_B);

                // mapping the position
                dist_x = std::abs(vehiclebox_A.cx - vehiclebox_B.cx);
                dist_y = std::abs(vehiclebox_A.cy - vehiclebox_B.cy);
                dist += std::sqrt(dist_x * dist_x + dist_y * dist_y);
                timediff += std::abs(iter->second.bbox_info.at(j).frame_num - iter->second.bbox_info.at(i).frame_num);
            }; // for j


         iter->second.avg_speed = config::cs_FPS*dist/(float) timediff;

         float score = 1 - std::abs(iter->second.avg_speed - config::cs_target_speed)/ config::cs_target_speed;

         iter->second.total_score = iter->second.target_iou_score + score;

        best_matches.push_back(iter->second);
    }

    std::sort(best_matches.begin(), best_matches.end(), cmp);

    // the first should be the target vehicle we are looking for to measure

   return  best_matches.at(0).avg_speed;
}



void calculateSpeed::map_position_2d_to_3d_b(cs_point & Point){

    float cx_r = Point.cx * config::cs_ProjectionMat[0] + Point.cy * config::cs_ProjectionMat[1] +
                 config::cs_ProjectionMat[2];
    float cy_r = Point.cx * config::cs_ProjectionMat[3] + Point.cy * config::cs_ProjectionMat[4] +
                 config::cs_ProjectionMat[5];
    float denum = Point.cx * config::cs_ProjectionMat[6] + Point.cy * config::cs_ProjectionMat[7] +
                  config::cs_ProjectionMat[8];
    Point.cx = cx_r / (denum + 1e-10);
    Point.cy = cy_r / (denum + 1e-10);

};
