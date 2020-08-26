/*
Ben Foxman netid- btf28
CS223 Problem 2
DUE 2/14/2020
Binpack Program
Program implements various algorithms to solve the Binpack problem.
*/

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int first_fit(int capacity, int weights[], int numWeights);
int best_fit(int capacity, int weights[], int numWeights);
int first_fit_decreasing(int capacity, int weights[], int numWeights);
int best_fit_decreasing(int capacity, int weights[], int numWeights);
int optimal(int capacity, int weights[], int bins[], int numWeights, int numBins,\
int bestSoFar, int lowerBound, int previousWeight, int previousWeightPlaced, int 
	weightsIndex);
void sort(int weights[], int numWeights);
int used_bins(int bins[], int numBins, int capacity);

int main(int argc, char *argv[]){
	//no elements
	if (argc == 1){
		printf("ERROR.\n");
		return 1;
	}
	//check that bin capacity is valid - i.e. that all chars are digits
	for (int i = 0; argv[1][i] != '\0'; i++){
		if (!isdigit(argv[1][i])){
			printf("ERROR.\n");
			return 1;
		}

	}
	int capacity = atoi(argv[1]);

	//mkae sure capacity is nonzero
	if (capacity == 0){
		printf("ERROR.\n");
			return 1;
	}

	//get weights - first weight stored in argv[2]
	int numWeights = 0;
	bool gettingWeights = true;
	bool noFlags = false; //track if last command lien arg is a weight 

	while (gettingWeights){ 
		//no flags were specified after the weights
		if (numWeights + 2 == argc){
			noFlags = true;
			break;
		}
		//printf("%d\n", numWeights);
		//check that weight value is valid - i.e. that all chars are digits
		for (int j = 0; argv[numWeights + 2][j] != '\0'; j++){
			if (!isdigit(argv[numWeights + 2][j])){
				gettingWeights = false;
				break;
			}
		}
		//and that the number is less than or equal to the capacity 
		if (atoi(argv[numWeights + 2]) > capacity){
			break;
		}
		
		if (gettingWeights){
			numWeights++;
		}
	}

	//put weights into array - make sure they are nonzero
	int weights[numWeights];
	for (int j = 0; j < numWeights; j++) {
		weights[j] = atoi(argv[j + 2]);
		if (weights[j] == 0){
		printf("ERROR.\n");
			return 1;
		}
	}

	//get flags - weights terminated at argv[numWeights + 1], start from 
	//argv[numWeights + 2]

	//check for errors
	for (int i = numWeights + 2; i < argc; i++){
		if (!(strcmp(argv[i], "-ff") == 0 || strcmp(argv[i], "-bf") == 0 || \
		strcmp(argv[i], "-ffd") == 0 || strcmp(argv[i], "-bfd") == 0 || \
		strcmp(argv[i], "-opt") == 0)) {
			printf("ERROR.\n");
			return 1;
		}
	}


	//SETUP, create x bins of desired capacity, x = numWeights(lower bound on
	//number of bins which guarantees all weights can be placed)
	int numBins = numWeights;
	int bins[numBins];

	for (int i = 0; i < numBins; i++){
		bins[i] = capacity;
	}

	//calculate lower bound for optimal
	int sumWeights = 0;
	for (int i = 0; i < numWeights; i++){
		sumWeights += weights[i];
	}
	int lowerBound = (sumWeights/capacity);
	if (lowerBound * capacity != sumWeights){
		lowerBound++;
	}
	//check for various flags, do the corresponding algorithm to each
	//flag in the order they appear on the command line
	for (int i = numWeights + 2; i < argc; i++){
		//there are no flags
		if (noFlags){
			return 0;
		}
		if (strcmp(argv[i], "-ff") == 0){
			printf("-ff  %d\n", first_fit(capacity, weights, numWeights));
		}
		else if (strcmp(argv[i], "-bf") == 0){
			printf("-bf  %d\n", best_fit(capacity, weights, numWeights));
		}
		else if (strcmp(argv[i], "-ffd") == 0){
			printf("-ffd %d\n", first_fit_decreasing(capacity, weights, numWeights));
		}
		else if (strcmp(argv[i], "-bfd") == 0){
			printf("-bfd %d\n", best_fit_decreasing(capacity, weights, numWeights));
		}
		else if (strcmp(argv[i], "-opt") == 0){
			//HEURISTIC A: Non-Increasing Order of Weights
			sort(weights, numWeights);
			printf("-opt %d\n", optimal(capacity, weights, bins, numWeights, numBins,\
			 numBins, lowerBound, -1, -1, 0));
			//printf("%ld\n", count);
		}
	}
	return 0;
}

//returns the minimum number of bins required to hold all weights, using the
//first fit heuristic.
int first_fit(int capacity, int weights[], int numWeights){
	int numBins = numWeights;
	int bins[numBins];

	//setup, create x bins of desired capacity, x = numWeights(lower bound on
	//number of bins which guarantees all weights can be placed)
	for (int i = 0; i < numBins; i++){
		bins[i] = capacity;
	}

	//loop through all weights
	for (int i = 0; i < numWeights; i++){

		//try each weight at each bin- break when first match found
		for (int j = 0; j < numBins; j++){
			if (bins[j] >= weights[i]) {
				bins[j] -= weights[i];
				break;
			}
		}
	}
	return used_bins(bins, numBins, capacity);
}

//returns the minimum number of bins required to hold all weights, using the
//best fit heuristic.
int best_fit(int capacity, int weights[], int numWeights){
	int numBins = numWeights;
	int bins[numBins + 1];
	//setup, create x bins of desired capacity, x = numWeights(lower bound on
	//number of bins which guarantees all weights can be placed)
	for (int i = 0; i < numBins + 1; i++){
		bins[i] = capacity;
	}
	bins[numBins]++;

	//loop through all weights
	for (int i = 0; i < numWeights; i++){

		//figure out the bin which is most full that can still hold the weight
		int bestBin = numBins;
		for (int j = 0; j < numBins; j++){

			//if the bin has room for the weight
			if (bins[j] >= weights[i]) {

				//if the bin has less capacity than the current best bin
				if (bins[j] < bins[bestBin]) {
					bestBin = j;
				}
			}
		}
		bins[bestBin] -= weights[i];
	}

	return used_bins(bins, numBins, capacity);
}

//returns the minimum number of bins required to hold all weights, using the
//first fit heuristic, and presorting the weights in non-increasing order. 
int first_fit_decreasing(int capacity, int weights[], int numWeights){
	sort(weights, numWeights);
	return first_fit(capacity, weights, numWeights);
}

//returns the minimum number of bins required to hold all weights, using the
//best fit heuristic, and presorting the weights in non-increasing order.
int best_fit_decreasing(int capacity, int weights[], int numWeights){
	sort(weights, numWeights);
	return best_fit(capacity, weights, numWeights);
}

//use backtracking to find minimum number of bins required
int optimal(int capacity, int weights[], int bins[], int numWeights, int numBins,\
	 int bestSoFar, int lowerBound, int previousWeight, int previousWeightPlaced, int 
	 weightsIndex){

	//printf("%d\n",bestSoFar);
	int binCount = used_bins(bins, numBins, capacity);
	
	//HEURISTIC B: Use of a lower bound
	if (bestSoFar == lowerBound){
		return bestSoFar;
	}

	//base case: a complete solution where there are no weights left not in bins
	if (numWeights == 0){
		return binCount;
	}

	//HEURISTIC C1: Only backtrack further if a partial assignment 
	//that is better than the best assignment so far
	if (binCount >= bestSoFar){
		return bestSoFar;
	}

	//HEURISTIC D: If the previous weight is the same, lower bins than
	//that were the previous weight was placed are no good
	int start = 0;
	if (weights[weightsIndex] == previousWeight) {
		start = previousWeightPlaced;
	}

	//now, loop through every bin: if it can hold the weight in question
	//which i arbitrarily choose to be the last weight in the sequence, put it 
	//in, recursively call this procedure until no weights remain

	for (int j = start; j < numBins; j++){
		if (bins[j] >= weights[weightsIndex]){
			if (binCount < bestSoFar){
				//HEURISTIC E: Avoid higher numbered bins when bins are the same
				bool noPrevMatches = true;
				for (int k = 0; k < j; k++){
					if (bins[k] == bins[j]){
						noPrevMatches = false;
						break;
					}
				}

				if(noPrevMatches){
					
					bins[j] -= weights[weightsIndex];

					//do the recursive call
					binCount = used_bins(bins, numBins, capacity);
					bestSoFar = optimal(capacity, weights, bins,\
					numWeights - 1, numBins, bestSoFar, lowerBound, weights[weightsIndex], j, \
					weightsIndex + 1);

					bins[j] += weights[weightsIndex];

					//HEURISTIC C2: Only backtrack further if a partial assignment 
					//that is better than the best assignment so far
					if (binCount >= bestSoFar){
						return bestSoFar;
					}
				}
			}
		}	
	}
	return bestSoFar;
}

//sorts an array of weights in non-increasing order
void sort(int weights[], int numWeights){
	for (int i = 0; i < numWeights - 1; i++){
		for (int j = i + 1; j < numWeights; j++){
			if (weights[i] < weights[j]){
				int temp = weights[i];
				weights[i] = weights[j];
				weights[j] = temp;
			}
		}
	}
}

//counts the number of used bins - i.e. bins not at full capacity
int used_bins(int bins[], int numBins, int capacity){
	int binsUsed = 0;
		for (int i = 0; i < numBins; i++){
			if (bins[i] != capacity){
				binsUsed++; 
			}
		}
	return binsUsed;
}







