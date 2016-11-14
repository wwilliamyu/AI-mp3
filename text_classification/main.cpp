#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>

using namespace std;


int main(int argc, char** argv) {

	if (argc != 1) {
		cout << "Number of inputs is wrong. Just do ./mp3";
	}

	ifstream fisher_train ("fisher_train_2topic.txt");
	ifstream fisher_test ("fisher_test_2topic.txt");
	ifstream movie_train ("rt-train.txt");
	ifstream movie_test ("rt-test.txt");

	// Laplacian smoothing: add k to numerator, add k*V to denominator (V = 2)
	double laplacian_k = 5;
	int laplacian_V = 2;

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
	int total_lp_num = 0, total_mw_num = 0;
	for (map<string,double>::iterator it = lp_dict.begin(); it != lp_dict.end(); it++) {
		total_lp_num += it->second;

		it->second += laplacian_k;
		it->second /= (total_lp_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}
	for (map<string,double>::iterator it = mw_dict.begin(); it != mw_dict.end(); it++) {
		total_mw_num += it->second;

		it->second += laplacian_k;
		it->second /= (total_mw_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}

	// =============== FISHER - MULTINOMIAL TESTING ===================
	int accurate = 0;
	int inaccurate = 0;
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
						num = curr;
						double temp = stod(num, nullptr);

						if (lp_dict.find(word) != lp_dict.end()) { // if word exists in dictionary
							lp_probability += log(temp * lp_dict.find(word)->second); // dict probability * occurrences in this document
						}
						if (mw_dict.find(word) != mw_dict.end()) {
							mw_probability += log(temp * mw_dict.find(word)->second);
						}
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
				}
				else {
					inaccurate++;
				}
			}
			if (fisher_test.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_test.close();
	}
	cout << "FISHER - MULTINOMIAL" << endl;
	cout << "   ACCURATE " << accurate <<endl;
	cout << "   INACCURATE " << inaccurate <<endl;

	// =============== FISHER - BERNOULLI TRAINING ================

	fisher_train.open("fisher_train_2topic.txt", ifstream::in);
	fisher_test.open("fisher_test_2topic.txt", ifstream::in);

	map<string,double> bern_lp, bern_mw;
	int num_lp_doc = 0, num_mw_doc = 0;
	if (fisher_train.is_open()) {
		while (fisher_train.good()) {
			cout << "JOHN CENA" <<endl;
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
	cout << "# LP DOCS: " << num_lp_doc << endl;
	cout << "# MW DOCS: " << num_mw_doc << endl;
	for (map<string,double>::iterator it = bern_lp.begin(); it != bern_lp.end(); it++) {
		it->second += laplacian_k;
		it->second /= (num_lp_doc + laplacian_k * laplacian_V);
		cout << it->first << ", " <<it->second<<endl;
	}
	for (map<string,double>::iterator it = bern_mw.begin(); it != bern_mw.end(); it++) {
		it->second += laplacian_k;
		it->second /= (num_mw_doc + laplacian_k * laplacian_V);
		cout << it->first << ", " <<it->second<<endl;
	}

	// =============== FISHER - BERNOULLI TESTING ================
	accurate = 0;
	inaccurate = 0;
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
				}
				else {
					inaccurate++;
				}
			}
			if (fisher_test.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_test.close();
	}
	cout << "FISHER - BERNOULLI" << endl;
	cout << "   ACCURATE " << accurate <<endl;
	cout << "   INACCURATE " << inaccurate <<endl;




//======================= MOVIE REVIEW ======================================================
	// First number -1 is Bad Review, 1 is Good Review

	// =============== MOVIE - MULTINOMIAL TRAINING ================
	map<string,double> bad_dict, good_dict;

	if (movie_train.is_open()) {
		while (movie_train.good()) {
			string rev_line;
			int review;

			while (getline(movie_train, rev_line)) {
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (rev_line[0] == '-') {
					review = -1;
					i = 3;
				}
				else {
					review = 1;
					i = 2;
				}
				// grab each word and the count

				string curr, word, num;
				if (review == -1) {
					while (i < rev_line.length()) {
						if (rev_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (rev_line[i] == ' ') {
							num = curr;
							double temp = stod(num, nullptr);
							if (bad_dict.find(word) != bad_dict.end()){
								bad_dict.find(word)->second += temp;
							}
							else {
								bad_dict.insert(make_pair(word, temp));
							}
							curr = "";
						}
						else {
							curr.push_back(rev_line[i]);
						}
						i++;
					}
				}
				if (review == 1) {
					while (i < rev_line.length()) {
						if (rev_line[i] == ':') {
							word = curr;
							curr = "";
						}
						else if (rev_line[i] == ' ') {
							num = curr;
							double temp = stod(num, nullptr);
							if (good_dict.find(word) != good_dict.end()){
								good_dict.find(word)->second += temp;
							}
							else {
								good_dict.insert(make_pair(word, temp));
							}
							curr = "";
						}
						else {
							curr.push_back(rev_line[i]);
						}
						i++;
					}
				}
			}

			if (movie_train.eof()) {	//check for end of label file
				break;
			}
		}
		movie_train.close();
	}

	int total_bad_num = 0, total_good_num = 0;
	for (map<string,double>::iterator it = bad_dict.begin(); it != bad_dict.end(); it++) {
		total_bad_num += it->second;
		
		it->second += laplacian_k;
		it->second /= (total_bad_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}
	for (map<string,double>::iterator it = good_dict.begin(); it != good_dict.end(); it++) {
		total_good_num += it->second;
		
		it->second += laplacian_k;
		it->second /= (total_good_num + laplacian_k * laplacian_V);
		//cout << it->first << ", " << it->second << endl;
	}

	// =============== MOVIE - MULTINOMIAL TESTING ================
	accurate = 0;
	inaccurate = 0;
	if (movie_test.is_open()) {
		while (movie_test.good()) {
			string rev_line;
			int review;

			while (getline(movie_test, rev_line)) {
				//check first character, if '-' then -1, if '1' then 1
				int i;
				if (rev_line[0] == '-') {
					review = -1;
					i = 3;
				}
				else {
					review = 1;
					i = 2;
				}
				// grab each word and the count
				int classification;
				string curr, word, num;

				double bad_probability;
				double good_probability;
				while (i < rev_line.length()) {
					if (rev_line[i] == ':') {
						word = curr;
						curr = "";
					}
					else if (rev_line[i] == ' ') {
						num = curr;
						double temp = stod(num, nullptr);

						if (bad_dict.find(word) != bad_dict.end()) { // if word exists in dictionary
							bad_probability += log(temp * bad_dict.find(word)->second); // dict probability * occurrences in this document
						}
						else {
							bad_probability += 0;
						}
						if (good_dict.find(word) != good_dict.end()) {
							good_probability += log(temp * good_dict.find(word)->second);
						}
						else {
							good_probability += 0;
						}
					}
					else {
						curr.push_back(rev_line[i]);
					}
					i++;
				}
				if (bad_probability >= good_probability) {
					classification = -1;
				}
				else {
					classification = 1;
				}

				if (classification == review) {
					accurate++;
				}
				else {
					inaccurate++;
				}
			}
			if (fisher_test.eof()) {	//check for end of label file
				break;
			}
		}
		fisher_test.close();
	}
	cout << "MOVIE - MULTINOMIAL" << endl;
	cout << "   ACCURATE " << accurate <<endl;
	cout << "   INACCURATE " << inaccurate <<endl;

	// =============== MOVIE - BERNOULLI TRAINING ================
	movie_train.open("rt-train.txt", ifstream::in);
	movie_test.open("rt-test.txt", ifstream::in);




// ======================== TOTALS =================================

	cout << "Total bad nums: " << total_bad_num <<endl;
	cout << "Total good nums: " << total_good_num <<endl;
	cout << "Total life partners nums: " << total_lp_num <<endl;
	cout << "Total minimum wage nums: " << total_mw_num <<endl;
}
