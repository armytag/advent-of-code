#include <stdio.h>
#include <stdlib.h>

typedef struct Column {
	unsigned long value;
	struct Column *prev;
	struct Column *next;
} Column;

Column*
column_create()
{
	Column *col = (Column *)malloc(sizeof(Column));
	col->value = 0;
	col->prev = (Column *)NULL;
	col->next = (Column *)NULL;
	return col;
}

Column*
column_advance(Column *col, int lines) {
	if (lines == 0) {
		Column *n = column_create();
		n->prev = col;
		col->next = n;
	}
	if (!(col->next)) {
		perror("No column to advance to");
		return col;
	}
	return col->next;
}

void
column_print(Column *col) {
	if (col->value) {
		printf("%2lu ", col->value);
	} else {
		printf(" . ");
	}
}

void
column_draw(Column *col) {
	if (col->value) {
		printf("|");
	} else {
		printf(".");
	}
}

int
main(int argc, char *argv[])
{
	// Initialization
	Column *columns;
	int lines = 0;
	unsigned long next_in = 0;
	char *filename;
	FILE *fptr;
	int ch;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	// Main loop
	fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	columns = column_create();
	Column *col = columns;
	while ((ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '\n':
				col = columns;
				lines++;
				break;
			case 'S':
				col->value = 1;
				col = column_advance(col, lines);
				break;
			case '^':
				if (col->value) {
					unsigned long v = col->value;
					if (col->prev) {
						col->prev->value += col->value;
					}
					col->value = next_in;
					next_in = v;
				}
				col = column_advance(col, lines);
				break;
			case '.':
				if (next_in) {
					col->value += next_in;
				}
				next_in = 0;
				col = column_advance(col, lines);
				break;
			default:
				break;
		}
	}
	// Cleanup
	fclose(fptr);
	unsigned long total = 0;
	while (columns) {
		Column *n = columns->next;
		total += columns->value;
		free(columns);
		columns = n;
	}
	printf("Total: %lu\n", total);
	return EXIT_SUCCESS;
}
