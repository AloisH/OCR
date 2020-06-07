#ifndef XOR2_H
#define XOR2_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

typedef struct Network Network;
struct Network
{

  int nbinput;
  int nboutput;
  int nbhidden;

  int *input;

  double *hidden;

  double *output;

  double **inputweights;

  double **hiddenweights;

  double *error;

  char defchar;

};

void FreeNet(Network net);

double Xor(int a, int b);

double sigmoid(double z);

double sigmoidPrime(double z);

double dotfloat(double coeff, double *a, int len);

Network *makeNetWork(int nbinputs, int nboutputs,int nbhidden);

void backprop(Network *net);

void feedforward(Network *net);

char CharResult(Network *net);
void xor_final();

void printOption();

void parse_weights(Network *net);

void save_weights(Network *net);

void printWeights(Network *net);

double rando();

Network trainNet(Network net);

#endif
