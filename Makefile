# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lm

# Target and object files
TARGET = application
OBJS = application.o iom361_r2.o tempHumidTree.o float_rndm.o

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile individual source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove build files
clean:
	rm -f *.o $(TARGET)
