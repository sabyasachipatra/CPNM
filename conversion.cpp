#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <map>

int main(int argc, char **argv)
{
	if (argc < 4) {
		std::cout << "usage: ./conversion.exe inputFile outputFile dictionary" << std::endl;
        	exit(1);
	}
	FILE *f1 = fopen(argv[1], "r");
    	if (!f1) {
        	std::cout << "unable to open input file" << std::endl;
        	exit(1);
    	}
	FILE *f2 = fopen(argv[2], "w");
    	if (!f2) {
        	std::cout << "invalid output file" << std::endl;
        	exit(1);
    	}
        FILE *fd = fopen(argv[3], "w");
    	if (!fd) {
        	std::cout << "invalid dictionary" << std::endl;
        	exit(1);
    	}

	int size;
	char a[20], b[20];
    	vector<string> va, vb;
	vector<string> dict;
    	size = 0;
    	while (fscanf(f1, "%s %s", a, b) == 2) {
        	va.push_back(a);
        	vb.push_back(b);
        	size++;
    	}
	//cout << "size=" << size << endl;
	int dict_size=0;
	int i,j;
	for (i=0; i<size; i++) {
		for (j=0; j<dict_size; j++) {
			if (va[i].compare(dict[j])==0) break;
		}
		if (j == dict_size) {
			dict.push_back(va[i]);
			dict_size++;
		}
		for (j=0; j<dict_size; j++) {
			if (vb[i].compare(dict[j])==0) break;
		}
		if (j == dict_size) {
			dict.push_back(vb[i]);
			dict_size++;
		}
	}
	//cout << "dict_size=" << dict_size << endl;
	for (i=0; i<dict_size; i++) {
        	fprintf(fd,"%s\t%d\n",dict[i].c_str(),i+1);
        }
	for (i=0; i<size; i++) {
		for (j=0; j<dict_size; j++) {
			if (va[i].compare(dict[j])==0) {
				fprintf(f2,"%d\t",j+1);
				break;
			}
		}
		for (j=0; j<dict_size; j++) {
			if (vb[i].compare(dict[j])==0) {
				fprintf(f2,"%d\t1\n",j+1);
				break;
			}
		}
	}
	fclose(f1);
        fclose(f2);
	fclose(fd);
	return 0;
}
