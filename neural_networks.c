// Copyright (C) 2017 Michal Drozd
// All Rights Reserved

// Program to show neural networks in C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "neural_networks.h"

// Global variables for the neural network
int num_inputs = 2; // Number of inputs to the neural network
int num_hidden = 3; // Number of hidden nodes in the neural network
int num_outputs = 1; // Number of outputs from the neural network
double learning_rate = 0.1; // Learning rate for the neural network
double momentum = 0.9; // Momentum for the neural network


// Function to initialize a new neural network with random weights and biases between -1 and 1
void init_neural_network(struct NeuralNetwork* nn)
{

	int i, j, k;

	// Initialize weights and biases for input to hidden layer with random values between -1 and 1
	for (i = 0; i < num_hidden; i++)
	{
		nn->bias_hidden[i] = ((double)rand() / (double)RAND_MAX) * 2 - 1;

		for (j = 0; j < num_inputs; j++)
		{
			nn->weights_input[j][i] = ((double)rand() / (double)RAND_MAX) * 2 - 1;
		}
	}

	// Initialize weights and biases for hidden to output layer with random values between -1 and 1
	for (i = 0; i < num_outputs; i++)
	{
		nn->bias_output[i] = ((double)rand() / (double)RAND_MAX) * 2 - 1;
		for (j = 0; j < num_hidden; j++)
		{
			nn->weights_hidden[j][i] = ((double)rand() / (double)RAND_MAX) * 2 - 1;
		}
	}

	// Initialize previous weight changes to zero for input to hidden layer weights and biases
	for (i = 0; i < num_hidden; i++)
	{
		nn->prevDeltaBiasHidden[i] = 0.0f;
		for (j = 0; j < num_inputs; j++)
		{
			nn->prevDeltaWeightsInput[j][i] = 0.0f;
		}
	}

	// Initialize previous weight changes to zero for hidden to output layer weights and biases
	for (i = 0; i < num_outputs; i++)
	{
		nn->prevDeltaBiasOutput[i] = 0.0f;
		for (j = 0; j < num_hidden; j++)
		{
			nn->prevDeltaWeightsHidden[j][i] = 0.0f;
		}
	}
}

// Function to calculate the sigmoid activation function given an input value x
double sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}

// Function to calculate the derivative of the sigmoid activation function given an input value x
double dsigmoid(double x)
{
	return sigmoid(x) * (1 - sigmoid(x));
}

// Function to calculate the output of the neural network given an input vector and a neural network structure
void feed_forward(double* inputs, struct NeuralNetwork* nn)
{
	int i, j;
	double sum;

	// Calculate input to hidden layer node activations (including bias node)
	for (i = 0; i < num_hidden; i++)
	{
		sum = 0.0f;
		for (j = 0; j < num_inputs; j++)
		{
			sum += inputs[j] * nn->weights_input[j][i];
		}
		sum += nn->bias_hidden[i]; // Add in bias
		nn->hidden[i] = sigmoid(sum); // Apply sigmoid activation function
	}

	// Calculate hidden to output layer node activations (including bias node)
	for (i = 0; i < num_outputs; i++)
	{
		sum = 0.0f;

		for (j = 0; j < num_hidden; j++)
		{
			sum += nn->hidden[j] * nn->weights_hidden[j][i];
		}
		sum += nn->bias_output[i]; // Add in bias
		// Apply sigmoid activation function
		nn->output[i] = sigmoid(sum);
	}
}

// Function to backpropagate errors given an expected output vector and a neural network structure
void backpropagate(double* expected, struct NeuralNetwork* nn)
{

	int i, j, k;

	double error, sum;

	// Calculate output layer error terms
	for (i = 0; i < num_outputs; i++)
	{
		error = expected[i] - nn->output[i];
		nn->errorOutput[i] = error * dsigmoid(nn->output[i]);
	}

	// Calculate hidden layer error terms
	for (i = 0; i < num_hidden; i++)
	{
		sum = 0.0f;
		for (k = 0; k < num_outputs; k++)
		{
			sum += nn->errorOutput[k] * nn->weights_hidden[i][k];
		}
		nn->errorHidden[i] = dsigmoid(nn->hidden[i]) * sum;
	}
}

// Function to update weights and biases given a neural network structure and an input vector
void updateWeightsBiases(double* inputs, struct NeuralNetwork* nn)
{
	int i, j, k;
	double deltaWeightInput, deltaWeightHidden, deltaBiasHidden, deltaBiasOutput;
	for (k = 0; k < num_outputs; k++)
	{
		deltaBiasOutput = learning_rate * nn->errorOutput[k] + momentum * nn->prevDeltaBiasOutput[k];
		nn->bias_output[k] += deltaBiasOutput;
		nn->prevDeltaBiasOutput[k] = deltaBiasOutput;
		for (j = 0; j < num_hidden; j++)
		{
			deltaWeightHidden =
				learning_rate * nn->errorOutput[k] * nn->hidden[j] + momentum * nn->prevDeltaWeightsHidden[j][k];
			nn->weights_hidden[j][k] += deltaWeightHidden;
			nn->prevDeltaWeightsHidden[j][k] = deltaWeightHidden;
		}
	}
	for (k = 0; k < num_hidden; k++)
	{
		deltaBiasHidden = learning_rate * nn->errorHidden[k] + momentum * nn->prevDeltaBiasHidden[k];
		nn->bias_hidden[k] += deltaBiasHidden;
		nn->prevDeltaBiasHidden[k] = deltaBiasHidden;
		for (j = 0; j < num_inputs; j++)
		{
			deltaWeightInput =
				learning_rate * nn->errorHidden[k] * inputs[j] + momentum * nn->prevDeltaWeightsInput[j][k];
			nn->weights_input[j][k] += deltaWeightInput;
			nn->prevDeltaWeightsInput[j][k] = deltaWeightInput;
		}
	}
}

// Function to train a neural network given an input vector, expected output vector, and a neural network structure
void train(double* pDouble, double* expected, struct NeuralNetwork* nn)
{

	feed_forward(pDouble, nn);
	backpropagate(expected, nn);
	updateWeightsBiases(pDouble, nn);

}

// Function to test a neural network given an input vector and a neural network structure
void test(double* pDouble, struct NeuralNetwork* nn)
{
	feed_forward(pDouble, nn);
}

// Function to print the weights and biases of a neural network structure

void print_neural_network(struct NeuralNetwork* nn)
{

	int i, j;
	printf("\nInput to Hidden Layer Weights and Biases:\n");

	for (i = 0; i < num_hidden; i++)
	{
		for (j = 0; j < num_inputs; j++)
		{
			printf("%lf ", nn->weights_input[j][i]);
		}
		printf("%lf\n", nn->bias_hidden[i]);

	}

	printf("\nHidden to Output Layer Weights and Biases:\n");

	for (i = 0; i < num_outputs; i++)
	{
		for (j = 0; j < num_hidden; j++)
		{
			printf("%lf ", nn->weights_hidden[j][i]);
		}
		printf("%lf\n", nn->bias_output[i]);
	}
}

// Main function to demonstrate neural network training and testing
int main()
{

	int i, j, k;

	// Seed the random number generator with the current time

	srand(time(NULL));

	// Declare a neural network structure variable and initialize it with random weights and biases between -1 and 1

	struct NeuralNetwork nn;

	init_neural_network(&nn);

	// Print the initial weights and biases of the neural network structure variable before training

	print_neural_network(&nn);

	// Train the neural network on a set of inputs and expected outputs for 1000 epochs (iterations)

	for (i = 0; i < 1000; i++)
	{
		train(inputs[0], expected[0], &nn);
		train(inputs[1], expected[1], &nn);
		train(inputs[2], expected[2], &nn);
		train(inputs[3], expected[3], &nn);
		train(inputs[4], expected[4], &nn);
		train(inputs[5], expected[5], &nn);
		train(inputs[6], expected[6], &nn);
		train(inputs[7], expected[7], &nn);
		train(inputs[8], expected[8], &nn);
		train(inputs[9], expected[9], &nn);
		train(inputs[10], expected[10], &nn);
		train(inputs[11], expected[11], &nn);
	}

	// Print the final weights and biases of the neural network structure variable after training

	print_neural_network(&nn);

	// Test the trained neural network on a set of inputs and print out the results

	printf("\nTesting Neural Network:\n");

	for (i = 0; i < 12; i++)
	{

		test(&testInputs1, &nn);

		printf("%lf %lf -> %lf\n", testInputs1, testInputs2, nn.output);

		testInputs1 += 0.1f;

		testInputs2 += 0.1f;

	}

	return 0;
}