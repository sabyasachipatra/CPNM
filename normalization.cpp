#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <map>
//#define SIZE 14
#define SIZE 8
#define LENGTH 335

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cout << "usage: ./conve.exe inputFile outputFile" << std::endl;
        	exit(1);
	}
	int i,j;
        float a;
	float A[LENGTH][SIZE];
	FILE *f1 = fopen(argv[1], "r");
    	if (!f1) {
        	std::cout << "unable to open input file" << std::endl;
        	exit(1);
    	}
	FILE *f2 = fopen(argv[2], "w");
    	if (!f2) {
        	std::cout << "unable to open output file" << std::endl;
        	exit(1);
    	}
	//Read
	for (i=0; i<LENGTH; i++) {
		for (int j=0; j<SIZE; j++) {
			fscanf(f1, "%f", &a);
			A[i][j]=a;
		}
    	}
	//Print
	for (i=0; i<LENGTH; i++) {
		for (int j=0; j<SIZE; j++) {
			printf("%10.7f  ", A[i][j]);
		}
		printf("\n");
    	}
	//Column Normalize
	for (int j=0; j<SIZE; j++) {
		float s=0;
		for (i=0; i<LENGTH; i++) {
			s=s+A[i][j];
		}
		for (i=0; i<LENGTH; i++) {
			A[i][j]=A[i][j]/s;
		}
    	}
	//Print
	for (i=0; i<LENGTH; i++) {
		for (int j=0; j<SIZE; j++) {
			printf("%10.7f  ", A[i][j]);
		}
		printf("\n");
    	}
	//Write
	for (i=0; i<LENGTH; i++) {
		for (int j=0; j<SIZE; j++) {
			fprintf(f2, "%10.7f  ", A[i][j]);
		}
		fprintf(f2, "\n");
    	}
	fclose(f1);
	fclose(f2);
	return 0;
}
