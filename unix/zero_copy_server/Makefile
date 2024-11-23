CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -luring

TARGET = zero_copy_server
SRCS = main.c zero_copy_server.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)