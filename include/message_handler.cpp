//
// Created by ek on 12.08.2022.
//

#include "message_handler.h"

namespace rtsphandler {

    gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
        GMainLoop *loop = (GMainLoop *) data;
        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
                g_print("End of stream\n");
                g_main_loop_quit(loop);
                break;
            case GST_MESSAGE_WARNING: {
                gchar *debug;
                GError *error;
                gst_message_parse_warning(msg, &error, &debug);
                g_printerr("WARNING from element %s: %s\n",
                           GST_OBJECT_NAME(msg->src), error->message);
                g_free(debug);
                g_printerr("Warning: %s\n", error->message);
                g_error_free(error);
                break;
            }
            case GST_MESSAGE_ERROR: {
                gchar *debug;
                GError *error;
                gst_message_parse_error(msg, &error, &debug);
                g_printerr("ERROR from element %s: %s\n",
                           GST_OBJECT_NAME(msg->src), error->message);
                if (debug)
                    g_printerr("Error details: %s\n", debug);
                g_free(debug);
                g_error_free(error);
                g_main_loop_quit(loop);
                break;
            }
            case GST_MESSAGE_ELEMENT: {
                if (gst_nvmessage_is_stream_eos(msg)) {
                    guint stream_id;
                    if (gst_nvmessage_parse_stream_eos(msg, &stream_id)) {
                        g_print("Got EOS from stream %d\n", stream_id);
                    }
                }
                break;
            }
            default:
                break;
        }
        return TRUE;
    }


    void cb_newpad(GstElement *decodebin, GstPad *decoder_src_pad, gpointer data) {
        GstCaps *caps = gst_pad_get_current_caps(decoder_src_pad);
        if (!caps) {
            caps = gst_pad_query_caps(decoder_src_pad, NULL);
        }
        const GstStructure *str = gst_caps_get_structure(caps, 0);
        const gchar *name = gst_structure_get_name(str);
        GstElement *source_bin = (GstElement *) data;
        GstCapsFeatures *features = gst_caps_get_features(caps, 0);

        /* Need to check if the pad created by the decodebin is for video and not
         * audio. */
        if (!strncmp(name, "video", 5)) {
            /* Link the decodebin pad only if decodebin has picked nvidia
             * decoder plugin nvdec_*. We do this by checking if the pad caps contain
             * NVMM memory features. */
            if (gst_caps_features_contains(features, GST_CAPS_FEATURES_NVMM)) {
                /* Get the source bin ghost pad */
                GstPad *bin_ghost_pad = gst_element_get_static_pad(source_bin, "src");
                if (!gst_ghost_pad_set_target(GST_GHOST_PAD(bin_ghost_pad),
                                              decoder_src_pad)) {
                    g_printerr("Failed to link decoder src pad to source bin ghost pad\n");
                }
                gst_object_unref(bin_ghost_pad);
            } else {
                g_printerr("Error: Decodebin did not pick nvidia decoder plugin.\n");
            }
        }
    }

    void decodebin_child_added(GstChildProxy *child_proxy, GObject *object,
                                      gchar *name, gpointer user_data) {
        g_print("Decodebin child added: %s\n", name);
        if (g_strrstr(name, "decodebin") == name) {
            g_signal_connect(G_OBJECT(object), "child-added",
                             G_CALLBACK(decodebin_child_added), user_data);
        }
    }

    GstElement *create_source_bin(guint index, gchar *uri) {
        GstElement *bin = NULL, *uri_decode_bin = NULL;
        gchar bin_name[16] = {};

        g_snprintf(bin_name, 15, "source-bin-%02d", index);
        /* Create a source GstBin to abstract this bin's content from the rest of the
         * pipeline */
        bin = gst_bin_new(bin_name);

        /* Source element for reading from the uri.
         * We will use decodebin and let it figure out the container format of the
         * stream and the codec and plug the appropriate demux and decode plugins. */
        if (PERF_MODE) {
            uri_decode_bin = gst_element_factory_make("nvurisrcbin", "uri-decode-bin");
            g_object_set(G_OBJECT(uri_decode_bin), "file-loop", TRUE, NULL);
        } else {
            uri_decode_bin = gst_element_factory_make("uridecodebin", "uri-decode-bin");
        }

        if (!bin || !uri_decode_bin) {
            g_printerr("One element in source bin could not be created.\n");
            return NULL;
        }

        /* We set the input uri to the source element */
        g_object_set(G_OBJECT(uri_decode_bin), "uri", uri, NULL);

        /* Connect to the "pad-added" signal of the decodebin which generates a
         * callback once a new pad for raw data has beed created by the decodebin */
        g_signal_connect(G_OBJECT(uri_decode_bin), "pad-added",
                         G_CALLBACK(cb_newpad), bin);
        g_signal_connect(G_OBJECT(uri_decode_bin), "child-added",
                         G_CALLBACK(decodebin_child_added), bin);

        gst_bin_add(GST_BIN(bin), uri_decode_bin);

        /* We need to create a ghost pad for the source bin which will act as a proxy
         * for the video decoder src pad. The ghost pad will not have a target right
         * now. Once the decode bin creates the video decoder and generates the
         * cb_newpad callback, we will set the ghost pad target to the video decoder
         * src pad. */
        if (!gst_element_add_pad(bin, gst_ghost_pad_new_no_target("src",
                                                                  GST_PAD_SRC))) {
            g_printerr("Failed to add ghost pad in source bin\n");
            return NULL;
        }

        return bin;
    }


    gboolean start_rtsp_streaming (guint rtsp_port_num, guint updsink_port_num,
                                          guint64 udp_buffer_size)
    {
        GstRTSPMountPoints *mounts;
        GstRTSPMediaFactory *factory;
        char udpsrc_pipeline[512];

        char port_num_Str[64] = { 0 };
        char *encoder_name;

        if (udp_buffer_size == 0)
            udp_buffer_size = 512 * 1024;

        sprintf (udpsrc_pipeline,
                 "( udpsrc name=pay0 port=%d buffer-size=%lu caps=\"application/x-rtp, media=video, "
                 "clock-rate=90000, encoding-name=H264, payload=96 \" )",
                 updsink_port_num, udp_buffer_size);

        sprintf (port_num_Str, "%d", rtsp_port_num);

        server = gst_rtsp_server_new ();
        g_object_set (server, "service", port_num_Str, NULL);

        mounts = gst_rtsp_server_get_mount_points (server);

        factory = gst_rtsp_media_factory_new ();
        gst_rtsp_media_factory_set_launch (factory, udpsrc_pipeline);

        gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

        g_object_unref (mounts);

        gst_rtsp_server_attach (server, NULL);

        g_print("\n *** DeepStream: Launched RTSP Streaming at rtsp://localhost:%d/test ***\n\n", rtsp_port_num);

        return TRUE;
    }

    GstRTSPFilterResult client_filter (GstRTSPServer * server, GstRTSPClient * client,
                                              gpointer user_data)
    {
        return GST_RTSP_FILTER_REMOVE;
    }

    void destroy_sink_bin ()
    {
        GstRTSPMountPoints *mounts;
        GstRTSPSessionPool *pool;

        mounts = gst_rtsp_server_get_mount_points (server);
        gst_rtsp_mount_points_remove_factory (mounts, "/test");
        g_object_unref (mounts);
        gst_rtsp_server_client_filter (server, client_filter, NULL);
        pool = gst_rtsp_server_get_session_pool (server);
        gst_rtsp_session_pool_cleanup (pool);
        g_object_unref (pool);
    }




}// namespace