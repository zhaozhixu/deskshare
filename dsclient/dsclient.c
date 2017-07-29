#include <gst/gst.h>


/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData {
     GstElement *pipeline;
     GstElement *videosource, *videoconvert, *videodec;
     GstElement *videoqueue, *videodepay, *videosink;
     GstElement *audiosource, *audioconvert, *audiodec;
     GstElement *audioqueue, *audiodepay, *audiosink;
} CustomData;

/* Handler for the pad-added signal */
static void dec_pad_added_handler (GstElement *src, GstPad *pad, CustomData *data);

int main(int argc, char *argv[]) {
     CustomData data;
     GstBus *bus;
     GstMessage *msg;
     GstStateChangeReturn ret;
     GstCaps *videosrccaps;
     GstCaps *audiosrccaps;
     gboolean terminate = FALSE;

     /* Initialize GStreamer */
     gst_init (&argc, &argv);

     /* Create the elements */
     data.videosource = gst_element_factory_make ("udpsrc", "videosource");
     data.videoconvert = gst_element_factory_make ("videoconvert", "videoconvert");
     data.videodec = gst_element_factory_make ("decodebin", "videodec");
     data.videoqueue = gst_element_factory_make ("queue", "videoqueue");
     data.videodepay = gst_element_factory_make ("rtph264depay", "videodepay");
     data.videosink = gst_element_factory_make ("autovideosink", "videosink");

     data.audiosource = gst_element_factory_make ("udpsrc", "audiosource");
     data.audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
     data.audiodec = gst_element_factory_make ("amrnbdec", "audiodec");
     data.audioqueue = gst_element_factory_make ("queue", "audioqueue");
     data.audiodepay = gst_element_factory_make ("rtpamrdepay", "audiodepay");
     data.audiosink = gst_element_factory_make ("alsasink", "audiosink");

     /* Create the empty pipeline */
     data.pipeline = gst_pipeline_new ("test-pipeline");

     if (!data.pipeline || !data.videosource || !data.videoconvert
         || !data.videodec || !data.videodepay || !data.videoqueue || !data.videosink
         || !data.audiosource || !data.audioconvert || !data.audiodec
         || !data.audioqueue || !data.audiodepay || !data.audiosink) {
          g_printerr ("Not all elements could be created.\n");
          return -1;
     }

     /* Build the pipeline. */
     gst_bin_add_many (GST_BIN (data.pipeline), data.videosource, data.videoqueue,
                       data.videodepay, data.videodec, data.videoconvert,
                       data.videosink,
                       data.audiosource, data.audioqueue, data.audiodepay,
                       data.audiodec, data.audioconvert,
                       data.audiosink, NULL);
     if (!gst_element_link_many (data.videosource, data.videoqueue, data.videodepay,
                                  data.videodec, NULL) ||
         !gst_element_link_many (data.videoconvert, data.videosink, NULL) ||
         !gst_element_link_many (data.audiosource, data.audioqueue, data.audiodepay,
                                 data.audiodec, data.audioconvert,
                                 data.audiosink, NULL)) {
          g_printerr ("Audio elements could not be linked.\n");
          gst_object_unref (data.pipeline);
          return -1;
     }

     videosrccaps = gst_caps_new_simple ("application/x-rtp",
                                         "media", G_TYPE_STRING, "video",
                                         "clock-rate", G_TYPE_INT, 90000,
                                         "encoding-name", G_TYPE_STRING, "H264",
                                         "payload", G_TYPE_INT, 96, NULL);
     audiosrccaps = gst_caps_new_simple ("application/x-rtp",
                                         "media", G_TYPE_STRING, "audio",
                                         "clock-rate", G_TYPE_INT, 8000,
                                         "encoding-name", G_TYPE_STRING, "AMR",
                                         "encoding-params", G_TYPE_STRING, "1",
                                         "octet-align", G_TYPE_STRING, "1", NULL);
     /* set properties */
     g_object_set (data.videosource, "port", 5000, "caps", videosrccaps, NULL);
     g_object_set (data.audiosource, "port", 5002, "caps", audiosrccaps,  NULL);
     /* gst_object_unref (videosrccaps); */
     /* gst_object_unref (audiosrccaps); */

     /* Connect to the pad-added signal */
     g_signal_connect (data.videodec, "pad-added", G_CALLBACK (dec_pad_added_handler), &data);

     /* Start playing */
     ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
     if (ret == GST_STATE_CHANGE_FAILURE) {
          g_printerr ("Unable to set the pipeline to the playing state.\n");
          gst_object_unref (data.pipeline);
          return -1;
     }

     /* Listen to the bus */
     bus = gst_element_get_bus (data.pipeline);
     do {
          msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                            GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

          /* Parse message */
          if (msg != NULL) {
               GError *err;
               gchar *debug_info;

               switch (GST_MESSAGE_TYPE (msg)) {
               case GST_MESSAGE_ERROR:
                    gst_message_parse_error (msg, &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;
                    break;
               case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    terminate = TRUE;
                    break;
               case GST_MESSAGE_STATE_CHANGED:
                    /* We are only interested in state-changed messages from the pipeline */
                    if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
                         GstState old_state, new_state, pending_state;
                         gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                         g_print ("Pipeline state changed from %s to %s:\n",
                                  gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                    }
                    break;
               default:
                    /* We should not reach here */
                    g_printerr ("Unexpected message received.\n");
                    break;
               }
               gst_message_unref (msg);
          }
     } while (!terminate);

     /* Free resources */
     gst_object_unref (bus);
     gst_element_set_state (data.pipeline, GST_STATE_NULL);
     gst_object_unref (data.pipeline);
     return 0;
}

/* This function will be called by the pad-added signal */
static void dec_pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data) {
     GstPad *sink_pad = gst_element_get_static_pad (data->videoconvert, "sink");
     GstPadLinkReturn ret;
     GstCaps *new_pad_caps = NULL;
     GstStructure *new_pad_struct = NULL;
     const gchar *new_pad_type = NULL;

     g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

     /* If our converter is already linked, we have nothing to do here */
     if (gst_pad_is_linked (sink_pad)) {
          g_print ("  We are already linked. Ignoring.\n");
          goto exit;
     }

     /* Check the new pad's type */
     new_pad_caps = gst_pad_query_caps (new_pad, NULL);
     new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
     new_pad_type = gst_structure_get_name (new_pad_struct);
     if (!g_str_has_prefix (new_pad_type, "video/x-raw")) {
          g_print ("  It has type '%s' which is not video/x-raw. Ignoring.\n", new_pad_type);
          goto exit;
     }

     /* Attempt the link */
     ret = gst_pad_link (new_pad, sink_pad);
     if (GST_PAD_LINK_FAILED (ret)) {
          g_print ("  Type is '%s' but link failed.\n", new_pad_type);
     } else {
          g_print ("  Link succeeded (type '%s').\n", new_pad_type);
     }

exit:
     /* Unreference the new pad's caps, if we got them */
     if (new_pad_caps != NULL)
          gst_caps_unref (new_pad_caps);

     /* Unreference the sink pad */
     gst_object_unref (sink_pad);
}
