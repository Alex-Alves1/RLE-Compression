CC = gcc
CFLAGS = -Wall
TARGET = compression

all: $(TARGET)

$(TARGET): clean
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)