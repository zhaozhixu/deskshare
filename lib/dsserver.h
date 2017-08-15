#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it to callbacks */
/* typedef struct _CustomData { */
/*      GstElement *pipeline; */
/*      GstElement *videosource, *scale, *videoconvert, *videoenc; */
/*      GstElement *videoqueue, *videopay, *videosink; */
/*      GstElement *audiosource, *audioconvert, *audioenc; */
/*      GstElement *audioqueue, *audiopay, *audiosink; */
/* } CustomData; */

typedef struct _CustomDataT {
     GstElement *pipeline;
     GstElement *videosource, *scale, *videoconvert, *videoenc;
     GstElement *videoqueue, *videopay, *videosink;
     GstElement *audiosource, *audioconvert, *audioenc;
     GstElement *audioqueue, *audiopay, *audiosink;
} CustomDataT;

int transmitter_setup(gchar *host, gint port_5000, gint port_5002);
