#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iomanip>
#include "image.h"

using namespace std;

void odds(int r, int c, vector< vector<double> > global);

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
	vector<int> nums;
	vector<int> testing_nums;
	for (int p = 0; p < 10; p++) {
		nums.push_back(0);
		testing_nums.push_back(0);
	}

	double laplace_k = 0.1;
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

			vector<string> image_string;					
			for (int x = 0; x < 28; x++)					
			{
				getline (image_file, char_line);			
				image_string.push_back(char_line);			
			}
			getline (label_file, label_line);				
			if (label_file.eof()) {							//check for end of label file
				break;
			}
			int label_val = atoi (label_line.c_str());		
			
			Image img = Image(image_string, label_val);		
			training_images.push_back(img);					
		}
		image_file.close();
		label_file.close();
	}

	if (test_file.is_open() && label_test.is_open()) {
		while (test_file.good() && label_test.good()) {	

			vector<string> image_string;					
			for (int x = 0; x < 28; x++)					//loops for the (relative) size of all of the chars
			{
				getline (test_file, char_line);			
				image_string.push_back(char_line);			
			}
			getline (label_test, label_line);				
			if (label_test.eof()) {							//check for end of label file
				break;
			}
			int label_val = atoi (label_line.c_str());		
			
			Image img = Image(image_string, label_val);		
			testing_images.push_back(img);					
		}
		test_file.close();
		label_test.close();
	}

// ========================================= TRAINING & SETUP ===========================================

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
		testing_nums[testing_images[i].real_number]++;
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

		nums[training_images[i].real_number]++;
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
				global[z][m*28+n] /= (nums[z] + laplace_k * laplace_V);
			}
		}
	}


	// print out number of training examples per digit
	// for (int a = 0; a < 28; a++) {
	// 	for (int c = 0; c < 28; c++) {
	// 		if (global[0][a*28+c] > 0.5) {
	// 			cout << '#' << ' ';
	// 		} else {
	// 			cout << ' ' << ' ';
	// 		}
	// 		//cout << setprecision(3) << fixed << globalZero[a*28+c] << " ";
	// 	}
	// 	cout << "\n";
	// }

	for (int r = 0; r < 10; r++) {
		cout << "Number of " << r << " training examples: " << nums[r] << endl;
	}
	for (int r = 0; r < 10; r++) {
		cout << "Number of " << r << " testing examples: " << testing_nums[r] << endl;
	}

// ========================================= TESTING ===========================================

	int correct_count = 0;
	vector<int> test_digits; 
	for (int p = 0; p < 10; p++) {
		test_digits.push_back(0);
	}
	vector< vector<double> > confusion_matrix;


	//initialize confusion matrix
	for (int a = 0; a < 10; a++) { // for each digit
		vector<double> temp_vec;
		for (int b = 0; b < 10; b++) { //for each pixel in digit
			temp_vec.push_back(0);
		}
		confusion_matrix.push_back(temp_vec);
	}

	// TESTING, MAP classification
	for (int w = 0; w < testing_images.size(); w++) { // for each image	cout << "The test number is: " << testing_images[0].real_number << endl;
		//cout << "The test image number is: " << testing_images[w].real_number << endl;
		int temp = -1000000, best_digit = 0;
		for (int k = 0; k < 10; k++) { 	// for each digit
			double probability = 0;
			for (int i = 0; i < 28; i++) {			// for each pixel in testimage
				for (int j = 0; j < 28; j++) {
					if (testing_images[w].image_data[i][j] == '1') {
						probability += log(global[k][i*28+j]);
					}
					else {
						probability += log(1-(global[k][i*28+j]));
					}
				}
			}
			if (probability > temp) {
				temp = probability;
				best_digit = k;
			}
			//cout << "The probability that this number is " << k << ": " << probability << endl;
		}
		//cout << "It is most likely that the number is this: " << best_digit <<endl;
		if (best_digit == testing_images[w].real_number) {
			test_digits[testing_images[w].real_number]++;
			correct_count++;
		}
		else { // ERRORS
			confusion_matrix[testing_images[w].real_number][best_digit]++;
		}
	}
	cout << "SUMMARY: " << endl;
	cout << "Guessed correct: " << (double)correct_count/ 10.0 << "%" << endl;
	int wrong_count = 1000 - correct_count;
	cout << "Guessed wrong: " << (double)wrong_count / 10.0 << "%" << endl;

	for (int g = 0; g < 10; g++) {
		cout << "Accuracy for " << g << " is: " << (double)test_digits[g]/testing_nums[g]*100.0 << "%" << endl;
	}

	// classification matrix
	//10x10 matrix whose entry in row r and column c 
	// is the percentage of test images from class r that are classified as class c
	// e.g. if 9 mistaken as 7, would add to (9, 7)
	
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			confusion_matrix[i][j] /= testing_nums[i];
			cout << setprecision(2) << fixed << confusion_matrix[i][j]*100 << "%   ";
		}
		cout << endl;
	}


// ========================================= ODDS RATIO STUFF ===========================================

// HIGHEST CONFUSION RATES - (4,9): 15.89%, (5,3): 13.04%, (7,9): 13.21%, (8,3): 13.59%
	odds(4,9,global);
	cout << endl;
	odds(5,3,global);
	cout << endl;
	odds(7,9,global);
	cout << endl;
	odds(8,3,global);



// extra report stuff
	vector<double> high;
	vector<double> low;
	vector<int> high_idx;
	vector<int> low_idx;

	for (int p = 0; p < 10; p++) {
		high.push_back(-1000000);
		low.push_back(1000000);
		high_idx.push_back(0);
		low_idx.push_back(0);
	}

	for (int w = 0; w < testing_images.size(); w++) { // for each image
		//cout << "The test image number is: " << testing_images[w].real_number << endl;
		double probability = 0;
		for (int i = 0; i < 28; i++) {			// for each pixel in testimage
			for (int j = 0; j < 28; j++) {
				if (testing_images[w].image_data[i][j] == '1') {
					probability += log(global[testing_images[w].real_number][i*28+j]);
				}
				else {
					probability += log(1-(global[testing_images[w].real_number][i*28+j]));
				}
			}
		}
		// cout << "Real number: " << testing_images[w].real_number << ", "
		// 	<< "Probability: " << probability << ", "
		// 	<< "Current highest prob of digit: " << high[testing_images[w].real_number]<< ", "
		// 	<< "Current index of highest prob: " << high_idx[testing_images[w].real_number] << endl;
		// testing_images[high_idx[]].print();

		if (probability > high[testing_images[w].real_number]) {
			high[testing_images[w].real_number] = probability;
			high_idx[testing_images[w].real_number] = w;
		}
		if (probability < low[testing_images[w].real_number]) {
			low[testing_images[w].real_number] = probability;
			low_idx[testing_images[w].real_number] = w;
		}
	}

	// for (int f = 0; f < 10; f++) {
	// 	cout << "Index of highest probability for digit"<<f<< " is: " << high_idx[f]<<endl;
	// 	testing_images[high_idx[f]].print();
	// }
	// for (int f = 0; f < 10; f++) {
	// 	cout << "Index of lowest probability for digit"<<f<< " is: " << low_idx[f]<<endl;
	// 	testing_images[low_idx[f]].print();
	// }

}

void odds(int r, int c, vector< vector<double> > global) {
	for (int i = 0; i < 28; i++) {
		for (int j = 0; j < 28; j++) {
			if (global[r][i*28+j] > 0.5) {
				cout<<"#";
			}
			// if (global[5][i*28+j] > 0.2) {
			// 	cout<<"+";
			// }
			else if (global[r][i*28+j] > 0.3) {
				cout<<"+";
			}
			else {
				cout<<" ";
			}
		}
		cout << "     ";
		for (int j = 0; j < 28; j++) {
			if (global[c][i*28+j] > 0.5) {
				cout<<"#";
			}
			// if (global[5][i*28+j] > 0.2) {
			// 	cout<<"+";
			// }
			else if (global[c][i*28+j] > 0.3) {
				cout<<"+";
			}
			else {
				cout<<" ";
			}
		}
		cout << "     ";
		for (int j = 0; j < 28; j++) {
			if (log(global[r][i*28+j])-log(global[c][i*28+j]) >= 1) {
				cout<<"\033[1;31m#\033[0m";
			}
			// else if (global[r][i*28+j]/global[c][i*28+j] > 1 && global[r][i*28+j]/global[c][i*28+j] < 2) {
			// 	cout<<" ";
			// }
			if (log(global[r][i*28+j])-log(global[c][i*28+j]) >= 0 && log(global[r][i*28+j])-log(global[c][i*28+j]) < 1){
				cout<<"\033[1;36m#\033[0m";
			}
			if (log(global[r][i*28+j])-log(global[c][i*28+j]) < 0 && log(global[r][i*28+j])-log(global[c][i*28+j]) >= -1){
				cout<<"\033[1;36m#\033[0m";
			}
			if (log(global[r][i*28+j])-log(global[c][i*28+j]) < -1 && log(global[r][i*28+j])-log(global[c][i*28+j]) >= -2){
				cout<<"\033[1;36m#\033[0m";
			}
			if (log(global[r][i*28+j])-log(global[c][i*28+j]) < -2){
				cout<<"\033[1;32m#\033[0m";
			}
		}
		cout << endl;
	}
}
