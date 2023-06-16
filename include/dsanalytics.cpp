//
// Created by ek on 30.07.2022.
//

#include "dsanalytics.h"
#include "metadata.h"

/*
 * Target vehicle detector should work once during vide test
 * and keep the target vehicle position during the process

*/
GstPadProbeReturn dsanalytic::nvdsanalyticsSrcPadBufferProbe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data)
{



    GstBuffer *buf = (GstBuffer *) info->data;
    guint num_rects = 0;
    NvDsObjectMeta *obj_meta = nullptr;
    NvDsMetaList * l_frame = nullptr;
    NvDsMetaList * l_obj = nullptr;

    // -----------------------------------------------------------------
    //                         Vizualization
    // -----------------------------------------------------------------
    /*
     Visuzlization:
     -> For visalizing and debugging purpose only
     */
//    NvDsDisplayMeta *display_meta = NULL;
    NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta (buf);
    /* display stuff */
//    display_meta = nvds_acquire_display_meta_from_pool(batch_meta);

    GstMapInfo in_map_info;
    if (!gst_buffer_map (buf, &in_map_info, GST_MAP_READ)) {
        g_print ("Error: Failed to map gst buffer\n");
        gst_buffer_unmap (buf, &in_map_info);
        return GST_PAD_PROBE_OK;
    }
//    NvBufSurface *surface = (NvBufSurface *)in_map_info.data;
    //
//    NvBufSurfaceMap(surface, -1, -1, NVBUF_MAP_READ);
    /* Cache the mapped data for CPU access */
//    NvBufSurfaceSyncForCpu(surface, 0, 0); //will do nothing for unified memory type on dGPU
    /* iterating over the collected data*/
    /* the first for loop is for the batch of frames from multiple(or single) sources
    */



    for (l_frame = batch_meta->frame_meta_list; l_frame != nullptr; l_frame = l_frame->next) {
        /**
         * IMPORTANT
         *
         * @brief later i need to sync the time using NTP from the source for better performance (keeping time track)
         *
         * @brief for the moment i will use fps recorded frames of the video to measure the time difference
         * and calculate speed
         *
         */

        NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) (l_frame->data);

        // get image height and width
//        guint height = surface->surfaceList[frame_meta->batch_id].height;
//        guint width = surface->surfaceList[frame_meta->batch_id].width;
//
        for (l_obj = frame_meta->obj_meta_list; l_obj != nullptr; l_obj = l_obj->next) {
            obj_meta = (NvDsObjectMeta *) (l_obj->data);
            // keep only car, truck, bus and motorbike
            if (!obj_meta && !(obj_meta->class_id==2 || obj_meta->class_id==3 || obj_meta->class_id==5 || obj_meta->class_id==7) )
                continue;

            // For speed calculation, only vehicles inside the selected areas will be considered
            for (NvDsMetaList *l_user_meta = obj_meta->obj_user_meta_list; l_user_meta != nullptr; l_user_meta = l_user_meta->next) {
                //
                NvDsUserMeta *user_meta = (NvDsUserMeta *) (l_user_meta->data);

                if( 1 ) //user_meta->base_meta.meta_type == NVDS_USER_OBJ_META_NVDSANALYTICS)
                {
                    NvDsAnalyticsObjInfo * user_meta_data = (NvDsAnalyticsObjInfo *)user_meta->user_meta_data;

                    /** check if vehicle is inside the ROI (region of the interest)
                     * the could be more that one region of the interest,
                     * this feature (handling multiple roi's) will be added later
                     *
                     * SECOND POINT: Speed well calculated after line crossing
                     *
                     * */
                    cs_rectangle tmp_object_position{
                            frame_meta->frame_num,
                            obj_meta->rect_params.left/frame_meta->pipeline_width,
                            obj_meta->rect_params.top/frame_meta->pipeline_height,
                            (obj_meta->rect_params.left + obj_meta->rect_params.width)/frame_meta->pipeline_width,
                            (obj_meta->rect_params.top + obj_meta->rect_params.height)/frame_meta->pipeline_height
                    };

                    // check if this is target vehicle
                    float target_score = trackVehicles::calculateTargetVehicleScore( tmp_object_position );
                    // if debug

                    if (target_score<1e-5) continue;
                    // adding the info
//                    std::cout<< "frame: "<< frame_meta->frame_num << " obj id: " << obj_meta->object_id  <<  "  iou score: " << target_score << std::endl;

                    targetVehicleRecord new_vehicle_record;

                    auto exists = cs_target_vehicle_candids.find( obj_meta->object_id );
                    if ( exists != cs_target_vehicle_candids.end()){
                        if (exists->second.target_iou_score < target_score )
                            exists->second.target_iou_score = target_score;
                        exists->second.bbox_info.push_back( tmp_object_position );
//                        std::cout<< obj_meta->object_id <<" updated! " << " target_score: " << exists->second.target_iou_score << std::endl;
                    }
                    else {
                        new_vehicle_record.target_iou_score = target_score;
                        new_vehicle_record.bbox_info.push_back( tmp_object_position);
                        cs_target_vehicle_candids.insert(std::make_pair(  obj_meta->object_id, new_vehicle_record  ) );
//                        std::cout<< obj_meta->object_id <<" created! " << " target_score: " << target_score << std::endl;
                    }
                } // if
            }// for in the user meta list
        }// for l_obj

    }





    return GST_PAD_PROBE_OK;
}


float trackVehicles::calculateTargetVehicleScore( cs_rectangle & newObjectPosition){
    /**
     * in this function, target vehicle will be detected by getting frame objects
     *  -> input: vehicle bboxes
     *  ->  output: IOU score
     *
     * */
    // calculate IOU between box and target radar box
    // clculating the iou with the target vehicle from radar
    float iou_x_min = MAX( newObjectPosition.xmin,config::cs_RadarPosition.xmin  );
    float iou_y_min = MAX(newObjectPosition.ymin, config::cs_RadarPosition.ymin  );
    float iou_x_max = MIN( newObjectPosition.xmax, config::cs_RadarPosition.xmax  );
    float iou_y_max = MIN( newObjectPosition.ymax, config::cs_RadarPosition.ymax  );
    //
    float iou_score = (iou_x_max - iou_x_min) * (iou_y_max - iou_y_min); // /(float) ( config::radarFrameShape.width * config::radarFrameShape.height \
                                                                                  + newObjectPosition.get_height() * newObjectPosition.get_width() );
    if ((iou_x_max - iou_x_min) < 0 || (iou_y_max - iou_y_min) < 0 ) iou_score = 0;

    return iou_score;

}


