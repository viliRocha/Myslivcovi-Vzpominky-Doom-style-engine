CC = g++
CFLAGS = -Wall -std=c++14
INCLUDE = -IC:/raylib/raylib/src
LIBS = -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm   # Windows

SRC = $(wildcard src/*.cpp)
OUT = Myslivcovi-Vzpominky.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(INCLUDE) $(LIBS)

run: $(OUT)
	./$(OUT)

clean:
	del $(OUT)