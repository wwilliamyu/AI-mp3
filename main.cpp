#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iomanip>
#include "image.h"

using namespace std;

int main(int argc, char** argv) {

	// Arg 1 = executable
	// Arg 2 = training
	if (argc != 1) {
		cout << "Number of inputs is wrong. Just do ./mp3";
	}

	// STORE ALL THEM DIGITS

	vector< vector<double> > global;
	// vector<double> globalZero;
	// vector<double> globalOne;
	// vector<double> globalTwo;
	// vector<double> globalThree;
	// vector<double> globalFour;
	// vector<double> globalFive;
	// vector<double> globalSix;
	// vector<double> globalSeven;
	// vector<double> globalEight;
	// vector<double> globalNine;
	for (int a = 0; a < 10; a++) { // for each digit
		vector<double> global_digit;
		for (int b = 0; b < 784; b++) { //for each pixel in digit
			global_digit.push_back(0);
			// globalZero.push_back(0);
			// globalOne.push_back(0);
			// globalTwo.push_back(0);
			// globalThree.push_back(0);
			// globalFour.push_back(0);
			// globalFive.push_back(0);
			// globalSix.push_back(0);
			// globalSeven.push_back(0);
			// globalEight.push_back(0);
			// globalNine.push_back(0);
		}
		global.push_back(global_digit);
	}

	int numZero = 0, numOne = 0, numTwo = 0, numThree = 0, numFour = 0,
		numFive = 0, numSix = 0, numSeven = 0, numEight = 0, numNine = 0;

	double laplace_k = 10;
	int laplace_V = 2;


	vector< Image > training_images; // vector of images (size should be 5000)
	vector< Image > testing_images;

	const char* training_image = "trainingimages";
	const char* training_label = "traininglabels";
	const char* test_image = "testimages";
	const char* test_label = "testlabels";

	// Storage processing
	ifstream image_file (training_image);
	ifstream label_file (training_label);
	ifstream test_file (test_image);
	ifstream label_test (test_label);

	string char_line, label_line;
	if (image_file.is_open() && label_file.is_open()) {
		while (image_file.good() && label_file.good()) {	

			vector<string> image_string;					//the vector to be populated by 28 strings
			for (int x = 0; x < 28; x++)					//loops for the (relative) size of all of the chars
			{
				getline (image_file, char_line);			//retrieves the current string in the image file
				image_string.push_back(char_line);			//and puts it on the current image string
			}
			getline (label_file, label_line);				//retrieves the current line in the labels listing
			if (label_file.eof()) {							//check for end of label file
				break;
			}
			int label_val = atoi (label_line.c_str());		//converts the line to a const char ** then to an int
			
			Image img = Image(image_string, label_val);		//Creates an image with the corresponding char array and label
			training_images.push_back(img);							//pushes the image onto the main vector in the ImageSet class
		}
		image_file.close();
		label_file.close();
	}

	if (test_file.is_open() && label_test.is_open()) {
		while (test_file.good() && label_test.good()) {	

			vector<string> image_string;					//the vector to be populated by 28 strings
			for (int x = 0; x < 28; x++)					//loops for the (relative) size of all of the chars
			{
				getline (test_file, char_line);			//retrieves the current string in the image file
				image_string.push_back(char_line);			//and puts it on the current image string
			}
			getline (label_test, label_line);				//retrieves the current line in the labels listing
			if (label_test.eof()) {							//check for end of label file
				break;
			}
			int label_val = atoi (label_line.c_str());		//converts the line to a const char ** then to an int
			
			Image img = Image(image_string, label_val);		//Creates an image with the corresponding char array and label
			testing_images.push_back(img);							//pushes the image onto the main vector in the ImageSet class
		}
		test_file.close();
		label_test.close();
	}


	//convert testing image to 1 and 0
	for (int i = 0; i < testing_images.size(); i++) { // for each image
		for (int x = 0; x < 28; x++) { 				    // for each pixel
			for (int y = 0; y < 28; y++) {

				// 1 and 0 be features
				switch(testing_images[i].image_data[x][y]) {
					case '+':
						testing_images[i].image_data[x][y] = '1';
					
						break;
					case '#':
						testing_images[i].image_data[x][y] = '1';
						break;
					case ' ':
						testing_images[i].image_data[x][y] = '0';
						break;
				}
				// once features done, then check what number it is
				// whatever number, increment the number in global
			}
		}
	}

	// TRAINING STAGE

	// P(Fij = f | class) = (# of times pixel (i,j) has value f in training examples from this class) 
	// 					/ (Total # of training examples from this class)


	// iterate through each training image
	// check what number that is
		// add all counts to global
		// iterate through each pixel
		// if pixel is 1, c1 ++
		// if pixel is 0, c0 ++
	// We want P(Fij | class)
	for (int i = 0; i < training_images.size(); i++) { // for each image
		for (int x = 0; x < 28; x++) { 				    // for each pixel
			for (int y = 0; y < 28; y++) {

				// 1 and 0 be features
				switch(training_images[i].image_data[x][y]) {
					case '+':
						training_images[i].image_data[x][y] = '1';
					
						break;
					case '#':
						training_images[i].image_data[x][y] = '1';
						break;
					case ' ':
						training_images[i].image_data[x][y] = '0';
						break;
				}
				// once features done, then check what number it is
				// whatever number, increment the number in global
			}
		}

		// to get number of each training example
		switch (training_images[i].real_number) {
			case 0:
				numZero++;
				break;
			case 1:
				numOne++;
				break;
			case 2:
				numTwo++;
				break;
			case 3:
				numThree++;
				break;
			case 4:
				numFour++;
				break;
			case 5:
				numFive++;
				break;
			case 6:
				numSix++;
				break;
			case 7:
				numSeven++;
				break;
			case 8:
				numEight++;
				break;
			case 9:
				numNine++;
				break;
		}
	}

	// starting training
	for (int i = 0; i < training_images.size(); i++) { // for each image
		for (int x = 0; x < 28; x++) { 				    // for each pixel
			for (int y = 0; y < 28; y++) {
				if (training_images[i].image_data[x][y] == '1') {
					global[training_images[i].real_number][x*28+y]++;
				}
			}
		}
	}
	// do division & laplacian smoothing
	for (int z = 0; z < 10; z++) { //for each digit
		for (int m = 0; m < 28; m++) {
			for (int n = 0; n < 28; n++) {
				// for digit 0
				global[z][m*28+n] += laplace_k;
				global[z][m*28+n] /= (numZero + laplace_k * laplace_V);
			}
		}
	}


	// print out number of training examples per digit
	for (int a = 0; a < 28; a++) {
		for (int c = 0; c < 28; c++) {
			if (global[0][a*28+c] > 0.5) {
				cout << '#' << ' ';
			} else {
				cout << ' ' << ' ';
			}
			//cout << setprecision(3) << fixed << globalZero[a*28+c] << " ";
		}
		cout << "\n";
	}

	cout << "Number of 0 training examples: " << numZero << endl;
	cout << "Number of 1 training examples: " << numOne << endl;
	cout << "Number of 2 training examples: " << numTwo << endl;
	cout << "Number of 3 training examples: " << numThree << endl;
	cout << "Number of 4 training examples: " << numFour << endl;
	cout << "Number of 5 training examples: " << numFive << endl;
	cout << "Number of 6 training examples: " << numSix << endl;
	cout << "Number of 7 training examples: " << numSeven << endl;
	cout << "Number of 8 training examples: " << numEight << endl;
	cout << "Number of 9 training examples: " << numNine << endl;

	// DONE WITH P(Fij = f | class) = (# of times pixel (i,j) has value f in training examples from this class) / (Total # of training examples from this class)


	// TESTING, MAP classification

	cout << "JOHN CENA" << endl;
	cout << "The test number is: " << testing_images[0].real_number << endl;
	for (int k = 0; k < 10; k++) { 	// for each digit
		double probability = 0, temp = 0;
		for (int i = 0; i < 28; i++) {			// for each pixel in testimage
			for (int j = 0; j < 28; j++) {
				if (testing_images[0].image_data[i][j] == '1') {
					probability += log(global[k][i*28+j]);
				}
				else {
					probability += log(1-global[k][i*28+j]);
				}
			}
		}
		cout << "The probability that this number is " << k << ": " << probability << endl;
	}



}
