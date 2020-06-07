#include <err.h>
#include "image_processing.h"
#include "img_utility.h"
#include <stddef.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL/SDL_image.h>
#include "../neuralNetwork/xor2.h"
void grayscale(SDL_Surface* image_surface, size_t width, size_t height)
{
	// shade of gray
	for(size_t i = 0; i < width; i++)
	{
		for(size_t j = 0; j < height; j++)
		{
			Uint32 pixel = get_pixel(image_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			Uint8 average = 0.3*r + 0.59*g + 0.11*b;
			pixel = SDL_MapRGB(image_surface->format, average, average, average);
			put_pixel(image_surface, i, j, pixel);
		}
	}

}

void img_binarisation(SDL_Surface* image_surface, int** binarisation, size_t width, size_t height)
{	

	// Binarisation + white and black
	for(size_t i = 0; i < width; i++)
	{
		for(size_t j = 0; j < height; j++)
		{
			Uint32 pixel = get_pixel(image_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			Uint8 a = r / 188;
			if(a < 1)
			{
				pixel = SDL_MapRGB(image_surface->format, 0, 0, 0);
				binarisation[i][j] = 1;
			}
			else
			{
				pixel = SDL_MapRGB(image_surface->format, 255, 255, 255);
				binarisation[i][j] = 0;
			}

			put_pixel(image_surface, i,j, pixel);
		}
	}
}

size_t get_border(int** binarisation, size_t width, size_t height)
{
	size_t nb_border = 0;
	for (size_t i = 0; i < width; i++)
	{
		size_t sum = 0;
		for (size_t j = 0; j < height; j++)
		{
			sum += binarisation[i][j];
		}
		if (sum == height)
		{
			nb_border++;
		}
		else
		{
			break;
		}
	}
	return nb_border;
}


size_t cut(int** binarisation, SDL_Surface* image_surface, size_t width, size_t height, size_t nb_border, size_t nb_border_height, int** charactere)
{
	// Segmentation
	int line[600];
	size_t index_line = 0;
	size_t tmp_prev = INT32_MAX;
	for(size_t i = nb_border_height + 2; i < height - 1 - nb_border_height; i++)
	{
		size_t value = 0;
		for(size_t j = 0; j < width; j++)
		{
			value += binarisation[j][i];
		}
		if(tmp_prev <= nb_border * 2)
		{
			if(value > nb_border * 2)
			{
				// draw line
				for(size_t j = 0; j < width; j++)
				{
					Uint32 pixel = SDL_MapRGB(image_surface->format,255,0,0);
					put_pixel(image_surface, j, i-1, pixel);
				}
				line[index_line] = i-1;
				index_line += 1;			
			}
		}
		tmp_prev = value;

		size_t tmp_next = 0;
		for(size_t j = 0; j < width; j++)
		{
			tmp_next += binarisation[j][i+1];
		}
		if(value > nb_border * 2)
		{
			if(tmp_next <= nb_border * 2)
			{
				for(size_t j = 0; j < width; j++)
				{
					Uint32 pixel = SDL_MapRGB(image_surface->format,255,0,0);
					put_pixel(image_surface, j, i+1, pixel);
				}
				line[index_line] = i+1;
				index_line += 1;
			}
		}
	}

	size_t index_tab = 0;	

	for(size_t i = 0; i < index_line; i += 2)
	{	
		size_t begin = line[i];
		size_t end = line[i+1];
		size_t tmp_previous = 1;
		for(size_t j = nb_border;  j < width - 1; j++)
		{
			size_t value = 0;
			for(size_t k = begin; k < end; k++)
			{
				value += binarisation[j][k];	
			}

			if(value)
			{
				if(tmp_previous == 0)
				{
					charactere[index_tab] = malloc(sizeof(int*) * 4);
					charactere[index_tab][0] = begin;
					charactere[index_tab][1] = end;
					charactere[index_tab][2] = j;
					for(size_t k = begin; k < end; k++)
					{
						Uint32 pixel = SDL_MapRGB(image_surface->format, 0,255,0);
						put_pixel(image_surface,j,k, pixel);
					}	
				}

				size_t next = 0;
				for(size_t k = begin; k < end; k++)
				{
					next += binarisation[j+1][k];
				}
				if(next == 0)
				{
					for(size_t k = begin; k < end; k++)
					{
						Uint32 pixel = SDL_MapRGB(image_surface->format, 0,255,0);
						put_pixel(image_surface,j+1,k, pixel);
					}
					charactere[index_tab][3] = j;
					index_tab += 1;
				}
			}

			tmp_previous = value;
		}

	}
	return index_tab;
}


void create_char_mat(int** binarisation, int *charactere, int *char_mat)
{
	int top = charactere[0];
	int bot = charactere[1];
	int left = charactere[2];
	int right = charactere[3];

	int not_res[(bot-top) * (right-left)];
	int rml = (right - left);
	for(int i = 0;i < (bot-top); i++)
	{
		for(int j = 0; j < (right-left); j++)
		{
			not_res[i * rml + j] = binarisation[left+j][top+i];
		}
	}

	float k1 = (float)(bot - top) / 32;
	float k2 = (float)(right - left) / 32;



	// Si taille sup a 32*32
	if((k1 + k2) >= 2)
	{
		for(int i = 0;i < 32; i++)
		{
			for(int j = 0; j < 32; j++)
			{
				float tmp =  ((k1 * i)) * rml;
				float tmp2 = (k2*j);
				int tmp1 = (int)tmp + (int)tmp2;
				char_mat[i * 32 + j] = not_res[tmp1];
			}
		}	
	}
	// SINON
	else
	{
		for(int i = 0; i < 32; i++)
		{
			for(int j = 0; j < 32; j++)
			{
				char_mat[i * 32 + j] = 0;
				if(i < (bot - top) && (j < (right - left)))
				{
					char_mat[i * 32 + j] = not_res[i * rml + j];
				}
			}
		}
	}
}

char get_char(int** binarisation, int* charactere,Network *net)
{
	int *char_mat = malloc(sizeof(int*) * (32 * 32));
	create_char_mat(binarisation, charactere, char_mat);
	for (int i = 0; i < net->nbinput; i++)
	{
		net->input[i] = char_mat[i];
	}
	feedforward(net);

	char result = CharResult(net);

	free(char_mat);

	return result;
}

void final_txt(int** binarisation, int** charactere, char path[], size_t index_tab,Network *net)
{

	FILE *fptr;
	fptr = fopen(path, "w");
	if (fptr == NULL)
	{
		printf("Error while opening file");
	}
	else
	{
		for(size_t i = 0; i < index_tab - 1; i++)
		{
			char c = get_char(binarisation, charactere[i],net);
			fprintf(fptr, "%c", c);


			int diff = charactere[i + 1][2] - charactere[i][3];

			//check is there a new line

			if(charactere[i][0] != charactere[i + 1][0])
			{
				fprintf(fptr, "\n");

				int tab = charactere[i+1][2] / 80;
				for(int i = 0; i <= tab; i++)
				{
					fprintf(fptr, "\t");
				}
			}

			//check if there is a space

			else if(diff >= 7)
			{
				fprintf(fptr, " ");
			}
		}

		fprintf(fptr, "%c", get_char(binarisation, charactere[index_tab - 1],net));
		fclose(fptr);
	}
}

int image_processing(char s[],Network *net)
{
	SDL_Surface* image_surface;

	init_sdl();

	image_surface = load_image(s);

	size_t width = image_surface->w;
	size_t height = image_surface->h;

	grayscale(image_surface, width, height);

	int **binarisation = NULL;
	binarisation = malloc(sizeof(int*) * width);
	for(size_t i = 0; i < width; i++)
	{
		binarisation[i] = malloc(sizeof(int) * height);
	}

	img_binarisation(image_surface, binarisation, width, height);	
	
	SDL_SaveBMP(image_surface, "bin.bmp");
	size_t nb_border = get_border(binarisation, width, height);
	size_t nb_border_height = get_border(binarisation, width, height);

	int** charactere = NULL;
	charactere = malloc(sizeof(int*) * width * height);
	
	size_t index_tab = cut(binarisation, image_surface, width, height, nb_border, nb_border_height, charactere);
	SDL_SaveBMP(image_surface, "seg.bmp");
	
	char path[] = "result.txt";

	final_txt(binarisation, charactere, path, index_tab, net);

	//Saving binarisation array into a txt file

	FILE *fptr;
	fptr = fopen("binarized_file.txt", "w");
	size_t i = 0;
	size_t j = 0;
	if (fptr == NULL)
	{
		printf("Error while opening file");
	}
	else
	{
		while (i < height)
		{
			j = 0;
			while (j < width)
			{
				fprintf(fptr, "%i ", binarisation[j][i]);
				j++;
			}
			fprintf(fptr, "\n");
			i++;
		}
		fclose(fptr);
	}

	//Saving charactere array into a txt file

	FILE *fcptr;
	fcptr = fopen("chars.txt", "w");
	if (fcptr == NULL)
	{
		printf("Error while opening file");
	}
	else
	{
		for (size_t i = 0; i < index_tab; i++)
		{
			fprintf(fcptr, "%i %i %i %i \n", charactere[i][0], charactere[i][1], charactere[i][2], charactere[i][3]);
		}
		fclose(fcptr);
	}

	for (size_t i = 0; i < width; i++)
	{
		free(binarisation[i]);
	}

	free(binarisation);

	for (size_t i = 0; i < index_tab; i++)
	{
		free(charactere[i]);
	}

	free(charactere);

	SDL_FreeSurface(image_surface);

	return 0;
}
