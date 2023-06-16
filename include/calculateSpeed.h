//
// Created by ek on 11.05.2023.
//

#ifndef DETTRACKPL_CALCULATESPEED_H
#define DETTRACKPL_CALCULATESPEED_H

#include "configurations.h"
#include "calculateSpeed.h"


class calculateSpeed {
public:
    static float calculate_speed();
    static void map_position_2d_to_3d_b(cs_point & Point);
    void Proj3d22d( float o2dPt[2], float o3dPt[3], int nLenUnit);
    void Bkproj2d23d( float o2dPt[2], float o3dPt[3], int nLenUnit=1000, int nCoordSysTyp=1);

//    void calculate_speed_b( track_obj_block & obj_speedmeta_in, int status);
//    void calculate_speed_a( track_obj_block & obj_speedmeta_in, int status);
//    void update_speed(std::unordered_map<gint64 , track_obj_block> * track_metadata_tr );




};


#endif //DETTRACKPL_CALCULATESPEED_H
