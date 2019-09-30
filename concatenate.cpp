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
	if (argc < 3) {
		std::cout << "usage: ./concatenate.exe file1 file2" << std::endl;
        	exit(1);
	}
	char ch;
	FILE *f1 = fopen(argv[1], "r");
    	if (!f1) {
        	std::cout << "invalid feature file1" << std::endl;
        	exit(1);
    	}
	FILE *f2 = fopen(argv[2], "r");
    	if (!f2) {
        	std::cout << "invalid feature file2" << std::endl;
        	exit(1);
    	}
	FILE *ft = fopen("temp.txt", "w");
    	if (!ft) {
        	std::cout << "unable to open temp.txt" << std::endl;
        	exit(1);
    	}
	
	while(1){
		ch = getc(f1);
		while(ch != '\n' && ch != EOF) {
			//printf("%c  ",ch);
			putc(ch,ft);
			ch=getc(f1);
		}
		ch = getc(f2);
		while(ch != '\n' && ch != EOF) {
			//printf("%c  ",ch);
			putc(ch,ft);
			ch=getc(f2);
		}
		if (ch == '\n') putc(ch,ft); 
		else if (ch == EOF) break;
	}
	fclose(f1);
	fclose(f2);
	fclose(ft);
	ft = fopen("temp.txt", "r");
    	if (!ft) {
        	std::cout << "unable to open temp.txt" << std::endl;
        	exit(1);
    	}
	f1 = fopen(argv[1], "w");
    	if (!f1) {
        	std::cout << "invalid feature file1" << std::endl;
        	exit(1);
    	}
	while ((ch=getc(ft)) != EOF) {
		putc(ch,f1);
    	}
	fclose(f1);
	fclose(ft);	
	return 0;
}
