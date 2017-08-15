CC = gcc
# DEBUG = 1
TARGET = client
LIB_DIR = ../lib
CFS =
LINKGST = `pkg-config --cflags --libs gstreamer-1.0`
LINKSQL = `pkg-config --cflags --libs mysqlclient`

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -O0
else
CFS += -O3
endif

$(TARGET): client.o $(LIB_DIR)/db.o $(LIB_DIR)/dgram.o $(LIB_DIR)/protocol.o $(LIB_DIR)/dsserver.o $(LIB_DIR)/dsclient.o
	$(CC) $(CFS) -o $(TARGET) client.o db.o dgram.o protocol.o dsserver.o dsclient.o $(LINKGST) $(LINKSQL)

client.o: client.c
	$(CC) $(CFS) -c client.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/dsserver.o: $(LIB_DIR)/dsserver.c $(LIB_DIR)/dsserver.h
	$(CC) $(CFS) -c $(LIB_DIR)/dsserver.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/dsclient.o: $(LIB_DIR)/dsclient.c $(LIB_DIR)/dsclient.h
	$(CC) $(CFS) -c $(LIB_DIR)/dsclient.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/db.o: $(LIB_DIR)/db.c $(LIB_DIR)/db.h
	$(CC) $(CFS) -c $(LIB_DIR)/db.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/dgram.o: $(LIB_DIR)/dgram.c $(LIB_DIR)/dgram.h
	$(CC) $(CFS) -c $(LIB_DIR)/dgram.c $(LINKGST) $(LINKSQL)

$(LIB_DIR)/protocol.o: $(LIB_DIR)/protocol.c $(LIB_DIR)/protocol.h
	$(CC) $(CFS) -c $(LIB_DIR)/protocol.c $(LINKGST) $(LINKSQL)

clean:
	rm $(TARGET).o
	rm db.o protocol.o dgram.o dsserver.o dsclient.o
