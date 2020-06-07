# Define the default target
CC = gcc

CPPFLAGS =`pkg-config --cflags sdl` `pkg-config --libs --cflags gtk+-3.0` -MMD -Wno-format -Wno-deprecated-declarations -Wno-format-security
CFLAGS = -std=c99 -O3 -Wall -Wextra
LDFLAGS = 
LDLIBS = `pkg-config --libs sdl` -lSDL_image -lm `pkg-config --libs --cflags gtk+-3.0` -export-dynamic

OBJ = main.o neuralNetwork/xor2.o imageProcessing/img_utility.o imageProcessing/image_processing.o

all: main

# Define dependencies and compile information

main: $(OBJ)


clean:
	$(RM) $(OBJ)
	$(RM) main
	$(RM) *.d
	$(RM) imageProcessing/*.d
	$(RM) neuralNetwork/*.d
	$(RM) *.txt
