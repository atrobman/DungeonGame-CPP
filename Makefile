CC             := g++
COMPILER_FLAGS := -Wall -g
LINKER_FLAGS   := -lSDL2 -lSDL2_image -lSDL2_ttf
EXE            := dgr
RMDIR          := rm
OBJ            := ./obj
SRC            := ./src
SRCS           := $(wildcard $(SRC)/*.cpp)
OBJS           := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

.PHONY: all clean

all: $(OBJS)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE)

clean:
	$(RMDIR) $(OBJ)/* $(EXE)

template: $(SRC)/tile.cpp $(SRC)/tile.hpp $(SRC)/templates.cpp $(SRC)/templates.hpp
	$(CC) $(SRC)/tile.cpp $(SRC)/templates.cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) -o template

$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	$(CC) -c $< $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@
