#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Column {
	uint64_t *values;
	struct Column *next;
	int num_values;
	char operation;
} Column;

Column*
column_create(int num_values) {
	Column *col = malloc(sizeof(Column));
	col->num_values = num_values;
	col->values = (uint64_t *)calloc(num_values, sizeof(uint64_t));
	col->operation = '?';
	col->next = (Column *)NULL;
	return col;
}

void
column_print(Column *col) {
	printf("%c -> [", col->operation);
	for (int i = 0; i < col->num_values; i++) {
		printf("%lu", col->values[i]);
		if (i < col->num_values - 1) {
			printf(", ");
		}
	}
	printf("]\n");
}

uint64_t
column_reduce(Column *col) {
	uint64_t total;
	if (col->operation == '*') { total = 1; }
	else if (col->operation == '+') { total = 0; }
	else { return 0; }
	for (int i = 0; i < col->num_values; i++) {
		if (col->operation == '*') { total *= col->values[i]; }
		if (col->operation == '+') { total += col->values[i]; }
	}
	return total;
}

int
file_count_lines(char *filename) {
	int lines = 0;
	int ch;
	FILE *fptr = fopen(filename, "r");
	while ((ch = getc(fptr)) != EOF) {
		if (ch == '\n') {
			lines++;
		}
	}
	if (!fptr) {
		perror("Error opening file");
		return -1;
	}
	fclose(fptr);
	return lines;
}

Column*
file_parse_columns(char *filename, int num_lines) {
	Column *cols = column_create(num_lines);
	bool spaces = true;
	int l = 0;
	int ch;
	FILE *fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return (Column *)NULL;
	}
	Column *cur = cols;
	while ((ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '0' ... '9':
				spaces = false;
				cur->values[l] = cur->values[l] * 10 + (ch - '0');
				break;
			case '*':
			case '+':
				spaces = false;
				cur->operation = ch;
				break;
			case ' ':
				if (!spaces) {
					if (l == 0) {
						cur->next = column_create(num_lines);
					}
					if (cur->next) {
						cur = cur->next;
					}
				}
				spaces = true;
				break;
			case '\n':
				spaces = true;
				cur = cols;
				l++;
				break;
			default:
				break;
		}
	}
	fclose(fptr);
	return cols;
}

int
main(int argc, char *argv[]) {
	char *filename;
	int num_lines;
	uint64_t total = 0;
	Column *columns;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	// Get file length
	if ((num_lines = file_count_lines(filename) - 1) <= 0) {
		perror("No lines found in file");
		return EXIT_FAILURE;
	}
	printf("Line count: %d\n", num_lines);
	// Parse file into columns
	if (!(columns = file_parse_columns(filename, num_lines))) {
		perror("Error parsing columns from file");
		return EXIT_FAILURE;
	}
	// Find total
	Column *c = columns;
	while (c) {
		column_print(c);
		total += column_reduce(c);
		c = c->next;
	}
	// Cleanup
	printf("Total: %lu\n", total);
	while (columns) {
		Column *next = columns->next;
		free(columns);
		columns = next;
	}
	return EXIT_SUCCESS;
}
