CC = gcc
# DEBUG = 1
LIB_DIR := ../lib

TARGET = server
CFS = -D SERVER
# LINKGST = `pkg-config --cflags --libs gstreamer-1.0`
LINKSQL = `pkg-config --cflags --libs mysqlclient`

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -O0
else
CFS += -O3
endif

$(TARGET): server.o $(LIB_DIR)/db.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o
	$(CC) $(CFS) -o $(TARGET) server.o $(LIB_DIR)/db.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o $(LINKSQL)

server.o: server.c $(LIB_DIR)/dgram.h $(LIB_DIR)/db.h $(LIB_DIR)/protocol.h
	$(CC) $(CFS) -c server.c $(LINKSQL)

$(LIB_DIR)/db.o: $(LIB_DIR)/db.c $(LIB_DIR)/db.h $(LIB_DIR)/deskshare.h
	(cd $(LIB_DIR) && \
	$(CC) $(CFS) -c db.c $(LINKSQL))

$(LIB_DIR)/dgram.o: $(LIB_DIR)/dgram.c $(LIB_DIR)/dgram.h $(LIB_DIR)/deskshare.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c dgram.c $(LINKSQL))

$(LIB_DIR)/protocol.o: $(LIB_DIR)/protocol.c $(LIB_DIR)/protocol.h $(LIB_DIR)/deskshare.h
	(cd $(LIB_DIR)/ && \
	$(CC) $(CFS) -c protocol.c $(LINKSQL))

clean:
	(cd $(LIB_DIR)/ && \
	rm -f db.o protocol.o dgram.o)
	rm -f server.o
