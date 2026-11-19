#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Column {
	uint64_t value;
	struct Column *next;
} Column;

Column*
column_create() {
	Column *col = malloc(sizeof(Column));
	col->value = 0;
	col->next = (Column *)NULL;
	return col;
}

void
column_print(Column *col) {
	printf("%lu ", col->value);
}

Column*
file_parse_columns(char *filename, uint64_t *total) {
	Column *columns = column_create();
	int ch;
	FILE *fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return (Column *)NULL;
	}
	Column *cur = columns;
	Column *acc = columns;
	// First row
	while ((ch = getc(fptr)) != EOF) {
		if (ch == '\n') {
			cur = columns;
			break;
		}
		if (ch >= '0' && ch <= '9') {
			cur->value = (ch - '0');
		}
		Column *new = column_create();
		cur->next = new;
		cur = new;
	}
	cur = columns;
	// Loop remaining rows
	while ((ch = getc(fptr)) != EOF) {
		uint64_t t;
		switch (ch) {
			case '\n':
				cur = columns;
				break;
			case '0' ... '9':
				cur->value = cur->value * 10 + (ch - '0');
				cur = cur->next;
				break;
			case '*':
				t = 1;
				acc = cur;
				while (acc->next && acc->value) {
					t *= acc->value;
					acc = acc->next;
				}
				*total += t;
				cur = cur->next;
				break;
			case '+':
				t = 0;
				acc = cur;
				while (acc->next && acc->value) {
					t += acc->value;
					acc = acc->next;
				}
				*total += t;
				cur = cur->next;
				break;
			default:
				cur = cur->next;
				break;
		}
	}
	fclose(fptr);
	return columns;
}

int
main(int argc, char *argv[]) {
	char *filename;
	uint64_t total = 0;
	Column *columns;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	// Parse file into columns
	if (!(columns = file_parse_columns(filename, &total))) {
		perror("Error parsing columns from file");
		return EXIT_FAILURE;
	}
	// Find total
	Column *c = columns;
	while (c) {
		column_print(c);
		c = c->next;
	}
	printf("\n");
	printf("Total: %lu\n", total);
	// Cleanup
	while (columns) {
		Column *next = columns->next;
		free(columns);
		columns = next;
	}
	return EXIT_SUCCESS;
}
