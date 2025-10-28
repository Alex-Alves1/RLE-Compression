CC = gcc
CFLAGS = -Wall -Wextra -c
TARGET = compression_testing
OBJS = run_length_encoding.o compression_testing.o

all: $(TARGET)

$(TARGET): compile
	$(CC) $(OBJS) -o $(TARGET)

compile:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET).o
	$(CC) $(CFLAGS) -c run_length_encoding.c -o run_length_encoding.o
	
clean:
	rm -f $(TARGET) $(OBJS)