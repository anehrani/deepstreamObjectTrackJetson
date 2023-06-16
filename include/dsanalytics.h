//
// Created by ek on 30.07.2022.
//

#ifndef DETTRACKPL_DSANALYTICS_H
#define DETTRACKPL_DSANALYTICS_H



#include <gst/gst.h>
#include <glib.h>
#include <array>
#include <vector>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <memory>

#include "nvds_analytics_meta.h"
#include "nvdsmeta.h"
#include "gstnvdsmeta.h"
//#include "nvbufsurface.h"
//#include "nvbufsurftransform.h"
#include "configurations.h"
//
#include <stdio.h>
#include <iostream>
// --------------------------------

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


namespace dsanalytic {
        GstPadProbeReturn nvdsanalyticsSrcPadBufferProbe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data);


}// namespace



namespace trackVehicles {

    // set all these hyper parameters from a config file
    constexpr auto MAX_ACTIVE_INTERVAL = 5;
    const int MAX_TRACK_AGE = 40;
    const int MIN_TRACK_AGE = 10;

    typedef struct _track_obj_block {
        float avg_speed = 0;
        std::vector<cs_rectangle> track_positions;
        // areas that object is in
        int age;
        int cuTrackSize;
        bool track_status = 1;
        int source_id;
        int inactive_depth = 0;
        float target_score;

    } track_obj_block;

    static std::unordered_map<gint64 , track_obj_block> tracking_metadata;

//    void update_track_metadata( std::unordered_map<gint64 , track_obj_block> * track_metadata_tr , int );
//    void update_target_vehicle_candids( cs_rectangle &, float  );

    float calculateTargetVehicleScore( cs_rectangle & );






}// namespace speedClc





#endif //DETTRACKPL_DSANALYTICS_H
