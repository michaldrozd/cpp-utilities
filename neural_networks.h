// Copyright (C) 2017 Michal Drozd
// All Rights Reserved

#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

// Neural network structure
struct NeuralNetwork
{

	double weights_input[2][3]; // Weights from input to hidden layer
	double weights_hidden[3][1]; // Weights from hidden to output layer

	double bias_hidden[3]; // Biases for hidden layer nodes
	double bias_output[1]; // Biases for output layer nodes

	double hidden[3]; // Hidden layer node activations
	double output[1]; // Output layer node activations

	double errorOutput[1]; // Output layer error terms
	double errorHidden[3]; // Hidden layer error terms

	double prevDeltaWeightsInput[2][3]; // Previous weight changes (input to hidden)
	double prevDeltaWeightsHidden[3][1]; // Previous weight changes (hidden to output)

	double prevDeltaBiasHidden[3]; // Previous bias changes (hidden)
	double prevDeltaBiasOutput[1]; // Previous bias changes (output)
};
#endif

#ifndef INPUTS_H
#define INPUTS_H

// Inputs to the neural network
double inputs[12][2] = {

	{ 0.0, 0.0 }, // Input 1

	{ 0.0, 1.0 }, // Input 2

	{ 1.0, 0.0 }, // Input 3

	{ 1.0, 1.0 }, // Input 4

	{ 1.5, 0.5 }, // Input 5

	{ 1.5, 1.5 }, // Input 6

	{ 2.5, 0.5 }, // Input 7

	{ 2.5, 1.5 }, // Input 8

	{ 3.5, 0.5 }, // Input 9

	{ 3.5, 1.5 }, // Input 10

	{ 4.5, 0.5 }, // Input 11

	{ 4.5, 1.5 }  // Input 12
};
#endif

#ifndef EXPECTED_OUTPUTS_H
#define EXPECTED_OUTPUTS_H

// Expected outputs from the neural network for each input vector above (corresponding to the XOR function)
double expected[12][1] = {
	{ 0.0 }, // Expected output 1
	{ 1.0 }, // Expected output 2
	{ 1.0 }, // Expected output 3
	{ 0.0 }, // Expected output 4
	{ 1.0 }, // Expected output 5
	{ 0.0 }, // Expected output 6
	{ 1.0 }, // Expected output 7
	{ 0.0 }, // Expected output 8
	{ 1.0 }, // Expected output 9
	{ 0.0 }, // Expected output 10
	{ 1.0 }, // Expected output 11
	{ 0.0 }  // Expected output 12
};
#endif


// test_inputs.h file
#ifndef TEST_INPUTS_H
#define TEST_INPUTS_H

// Test inputs to the neural network
double testInputs1 = 0.0f ;
double testInputs2 = 0.0f ;
#endif