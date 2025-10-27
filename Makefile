CC = gcc
CFLAGS = -Wall
TARGET = annotated_compression

all: $(TARGET)

$(TARGET): clean
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)