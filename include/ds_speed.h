//
// Created by ek on 28.07.2022.
//

#ifndef TINYYOLOV2_DS_YOLO_H
#define TINYYOLOV2_DS_YOLO_H

#include <stdio.h>
#include <iostream>

#include <gst/gst.h>
#include <glib.h>
#include <cuda_runtime_api.h>
#include <array>
#include <sstream>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
//
#include "nvdsmeta.h"
#include "gstnvdsmeta.h"
#include "nvds_analytics_meta.h"
//
#include "dsanalytics.h"


#define MEASURE_ENABLE 1
#define MUXER_OUTPUT_WIDTH 1920 // 1280
#define MUXER_OUTPUT_HEIGHT 1080 // 720
#define MUXER_BATCH_TIMEOUT_USEC 400000
#define TILED_OUTPUT_WIDTH 1920
#define TILED_OUTPUT_HEIGHT 1080
#define CONFIG_GROUP_TRACKER "tracker"
#define CONFIG_GROUP_TRACKER_WIDTH "tracker-width"
#define CONFIG_GROUP_TRACKER_HEIGHT "tracker-height"
#define CONFIG_GROUP_TRACKER_LL_CONFIG_FILE "ll-config-file"
#define CONFIG_GROUP_TRACKER_LL_LIB_FILE "ll-lib-file"
#define CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS "enable-batch-process"
#define CONFIG_GPU_ID "gpu-id"

/* By default, OSD process-mode is set to CPU_MODE. To change mode, set as:
 * 1: GPU mode (for Tesla only)
 * 2: HW mode (For Jetson only)
 */
#define OSD_PROCESS_MODE 0

/* By default, OSD will not display text. To display text, change this to 1 */
#define OSD_DISPLAY_TEXT 0

// for analytics part
#define PRIMARY_DETECTOR_UID 1



const int gpu_id=0;
const int enable_batch_process=1;


static const char ll_lib_file[128] = "/opt/nvidia/deepstream/deepstream-6.0/lib/libnvds_nvmultiobjecttracker.so";
// ll-config-file required to set different tracker types
static const std::string ll_config_file="../cfg/config_tracker_NvDCF_accuracy.yml";
//ll-config-file=config_tracker_DeepSORT.yml


static const int tracker_width=640;
static const int tracker_height=384;


void speedEstimator ();
float  ds_speed_estimation (int argc, char *argv[]);




#endif //DS_YOLO_H
