CC = gcc
DEBUG = 1
TARGET = dsclient
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

$(TARGET).o: dsclient.c
	$(CC) $(CFS) -c dsclient.c $(LINK)

clean:
	rm $(TARGET).o
