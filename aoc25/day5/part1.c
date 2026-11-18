#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Range {
	uint64_t start;
	uint64_t stop;
	struct Range *next;
} Range;

typedef struct Ingredient {
	uint64_t value;
	struct Ingredient *next;
} Ingredient;

Range*
range_create(void) {
	Range *range = malloc(sizeof(Range));
	range->start = 0;
	range->stop = 0;
	range->next = (Range *)NULL;
	return range;
}

Ingredient*
ingredient_create(void) {
	Ingredient *ingredient = malloc(sizeof(Ingredient));
	ingredient->value = 0;
	ingredient->next = (Ingredient *)NULL;
	return ingredient;
}

int
parse_file(char *filename, Range *ranges, Ingredient *ingredients) {
	printf("%lu %lu\n", ranges->start, ingredients->value);
	FILE *fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	int ch, pch;
	uint64_t num = 0;
	bool ranges_done = false;
	Range *curr_range = ranges;
	Ingredient *curr_ingredient = ingredients;
	while ((ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '0' ... '9':
				num = num * 10 + (ch - '0');
				break;
			case '-':
				curr_range->start = num;
				num = 0;
				break;
			case '\n':
				if (pch == '\n') {
					ranges_done = true;
				}
				else if (ranges_done) {
					curr_range->stop = num;
				} else {

				}
				num = 0;
				break;
			default:
				break;
		}
		pch = ch;
	}
	return EXIT_SUCCESS;
}

int
main(void) {
	char *filename = "example.txt";
	Range *ranges = range_create();
	Ingredient *ingredients = ingredient_create();
	if (parse_file(filename, ranges, ingredients) != EXIT_SUCCESS) {
		perror("Error parsing file %s");
		return EXIT_FAILURE;
	}
	// Cleanup
	while(ranges) {
		Range *next = ranges->next;
		free(ranges);
		ranges = next;
	}
	while (ingredients) {
		Ingredient *next = ingredients->next;
		free(ingredients);
		ingredients = next;
	}
	return EXIT_SUCCESS;
}
