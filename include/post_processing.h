//
// Created by ek on 12.08.2022.
//

#ifndef DETTRACKPL_VISUALIZATION_H
#define DETTRACKPL_VISUALIZATION_H

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
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

#include <iostream>

#define PGIE_CLASS_ID_VEHICLE 0
#define PGIE_CLASS_ID_PERSON 2

#define MAX_DISPLAY_LEN 64


namespace process {


    typedef struct _perf_measure{
        GstClockTime pre_time;
        GstClockTime total_time;
        guint count;
    }perf_measure;



    static gint frame_number = 0;
    static gint offset = 0;

//    GstPadProbeReturn osd_sink_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data);
    GstPadProbeReturn tiler_src_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data);
    GstPadProbeReturn osd_sink_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data);



};


#endif //DETTRACKPL_VISUALIZATION_H
