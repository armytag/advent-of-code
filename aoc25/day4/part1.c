#include "fileio.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_lines(int *lines[3], int line_length) {
	for (int l = 0; l < 3; l++) {
		for (int i = 0; i < line_length; i++) {
			printf("%d ", lines[l][i]);
		}
		printf("\n");
	}
}

void advance_lines(int *lines[3], int line_length) {
	free(lines[0]);
	lines[0] = lines[1];
	lines[1] = lines[2];
	lines[2] = (int *)calloc(line_length, sizeof(int));
}

int valid_paper(int *lines[3], int line_length, int col) {
	if (col < 1 || col >= (line_length - 1)) {
		return 0;
	}
	if ((lines[0][col-1] + lines[0][col] + lines[0][col+1] +
			 lines[2][col-1] + lines[2][col] + lines[2][col+1] +
			 lines[1][col-1] + lines[1][col+1]) < 4) {
		return lines[1][col];
	}
	return 0;
}

int64_t loop_lines(char *filename, int line_length) {
	int64_t count = 0, line_count = 0;
	int ch = 0, col = 0;
	int **lines;
	FILE *fptr;
	// Create memory for lines
	lines = (int **)calloc(3, sizeof(int *));
	for (int i = 0; i < 3; i++) {
		lines[i] = (int *)calloc(line_length + 2, sizeof(int));
	}
	// Read lines
	fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return -1;
	}
	while ((ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '\n':
				for (int c = 0; c < line_length; c++) {
					if (lines[1][c+1] > 0 && valid_paper(lines, line_length+2, c+1)) {
						count++;
					}
				}
				advance_lines(lines, line_length + 2);
				col = 0;
				line_count++;
				break;
			case '@':
				lines[2][col+1] = 1;
			default:
				col++;
				if (col > line_length) {
					perror("Line too long");
					return -1;
				}
				break;
		}
	}
	printf("Line count: %ld\n", line_count);
	// Count last line
	// print_lines(lines, line_length + 2);
	for (col = 0; col < line_length; col++) {
		if (valid_paper(lines, line_length + 2, col + 1)) {
			count++;
		}
	}
	// Cleanup
	for (int i = 0; i < 3; i++) {
		free(lines[i]);
	}
	free(lines);
	return count;
}

int main() {
	int64_t count = 0;
	int line_length = 0;
	char *filename = "input.txt";
	// Get line length
	if ((line_length = line_length_of_file(filename)) < 0) {
		perror("Error getting line length\n");
		return EXIT_FAILURE;
	}
	printf("Line length: %d\n", line_length);
	// Loop lines
	if((count = loop_lines(filename, line_length)) < 0) {
		perror("");
		return EXIT_FAILURE;
	}
	printf("Count: %lu\n", count);
	return EXIT_SUCCESS;
}
