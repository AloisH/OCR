#ifndef IMG_UTILITY_H_
#define IMG_UTILITY_H_

#include <stdlib.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
void update_surface(SDL_Surface* screen, SDL_Surface* image);

void init_sdl();

SDL_Surface* load_image(char *path);

SDL_Surface* display_image(SDL_Surface *img);

void wait_for_keypressed();


#endif
