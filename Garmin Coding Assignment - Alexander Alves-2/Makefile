CC = gcc
CFLAGS = -Wall -Wextra
TARGET = compression_testing
OBJS = run_length_encoding.o compression_testing.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)