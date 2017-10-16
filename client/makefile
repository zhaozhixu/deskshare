CC = gcc
# DEBUG = 1
TARGET = client
LIB_DIR = ../lib
CFS = -D CLIENT
LINKGST = `pkg-config --cflags --libs gstreamer-1.0`
# LINKSQL = `pkg-config --cflags --libs mysqlclient`

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -O0
else
CFS += -O3
endif

$(TARGET): client.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o $(LIB_DIR)/dsclient.o $(LIB_DIR)/dsserver.o
	$(CC) $(CFS) -o $(TARGET) client.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o $(LIB_DIR)/dsclient.o $(LIB_DIR)/dsserver.o $(LINKGST)

client.o: client.c $(LIB_DIR)/dgram.h $(LIB_DIR)/protocol.h $(LIB_DIR)/dsclient.h $(LIB_DIR)/dsserver.h
	$(CC) $(CFS) -c client.c $(LINKGST)

$(LIB_DIR)/dsclient.o: $(LIB_DIR)/dsclient.c $(LIB_DIR)/dsclient.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c dsclient.c $(LINKGST))

$(LIB_DIR)/dsserver.o: $(LIB_DIR)/dsserver.c $(LIB_DIR)/dsserver.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c dsserver.c $(LINKGST))

$(LIB_DIR)/dgram.o: $(LIB_DIR)/dgram.c $(LIB_DIR)/dgram.h $(LIB_DIR)/deskshare.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c dgram.c $(LINKGST))

$(LIB_DIR)/protocol.o: $(LIB_DIR)/protocol.c $(LIB_DIR)/protocol.h $(LIB_DIR)/deskshare.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c protocol.c $(LINKGST))

clean:
	(cd $(LIB_DIR)/ && \
	rm -f protocol.o dgram.o)
	rm -f client.o
