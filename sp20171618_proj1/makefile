CC = gcc
CFLAGS = -Wall -W -g
OBJECTS = 20171618.o 
TARGET = 20171618.out

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) $(TARGET) $(TARGET) .dSYM
