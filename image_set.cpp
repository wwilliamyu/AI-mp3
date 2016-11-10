#include "image_set.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void ImageSet::populateLineUp(bool training)
{

	//if training == true, then set paths to training sets. else, set path to test sets
	
	string charLine, labelLine;
	const char* imagePath;					//set to const char* for fstream ease
	const char* labelPath;

	if (training == true)					//if train = true; use training set. else, use test set
	{
		imagePath = "digitdata/trainingimages";
		labelPath = "digitdata/traininglabels";
	}
	else {
		imagePath = "digitdata/testimages";
		labelPath = "digitdata/testlabels";
	}

	ifstream imageFile (imagePath);
	ifstream labelFile (labelPath);

	if (imageFile.is_open() && labelFile.is_open())
	{
		while (imageFile.good() && labelFile.good())
		{	
			vector<string> imageStringVec;	//the vector to be populated by 28 strings
			for (int x = 0; x < 28; x++)			//loops for the (relative) size of all of the chars
			{
			
				getline (imageFile, charLine);		//retrieves the current string in the image file
				imageStringVec.push_back(charLine);	//and puts it on the current image string
			}

			getline (labelFile, labelLine);			//retrieves the current line in the labels listing

			if (labelFile.eof())				//HACK CITY HACK HACK CITY
				break;
			
			
			int labelVal = atoi (labelLine.c_str());	//converts the line to a const char ** then to an int
			Image x = Image(imageStringVec, labelVal);	//Creates an image with the corresponding char array and label
			imageLineUp.push_back(x);			//pushes the image onto the main vector in the ImageSet class
		}
		imageFile.close();
		labelFile.close();
	}

}

ImageSet::ImageSet(bool training)
{
	populateLineUp(training);
}

void ImageSet::checkForNums(int n)   			//(debug) finds all images with a certain label and prints them
{	
	for (vector<Image>::iterator it = imageLineUp.begin(); it != imageLineUp.end(); it++)
	{
		if (it->getRealClass() == n)
			it->print();
	}
};

vector< vector<char> > ImageSet::makeClassAggregate(int n)		//returns a 2d vector of doubles representing probabilities of the training set
{
	vector<Image> classDefinedSet;

	vector< vector<char> > classDefinedProbabilitySet;		//the return vector
	
	for (vector<Image>::iterator it = imageLineUp.begin(); it != imageLineUp.end(); it++)   	/*constructs a vector of only
													images with a certain label*/
	{
		if (it->getRealClass() == n)
			classDefinedSet.push_back(*it);
	}

	for (int i = 0; i < 28; i++)						//hardcoded 28; we know the size
	{
		vector<char> classDefinedProbabilityLine;		//the string-by-string of the returned vector
		for (int j = 0; j < 28; j++)
		{
			double spaceChars = 0;					//the vars for char counts
			double edgeChars = 0;					
			double mainChars = 0;
			for (int setVar = 0; setVar < classDefinedSet.size(); setVar++) //iterate on the same pixel in the imageSet 
			{
				switch(classDefinedSet[setVar].getCharAt(i,j))	//increments the char count based on identified character
				{
					case(' '):
						spaceChars++;
						break;
					case('+'):
						edgeChars++;
						break;
					case('#'):
						mainChars++;
						break;
					default:
						spaceChars++;
				
				}
			}
			double total = spaceChars+edgeChars+mainChars;      				//calculating laplacian 
			double spaceProb = spaceChars/total;
			double edgeProb = edgeChars/total;
			double mainProb = mainChars/total;
			
			if (spaceProb >= edgeProb && spaceProb >= mainProb)
				classDefinedProbabilityLine.push_back(' ');
			else if (mainProb > spaceProb && mainProb > edgeProb)
				classDefinedProbabilityLine.push_back('#');
			else 
				classDefinedProbabilityLine.push_back('+');

		
		}
		classDefinedProbabilitySet.push_back(classDefinedProbabilityLine);
	}	

return classDefinedProbabilitySet;
};