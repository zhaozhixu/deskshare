CC = gcc
# DEBUG = 1
LIB_DIR := ../lib

TARGET = server
CFS =
LINKGST = `pkg-config --cflags --libs gstreamer-1.0`
LINKSQL = `pkg-config --cflags --libs mysqlclient`

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -O0
else
CFS += -O3
endif

$(TARGET): server.o $(LIB_DIR)/db.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o
	$(CC) $(CFS) -o $(TARGET) server.o db.o dgram.o protocol.o $(LINKGST) $(LINKSQL)

server.o: server.c
	$(CC) $(CFS) -c server.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/db.o: $(LIB_DIR)/db.c $(LIB_DIR)/db.h
	$(CC) $(CFS) -c $(LIB_DIR)/db.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/dgram.o: $(LIB_DIR)/dgram.c $(LIB_DIR)/dgram.h
	$(CC) $(CFS) -c $(LIB_DIR)/dgram.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/protocol.o: $(LIB_DIR)/protocol.c $(LIB_DIR)/protocol.h
	$(CC) $(CFS) -c $(LIB_DIR)/protocol.c $(LINKGST) $(LINKSQL)

clean:
	rm server.o db.o protocol.o dgram.o
