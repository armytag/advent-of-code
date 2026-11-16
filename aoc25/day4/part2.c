#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef
struct Row {
	struct Row *prev;
	struct Row *next;
	int values_count;
	int *values;
} Row;

int
line_length_of_file(char *filename) {
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

Row*
row_create(int row_length) {
	Row *row = (Row *)malloc(sizeof(Row));
	row->next = (Row *)NULL;
	row->prev = (Row *)NULL;
	row->values_count = row_length;
	row->values = (int *)calloc(row_length, sizeof(int));
	return row;
}

void
row_print(Row *row) {
	printf("[");
	for (int i = 0; i < row->values_count; i++) {
		printf("%d", row->values[i]);
		if (i < row->values_count - 1) {
			printf(", ");
		}
	}
	printf("]");
	if (!row->prev) { printf(" First"); }
	if (!row->next) { printf(" Last"); }
	printf("\n");
}

Row*
load_grid(char *filename, int line_length) {
	Row* grid = row_create(line_length);
	Row* curr = grid;
	int col = 0;
	int ch;
	FILE *fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return (Row *)NULL;
	}
	while ((ch = fgetc(fptr)) != EOF) {
		switch (ch) {
			case '.':
				if (col >= curr->values_count) {
					perror("Line length too long");
					return (Row *)NULL;
				}
				curr->values[col] = 0;
				col++;
				break;
			case '@':
				if (col >= curr->values_count) {
					perror("Line length too long");
					return (Row *)NULL;
				}
				curr->values[col] = 1;
				col++;
				break;
			case '\n':
				col = 0;
				Row *new_row = row_create(line_length);
				new_row->prev = curr;
				curr->next = new_row;
				curr = new_row;
				break;
			default:
				break;
		}
	}
	fclose(fptr);
	return grid;
}

int valid_paper(Row *row, int c) {
	if (c < 0 || c >= row->values_count) { return 0; }
	if (!row->values[c]) { return 0; }
	int count = 0;
	int first_col = c == 0;
	int last_col = c == row->values_count - 1;
	if (!first_col) { count += row->values[c-1]; }
	if (!last_col) { count += row->values[c+1]; }
	if (row->prev) {
		count += row->prev->values[c];
		if (!first_col) { count += row->prev->values[c-1]; }
		if (!last_col) { count += row->prev->values[c+1]; }
	}
	if (row->next) {
		count += row->next->values[c];
		if (!first_col) { count += row->next->values[c-1]; }
		if (!last_col) { count += row->next->values[c+1]; }
	}
	if (count < 4) {
		row->values[c] = 0;
		return 1;
	}
	return 0;
}

int
grid_find_valid_papers(Row *grid) {
	int count = 0;
	Row *curr = grid;
	while (curr) {
		for (int i = 0; i < curr->values_count; i++) {
			if (valid_paper(curr, i)) {
				count++;
			}
		}
		curr = curr->next;
	}
	return count;
}

int
main() {
	int64_t count = 0;
	int line_length = 0;
	char *filename = "input.txt";
	// Get line length
	if ((line_length = line_length_of_file(filename)) < 0) {
		perror("Error getting line length\n");
		return EXIT_FAILURE;
	}
	printf("Line length: %d\n", line_length);
	// Load grid
	Row *grid = load_grid(filename, line_length);
	if (!grid) {
		perror("Error loading grid");
		return EXIT_FAILURE;
	}
	// Find valid papers
	int new_count = 0;
	do {
		count = new_count;
		new_count += grid_find_valid_papers(grid);
	} while (new_count != count);
	count = new_count;
	printf("Count: %lu\n", count);
	// Cleanup
	while (grid) {
		Row *next = grid->next;
		free(grid);
		grid = next;
	}
	return EXIT_SUCCESS;
}
