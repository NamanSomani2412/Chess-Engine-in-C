CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11
LDFLAGS = -L/mingw64/lib -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_dialog

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = $(BIN_DIR)/chess.exe

all: build_folders $(TARGET)

build_folders:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJECTS) obj/resource.o
	$(CXX) $(OBJECTS) obj/resource.o -o $(TARGET) -mwindows $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

obj/resource.o: resource.rc
	windres resource.rc -o obj/resource.o

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
