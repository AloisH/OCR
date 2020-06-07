#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <stddef.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL/SDL_image.h>
#include "../neuralNetwork/xor2.h"
void grayscale(SDL_Surface* image_surface, size_t width, size_t height);

void img_binarisation(SDL_Surface* image_surface,int** binarisation, size_t width, size_t height);

size_t get_border(int** binarisation, size_t witdh, size_t height);

size_t cut(int** binarisation, SDL_Surface* image_surface, size_t width, size_t height, size_t nb_border, size_t nb_border_height, int** charactere);

int image_processing(char s[],Network *net);

void create_char_mat(int** binarisation, int *charactere, int *char_mat);

void final_txt(int** binarisation, int** charactere, char path[], size_t index_tabi,Network *net);

char get_char(int** binarisation, int* charactere,Network *net);

#endif
