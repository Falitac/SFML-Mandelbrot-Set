
MAIN = main
MAIN_SRC = $(MAIN).cpp
OBJ_DIR = ../obj/
MAIN_OBJ = $(OBJ_DIR)$(MAIN).o
BIN_DIR = ../bin/
BIN_OUT = $(BIN_DIR)Mandelbrot

COMPILER = g++
COMPILER_OPTIONS = -std=c++17 -Ofast

SFML_LIB = -lsfml-graphics
SFML_LIB += -lsfml-window
SFML_LIB += -lsfml-system
COMPILER_OPTIONS += $(SFML_LIB)

all: compile run

compile: $(MAIN_OBJ)
	$(COMPILER) $(MAIN_OBJ) -o $(BIN_OUT) $(COMPILER_OPTIONS) 

$(MAIN_OBJ):
	$(COMPILER) -c $(MAIN_SRC) -o $(MAIN_OBJ) $(COMPILER_OPTIONS) 

run:
	./$(BIN_OUT).*
clean:
	rm $(OBJ_DIR)*.o $(BIN_OUT).*
