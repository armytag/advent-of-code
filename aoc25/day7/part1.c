#include <stdio.h>
#include <stdlib.h>

typedef struct Column {
	char value;
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
	printf("%d", (int)col->value);
	col = col->next;
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
	int splits = 0;
	int lines = 0;
	char next_on = 0;
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
					col->value = 0;
					if (col->prev) {
						col->prev->value = 1;
					}
					next_on = 1;
					splits++;
				}
				col = column_advance(col, lines);
				break;
			case '.':
				if (next_on) {
					col->value = 1;
				}
				next_on = 0;
				col = column_advance(col, lines);
				break;
			default:
				break;
		}
	}
	// Cleanup
	printf("Splits: %d\n", splits);
	fclose(fptr);
	while (columns) {
		Column *n = columns->next;
		free(columns);
		columns = n;
	}
	return EXIT_SUCCESS;
}
