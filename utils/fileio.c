#include "fileio.h"

#include <stdio.h>

int line_length_of_file(char *filename) {
	int length = 0;
	int ch;
	FILE *fptr = fopen(filename, "r");
	// Open file
	if (!fptr) {
		perror("Error opening file\n");
		return -1;
	}
	// Find line length
	while ((ch = getc(fptr)) != EOF) {
		if (ch == '\n') { break; }
		length++;
	}
	fclose(fptr);
	return length;
}

