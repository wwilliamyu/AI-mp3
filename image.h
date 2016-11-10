#ifndef _IMAGE_H
#define _IMAGE_H

#include <iostream>
#include <vector>

using namespace std;

class Image {
	
	public:
		
		int predicted_number;
		int real_number;
		vector<string> string_image;
		vector< vector<char> > image_data;

		Image() {};

		Image(vector<string> inputStringSet) {
			string_image = inputStringSet;

		};

		Image(vector<string> input_string, int real_label) {
			string_image = input_string;
			real_number = real_label;
			populateImageData();
		};

		char getchar(int x, int y) {
			return image_data[x][y];
		}

		int get_real_num() {
			return real_number;
		};

		int get_predicted_num()
		{
			return predicted_number;
		};

		void print() {
			for (int x = 0; x < 28; x++) {
				for (int y = 0; y < 28; y++) {
					cout << image_data[x][y];
				}
				cout << "\n";
			}
			cout << "The image's real number is: " << real_number << endl;
		};

		void populateImageData() {
			for (int i = 0; i < 28; i++) {
				string img = string_image[i];
				vector<char> data(img.begin(), img.end());
				image_data.push_back(data);
			}
		};

};

#endif