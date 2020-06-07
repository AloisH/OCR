#include "xor2.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int WhoIsThisPokemon(char c)
{
	char caract[] = "AZERTYUlOPQSDFGHJKLMWXCVBN1234567890.!?:;,azertyuiopqsdfghjklmwxcvbn";
	int i = 0;
	while (caract[i] != c)
		i++;
	return i ;
}
double Xor(int a, int b)
{
	if (a ==b)
		return 0.0;
	return 1.0;
}

double sigmoid(double z)
{  //explicit content
	return 1.0 / (1.0 + exp(-z));
}

double sigmoidPrime(double z)
{  //explicit content
	return z*(1-z);
}

double rando2()
{
	double rn = rand();
	while (rn > 1)
	{
		rn = rn / 10;
	}
	return ((rn * 2));
}

double rando()
{
	double result =(double)rand()/((double)RAND_MAX+1);
	int a = rand()%2;
	if (a)
		result *= -1;
	return result;
}

Network *makeNetWork(int nbinputs, int nboutputs,int nbhiddens)
{
	Network *net = malloc(sizeof(Network));
	net->nbinput = nbinputs;
	net->nboutput = nboutputs;
	net->nbhidden = nbhiddens;

	net->input = calloc(sizeof(int),nbinputs);

	net->hidden = calloc(sizeof(double),nbhiddens);

	net->output = calloc(sizeof(double),nboutputs);

	net->inputweights = malloc(sizeof(double*)*nbinputs);

	for (int i = 0; i < nbinputs;i++)
	{
		net->inputweights[i]= calloc(sizeof(double),nbhiddens);
	}

	net->hiddenweights = malloc(sizeof(double*)*nbhiddens);

	for (int i = 0; i < nbhiddens;i++)
	{
		net->hiddenweights[i]= calloc(sizeof(double),nboutputs);
	}

	net->error = calloc(sizeof(double),nboutputs);

	for (int i = 0 ; i < nbinputs; i++)
	{
		for (int j = 0; j < nbhiddens; j++)
		{
			net->inputweights[i][j] = rando();
			//printf("%f\n",net->inputweights[i][j]);
		}
	}
	for (int k = 0; k < nbhiddens; k++)
	{
		for (int i = 0; i < net->nboutput; i ++)
		{
			net->hiddenweights[k][i] = rando();
		}
	}
	return net;
}

char CharResult(Network *net)
{
	int max = 0;
	int i = 0;
	while (i < net->nboutput)
	{
		if (net->output[i] > net->output[max])
			max = i;
		i++;
	}
	char caract[] = "AZERTYUlOPQSDFGHJKLMWXCVBN1234567890.!?:;,azertyuiopqsdfghjklmwxcvbn";
	return caract[max];
}

//reprendre ici les remplacemets par ->

void feedforward(Network *net)
{
	for (int i = 0; i < net->nbhidden; i++)
	{
		double result = 0;
		for (int j = 0; j < net->nbinput; j++)
		{
			result += net->input[j] * net->inputweights[j][i];
		}
		net->hidden[i] = sigmoid(result);
	}
	for (int i = 0; i < net->nboutput; i++)
	{
		double result = 0;
		for (int j = 0; j < net->nbhidden; j++)
		{
			result += net->hidden[j] * net->hiddenweights[j][i];
		}
		net->output[i] = sigmoid(result);
	}
}

void backprop(Network *net)
{
	//printf("%fhello\n",net.output);
	int c = WhoIsThisPokemon(net->defchar);
	double adjustement[net->nboutput];
	for (int i = 0;i < net->nboutput;i++)
	{
		if (i == c)
			net->error[i] = net->output[i]-1;
		else
			net->error[i] = net->output[i];
		adjustement[i] = net->error[i]* sigmoidPrime(net->output[i])*0.5;
		if (net->error[i] < 0)
			net->error[i] *= -1;
	}
	double adjust[net->nbhidden][net->nboutput];
	for (int j = 0; j < net->nboutput; j++)
	{
		for (int i = 0; i < net->nbhidden; i ++)
		{
			adjust[i][j] = adjustement[j] * net->hiddenweights[i][j] 
				* sigmoidPrime(net->hidden[i]);
		}
	}

	//Updating weights_inputs

	for (int i = 0; i < net->nbhidden; i++)
	{
		for (int j = 0 ; j < net->nbinput; j++)
		{
			for (int k = 0; k< net->nboutput; k++)
				net->inputweights[j][i] -= (net->input[j] 
						* adjust[i][k]);
			//printf("%f\n",net->inputweights[j][i]);
		}
	}
	// Updating weights_hidden

	for (int i = 0 ; i < net->nbhidden; i ++)
	{
		for ( int j = 0; j< net->nboutput; j++)
			net->hiddenweights[i][j] -= (net->hidden[i] 
					* adjustement[j]);
	}
}

void printOption()
{
	printf("|========================================================|\n");
	printf("| Options:						 |\n");
	printf("| 0) Option						 |\n");
	printf("| 1) Weights						 |\n");
	printf("| 2) Trainning						 |\n");
	printf("| 3) Test output					 |\n");
	printf("| 4) Save						 |\n");
	printf("| 5) Load						 |\n");
	printf("| 6) Quit						 |\n");
	printf("|========================================================|\n");
}

void printWeights(Network *net)
{
	printf("| Inputs synaptic weights :\n");
	for(int j = 0; j < net->nbinput; j++)
	{
		for(int z = 0; z < net->nbhidden; z++)
		{
			printf("| Input_W[%i][%i] = %f \n" , j, z, net->inputweights[j][z]);
		}
	}
	printf("| Hidden synaptic weights :\n");
	for(int j = 0; j < net->nbhidden; j++)
	{
		for ( int i = 0; i < net->nboutput;i++)
		{
			printf("| Hidden_W[%i] = %f \n", j, net->hiddenweights[j][i]);
		}
	}
	printf("|========================================================|\n");
	printf("Input ; %i Hidden : %i Output : %i \n", net->nbinput, net->nbhidden, net->nboutput);
}

void parse_weights(Network *net)
{
	//Open the file
	FILE *fptr;
	fptr = fopen("weights_values.txt", "r");
	//Ite to interrate in weights, used to know if we are working on inputs_weights or hidden_weights
	int ite = 1;
	//index and value will get the values from the file (with the fscanf function)
	int index = 0;
	int index2 = 0;
	int *indexptr = NULL;
	int *indexptr2 = NULL;
	indexptr = &index;
	indexptr2 = &index2;
	int nb= net->nbinput*net->nbhidden;
	float value = 0;
	float *valueptr = NULL;
	valueptr = &value;
	//Check if there is a problem with the reading of the file, exit the function if it is the case
	if (fptr == NULL)
	{
		printf("Error while reading weights file");
	}
	else
	{
		//Get the index and the weight value from the line
		while ((fscanf(fptr,"%d %d %f",indexptr, indexptr2, valueptr)) != EOF)
		{
			//Check if the weight is an input weight or an hidden weight
			if (ite <= nb)
			{
				// j = units of the value / i = tens of the value
				double weight = *valueptr;
				net->inputweights[*indexptr][*indexptr2] = weight;
			}
			else
			{
				double weight = *valueptr;
				net->hiddenweights[*indexptr][*indexptr2] = weight;
			}
			ite++;
		}
		fclose(fptr);
	}
}

void save_weights(Network *net)
{
	//Open the file
	FILE *fptr;
	fptr = fopen("weights_values.txt", "w");
	//Create i and j to write down weights values in the file
	int i = 0;
	int j = 0;

	//printf("");

	//If there is a problem with the creation/modification of the file and stop the process of saving
	if (fptr == NULL)
	{
		printf("Error while opening weights file");
	}
	else
	{
		//iterrate in the two indexes
		while(i < net->nbinput)
		{
			j = 0;
			while(j < net->nbhidden)
			{
				//Write down index and value
				fprintf(fptr,"%lu %lu %f\n", i, j, net->inputweights[i][j]);
				j++;
			}
			i++;
		}
		i = 0;
		//same for hidden weights
		while (i < net->nbhidden)
		{
			j = 0;
			while (j < net->nboutput)
			{
				fprintf(fptr,"%lu %lu %f\n", i, j, net->hiddenweights[i][j]);
				j++;
			}
			i++;
		}
		fclose(fptr);
	}

}


void xor_final()
{
	/*srand(time(NULL));

	printf("|========================================================|\n");
	printf("|                  C, t'es sur en fait ?                 |\n");
	printf("|========================================================|\n");
	printf("|                   XOR - Soutenance 1                   |\n");
	printf("|========================================================|\n");


	int option;
	int x;
	int train;
	Network *net = makeNetWork(1024,68,400);

	printOption();

	printf("| Please select an option : ");
	int f = scanf("%d", &option);


	while (option != 6 && f)
	{
		switch(option)
		{
			case 0:
				printOption();
				break;

			case 1:
				printWeights(net);
				break;

			case 2:
				printf("| Itteration : ");
				break;			
			case 3:
				printf("| Enter the path of your picture: ");
				f = scanf("%d", &x);
				//segmentation(f)
				//recuperer matrice de limage
				//mettre les inputs en fct de la matrice
				feedforward(net);
				//printf("| Result : %c\n",CharResult(net->output,net->nboutput));
				break;

			case 4:
				save_weights(net);
				printf("| Weights saved successfully\n");
				printWeights(net);
				break;

			case 5:
				parse_weights(net);
				printf("| Weights loaded succefully\n");
				printWeights(net);
				break;
			default:
				printf("| Unkown Command!\n");
				break;
		}


		printf("| Please select an option : ");
		f = scanf("%d", &option);
	}*/
}
