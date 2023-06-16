//
// Created by ek on 30.07.2022.
//

#ifndef DETTRACKPL_MESSAGE_HANDLER_H
#define DETTRACKPL_MESSAGE_HANDLER_H

#include <gst/gst.h>
#include <sstream>
#include "gst-nvmessage.h"
#include <gst/rtsp-server/rtsp-server.h>



#define GST_CAPS_FEATURES_NVMM "memory:NVMM"



namespace rtsphandler {

    static gboolean PERF_MODE = FALSE;


    static GstRTSPServer *server;

    gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

    GstElement *create_source_bin(guint index, gchar *uri);

    gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

    GstRTSPFilterResult client_filter (GstRTSPServer * server, GstRTSPClient * client,
                                              gpointer user_data);
    void destroy_sink_bin ();

    gboolean start_rtsp_streaming (guint rtsp_port_num, guint updsink_port_num,
                                          guint64 udp_buffer_size);


} // namespace




#endif //DETTRACKPL_MESSAGE_HANDLER_H
