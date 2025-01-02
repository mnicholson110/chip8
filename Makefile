TARGET = bin/chip8
SRC = $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin 
	gcc -Wall -Wextra -O3 -o $(TARGET) $(SRC) -lSDL2

clean:
	rm -rf bin/

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
