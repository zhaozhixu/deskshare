CC = gcc
DEBUG = 1
TARGET = dsserver
CFS =
LINK = `pkg-config --cflags --libs gstreamer-1.0`

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -O0
else
CFS += -O3
endif

$(TARGET): $(TARGET).o
	$(CC) $(CFS) -o $(TARGET) $(TARGET).o $(LINK)

$(TARGET).o: dsserver.c
	$(CC) $(CFS) -c dsserver.c $(LINK)

clean:
	rm $(TARGET).o
