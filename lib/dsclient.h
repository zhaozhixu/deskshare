#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomDataR {
     GstElement *pipeline;
     GstElement *videosource, *videoconvert, *videodec;
     GstElement *videoqueue, *videodepay, *videosink;
     GstElement *audiosource, *audioconvert, *audiodec;
     GstElement *audioqueue, *audiodepay, *audiosink;
} CustomDataR;

int receiver_setup();
