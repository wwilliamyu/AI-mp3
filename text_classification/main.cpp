#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <vector>

using namespace std;

bool sorter (const pair<string, int> a, const pair<string, int>  b) {
	return a.second > b.second;
}

int main(int argc, char** argv) {

	if (argc != 1) {
		cout << "Number of inputs is wrong. Just do ./mp3";
	}

	// ifstream fisher_train ("fisher_train_2topic.txt");
	// ifstream fisher_test ("fisher_test_2topic.txt");
	ifstream fisher_train ("rt-train.txt");
	ifstream fisher_test ("rt-test.txt");


	int lol = 1;
	int divide;
	

	// Laplacian smoothing: add k to numerator, add k*V to denominator (V = 2)

//======================= FISHER ==========================================================

	// First number -1 is Life Partners, 1 is Minimum Wage
	// every line is one conversation
	
	// =========== FISHER - MULTINOMIAL TRAINING =============
	map<string,double> lp_dict, mw_dict;

	if (fisher_train.is_open()) {
		while (fisher_train.good()) {
			string conv_line;
			int topic;

			while (getline(fisher_train, conv_line)) {
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (conv_line[0] == '-') {
					topic = -1;
					i = 3;
				}
				else {
					topic = 1;
					i = 2;
				}
				// grab each word and the count
				string curr, word, num;
				if (topic == -1) {
					while (i < conv_line.length()) {
						if (conv_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (conv_line[i] == ' ') {
							num = curr;
							double temp = stod(num, nullptr);
							if (lp_dict.find(word) != lp_dict.end()){
								lp_dict.find(word)->second += temp;
							}
							else {
								lp_dict.insert(make_pair(word, temp));
							}
							curr = "";
						}
						else {
							curr.push_back(conv_line[i]);
						}
						i++;
					}
				}
				if (topic == 1) {
					while (i < conv_line.length()) {
						if (conv_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (conv_line[i] == ' ') {
							num = curr;
							double temp = stod(num, nullptr);
							if (mw_dict.find(word) != mw_dict.end()){
								mw_dict.find(word)->second += temp;
							}
							else {
								mw_dict.insert(make_pair(word, temp));
							}
							curr = "";
						}
						else {
							curr.push_back(conv_line[i]);
						}
						i++;
					}
				}
			}

			if (fisher_train.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_train.close();
	}

	map<string,double> bigboy; // get vocabulary size
	for (map<string,double>::iterator it = lp_dict.begin(); it != lp_dict.end(); it++) {
		bigboy.insert(make_pair(it->first,it->second));
	}
	for (map<string,double>::iterator it = mw_dict.begin(); it != mw_dict.end(); it++) {
		if (bigboy.find(it->first) == bigboy.end()) {
			bigboy.insert(make_pair(it->first,it->second));
		}
	}

	int total_lp_num = 0, total_mw_num = 0;
	for (map<string,double>::iterator it = lp_dict.begin(); it != lp_dict.end(); it++) {
		total_lp_num += it->second;
		//cout << it->first << ", " << it->second << endl;
	}
	for (map<string,double>::iterator it = mw_dict.begin(); it != mw_dict.end(); it++) {
		total_mw_num += it->second;
		//cout << it->first << ", " << it->second << endl;
	}
	int laplacian_k = 1;
	int laplacian_V = bigboy.size();

	// the actual smoothing
	for (map<string,double>::iterator it = lp_dict.begin(); it != lp_dict.end(); it++) {
		it->second += laplacian_k;
		it->second /= (total_lp_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}
	for (map<string,double>::iterator it = mw_dict.begin(); it != mw_dict.end(); it++) {
		it->second += laplacian_k;
		it->second /= (total_mw_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}

	// =============== FISHER - MULTINOMIAL TESTING ===================
	int accurate = 0;
	int inaccurate = 0;
	int total_classifications = 0;
	int cf00 = 0, cf01 = 0, cf10 = 0, cf11 = 0;
	if (fisher_test.is_open()) {
		while (fisher_test.good()) {
			string conv_line;
			int topic;

			while (getline(fisher_test, conv_line)) { // for each line
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (conv_line[0] == '-') {
					topic = -1;
					i = 3;
				}
				else {
					topic = 1;
					i = 2;
				}
				// grab each word and the count
				int classification = 0;
				string curr, word, num;

				double lp_probability = 0;
				double mw_probability = 0;
				while (i < conv_line.length()) { // iterate through the line
					if (conv_line[i] == ':') {
						word = curr;
						curr = "";
					}
					else if (conv_line[i] == ' ') {
						num = curr;
						double temp = stod(num, nullptr);
						if (lp_dict.find(word) != lp_dict.end()) { // if word exists in dictionary
							lp_probability += temp*log(lp_dict.find(word)->second); // dict probability * occurrences in this document
						}
						if (mw_dict.find(word) != mw_dict.end()) {
							mw_probability += temp*log(mw_dict.find(word)->second);
						}
						curr = "";

					}
					else {
						curr.push_back(conv_line[i]);
					}
					i++;
				}
				// priors
				lp_probability += (double)total_lp_num / (total_lp_num + total_mw_num);
				mw_probability += (double)total_mw_num / (total_lp_num + total_mw_num);
				if (lp_probability >= mw_probability) {
					classification = -1;
				}
				else {
					classification = 1;
				}
				if (classification == topic) {
					accurate++;
					if (classification == 1) {
						cf00++;
					}
					if (classification == -1) {
						cf11++;
					}
				}
				else {
					inaccurate++;
					if (classification == 1) {
						cf01++;
					}
					if (classification == -1) {
						cf10++;
					}
				}
				total_classifications++;
			}
			if (fisher_test.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_test.close();
	}
	if (lol == 0) {
		cout << "FISHER - MULTINOMIAL" << endl;
		divide = 49;
	}
	else {
		cout << "MOVIE - MULTINOMIAL" << endl;
		divide = 500;
	}

	cout << "   CONFUSION MATRIX with # of classifications: " <<total_classifications<<endl;
	cout << "   " << (double)cf00/divide*100<<"%" << ", " << (double)cf01/divide*100<<"%" << ", " << endl;
	cout << "   " << (double)cf10/divide*100<<"%" << ", " << (double)cf11/divide*100<<"%" << ", " <<endl<<endl;
	cout << "   ACCURATE " << ((double)accurate/(accurate+inaccurate)) * 100 << "%" << endl;
	cout << "   INACCURATE " << ((double)inaccurate/(accurate+inaccurate)) * 100 << "%" <<endl<<endl;;

	cout << "   Top 10 Odds Ratio - Fisher,Multinomial" << endl;
	vector<pair<string, double> > odds_ratio;

	for (map<string, double>::iterator i = bigboy.begin(); i != bigboy.end(); i++) {
		string word = i->first;
		double mw = log(mw_dict[word]);
		double lp = log(lp_dict[word]);
		odds_ratio.push_back(make_pair(word, mw - lp) );
	}
	sort(odds_ratio.begin(), odds_ratio.end(), sorter);
	cout << "   ";
	for (int i = 0; i < 10; i++) {
		cout <<odds_ratio[i].first << ", ";
	}
	cout <<endl;

	// =============== FISHER - BERNOULLI TRAINING ================

	// fisher_train.open("fisher_train_2topic.txt", ifstream::in);
	// fisher_test.open("fisher_test_2topic.txt", ifstream::in);
	fisher_train.open("rt-train.txt", ifstream::in);
	fisher_test.open("rt-test.txt", ifstream::in);

	map<string,double> bern_lp, bern_mw;
	int num_lp_doc = 0, num_mw_doc = 0;
	if (fisher_train.is_open()) {
		while (fisher_train.good()) {
			string conv_line;
			int topic;

			while (getline(fisher_train, conv_line)) {
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (conv_line[0] == '-') {
					topic = -1;
					i = 3;
				}
				else {
					topic = 1;
					i = 2;
				}
				// grab each word and the count
				string curr, word;
				if (topic == -1) {
					num_lp_doc++;
					while (i < conv_line.length()) {
						if (conv_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (conv_line[i] == ' ') {
							if (bern_lp.find(word) != bern_lp.end()){
								bern_lp.find(word)->second++;
							}
							else {
								bern_lp.insert(make_pair(word, 1));
							}
							curr = "";
						}
						else {
							curr.push_back(conv_line[i]);
						}
						i++;
					}
				}
				if (topic == 1) {
					num_mw_doc++;
					while (i < conv_line.length()) {
						if (conv_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (conv_line[i] == ' ') {
							if (bern_mw.find(word) != bern_mw.end()){
								bern_mw.find(word)->second++;
							}
							else {
								bern_mw.insert(make_pair(word, 1));
							}
							curr = "";
						}
						else {
							curr.push_back(conv_line[i]);
						}
						i++;
					}
				}
			}
			if (fisher_train.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_train.close();
	}
	for (map<string,double>::iterator it = bern_lp.begin(); it != bern_lp.end(); it++) {
		it->second += laplacian_k;
		it->second /= (num_lp_doc + laplacian_k * 2);
		//cout << it->first << ", " <<it->second<<endl;
	}
	for (map<string,double>::iterator it = bern_mw.begin(); it != bern_mw.end(); it++) {
		it->second += laplacian_k;
		it->second /= (num_mw_doc + laplacian_k * 2);
		//cout << it->first << ", " <<it->second<<endl;
	}

	// =============== FISHER - BERNOULLI TESTING ================
	accurate = 0;
	inaccurate = 0;
	total_classifications = 0;
	cf00 = 0, cf01 = 0, cf10 = 0, cf11 = 0;
	if (fisher_test.is_open()) {
		while (fisher_test.good()) {
			string conv_line;
			int topic;

			while (getline(fisher_test, conv_line)) {
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (conv_line[0] == '-') {
					topic = -1;
					i = 3;
				}
				else {
					topic = 1;
					i = 2;
				}
				// grab each word and the count
				int classification;
				string curr, word, num;

				double lp_probability=0;
				double mw_probability=0;
				while (i < conv_line.length()) {
					if (conv_line[i] == ':') {
						word = curr;
						curr = "";
					}
					else if (conv_line[i] == ' ') {
						if (bern_lp.find(word) != bern_lp.end()) { // if word exists in dictionary
							lp_probability += log(bern_lp.find(word)->second); // dict probability * occurrences in this document
						}
						if (bern_mw.find(word) != bern_mw.end()) {
							mw_probability += log(bern_mw.find(word)->second);
						}
						curr = "";
					}
					else {
						curr.push_back(conv_line[i]);
					}
					i++;
				}
				if (lp_probability > mw_probability) {
					classification = -1;
				}
				else {
					classification = 1;
				}

				if (classification == topic) {
					accurate++;
					if (classification == 1) {
						cf00++;
					}
					if (classification == -1) {
						cf11++;
					}
				}
				else {
					inaccurate++;
					if (classification == 1) {
						cf01++;
					}
					if (classification == -1) {
						cf10++;
					}
				}
				total_classifications++;
			}
			if (fisher_test.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_test.close();
	}
	if (lol == 0) {
		cout << "FISHER - MULTINOMIAL" << endl;
		divide = 49;
	}
	else {
		cout << "MOVIE - MULTINOMIAL" << endl;
		divide = 500;
	}

	cout << "   Confusion Matrix with # classifications: " <<total_classifications<<endl;
	cout << "   " << (double)cf00/divide*100<<"%" << ", " << (double)cf01/divide*100<<"%" << ", " << endl;
	cout << "   " << (double)cf10/divide*100<<"%" << ", " << (double)cf11/divide*100<<"%" << ", " <<endl<<endl;
	cout << "   ACCURATE " << ((double)accurate/(accurate+inaccurate)) * 100 << "%" << endl;
	cout << "   INACCURATE " << ((double)inaccurate/(accurate+inaccurate)) * 100 << "%" <<endl<<endl;
	cout << "   Top 10 Odds Ratio - Fisher,Bernoulli" << endl;
	vector<pair<string, double> > evens_ratio;

	for (map<string, double>::iterator i = bigboy.begin(); i != bigboy.end(); i++) {
		string word = i->first;
		double mw = log(bern_mw[word]);
		double lp = log(bern_lp[word]);
		evens_ratio.push_back(make_pair(word, mw - lp) );
	}
	sort(evens_ratio.begin(), evens_ratio.end(), sorter);
	cout << "   ";
	for (int i = 0; i < 10; i++) {
		cout << evens_ratio[i].first << ", ";
	}
	cout << endl;


// CONFUSION LIKELIHOOD STUFF


}
