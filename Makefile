CC = gcc
CFLAGS = -Wall -Wextra
TARGET = compression_testing

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(TARGET).c run_length_encoding.c -o $(TARGET)

clean:
	rm -f $(TARGET)