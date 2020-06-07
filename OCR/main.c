#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <math.h>
#include "imageProcessing/img_utility.h"
#include "neuralNetwork/xor2.h"
#include "imageProcessing/image_processing.h"

#include <gtk/gtk.h>

GtkBuilder *builder;
gchar *path;
GtkImage *image;
Network *net;
GtkWidget *window2;
GtkLabel *result_label;
int** charactere = NULL;
int **binarisation = NULL;


size_t parse(char *text)
{
	FILE *fptr;
	fptr = fopen("result.txt", "r");
	size_t i = 0;
	if(fptr == NULL)
		printf("Error while reading file");
	else
	{
		char letter;
		char *letter_ptr = &letter;

		while(fscanf(fptr,"%c",letter_ptr) != EOF)
		{
			text[i] = *letter_ptr;
			i++;
		}
	}

	return i;
}

void on_bin()
{
	if(path != NULL)
	{
		gtk_image_set_from_file(image,"bin.bmp");
	}
}

void on_seg()
{
	if(path != NULL)
	{
		gtk_image_set_from_file(image,"seg.bmp");
	}
}

void on_train()
{
	// Trainning
	srand(time(NULL));
	int train = 50000;
	int *char_mat = malloc(sizeof(int*) * (32*32));
	char caract[] = "AZERTYUlOPQSDFGHJKLMWXCVBN1234567890.!?:;,azertyuiopqsdfghjklmwxcvbn";
	while (train > 0)
	{
		int rng = rand()%(68);
		net->defchar = caract[rng]; 
		create_char_mat(binarisation, charactere[rng], char_mat);
		for(size_t i = 0; i < 1024; i++)
		{
			net->input[i] = char_mat[i];
		}
		// a changer en void
		feedforward(net);
		backprop(net);
		if(train % 1000 == 0)
		{
			for (int i = 0; i < net->nboutput; i++)
			{
				printf("error = %f\n",net->error[i]);
			}
			printf("Remaining: %i\n", train);
			save_weights(net);
		}
		train--;
	}
	printf("Trainning Done !\n");
	printf("Remaining: %i\n", train);
}

void on_quit_2()
{
	gtk_widget_hide(window2);
}

void on_ocr()
{
	if (path)
	{
		gtk_widget_show(window2);
		char *text = calloc(sizeof(char), 50000);
		parse(text);
		gtk_label_set_text(result_label, text);
	}
}

void on_quit()
{
	gtk_main_quit();
}

void on_image_set(GtkFileChooser *file)
{
	path =  gtk_file_chooser_get_filename (file);
	gtk_image_set_from_file(image,path);	
	image_processing(path,net);
}

int main(int argc, char *argv[])
{
	// ======================================
	// Create Network and Train it
	// ======================================

	// Open the trainning image
	SDL_Surface* image_surface;
	init_sdl();
	image_surface = load_image("images/train.jpg");
	size_t width = image_surface->w;
	size_t height = image_surface->h;
	grayscale(image_surface, width, height);

	binarisation = malloc(sizeof(int*) * width);
	for(size_t i = 0; i < width; i++)
	{
		binarisation[i] = malloc(sizeof(int) * height);
	}

	img_binarisation(image_surface, binarisation, width, height);	
	size_t nb_border = get_border(binarisation, width, height);
	size_t nb_border_height = get_border(binarisation, width, height);

	charactere = malloc(sizeof(int*) * width * height);
	cut(binarisation, image_surface, width, height, nb_border, nb_border_height, charactere);

	// Create the Network
	net = makeNetWork(1024,68,69);
	parse_weights(net);

	printf("Successful load\n");


	// ======================================
	// GTK Init 
	// ======================================

	GtkWidget *window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();

	gtk_builder_add_from_file(builder, "mainuiTEST.glade", NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

	// Filter
	GtkFileFilter *filter =  gtk_file_filter_new ();
	gtk_file_filter_set_name(filter, "Image");
	gtk_file_filter_add_pixbuf_formats(filter);

	// Gets the widgets
	GtkButton *quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
	GtkButton *seg_button = GTK_BUTTON(gtk_builder_get_object(builder, "seg_button"));
	GtkButton *bin_button = GTK_BUTTON(gtk_builder_get_object(builder, "bin_button"));
	GtkButton *ocr_button = GTK_BUTTON(gtk_builder_get_object(builder, "ocr_button"));
	GtkButton *training_button = GTK_BUTTON(gtk_builder_get_object(builder, "trainning_button"));
	GtkFileChooserButton *image_choose = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder,"image_choose"));
	GtkFileChooser *image_file = GTK_FILE_CHOOSER(image_choose);
	image = GTK_IMAGE(gtk_builder_get_object(builder,"image"));

	gtk_file_chooser_add_filter(image_file,filter);

	// Connects event handlers
	g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit), NULL);
	g_signal_connect(seg_button, "clicked", G_CALLBACK(on_seg), NULL);
	g_signal_connect(bin_button, "clicked", G_CALLBACK(on_bin), NULL);
	g_signal_connect(training_button, "clicked", G_CALLBACK(on_train), NULL);
	g_signal_connect(ocr_button, "clicked", G_CALLBACK(on_ocr), NULL);
	g_signal_connect(image_choose, "file-set", G_CALLBACK(on_image_set), image_file);

	// Second Window

	window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window_final"));
	GtkButton *quit_button_2 = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button_2"));
	g_signal_connect(quit_button_2, "clicked", G_CALLBACK(on_quit_2), NULL);
	result_label = GTK_LABEL(gtk_builder_get_object(builder, "Final"));


	gtk_widget_show(window);

	gtk_main();

	return 0;

}
