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

void
range_print(Range *range) {
	printf("R: %lu-%lu\n", range->start, range->stop);
}

Ingredient*
ingredient_create(void) {
	Ingredient *ingredient = malloc(sizeof(Ingredient));
	ingredient->value = 0;
	ingredient->next = (Ingredient *)NULL;
	return ingredient;
}

void
ingredient_print(Ingredient *ingredient) {
	printf("I: %lu\n", ingredient->value);
}

int
parse_file(char *filename, Range *ranges, Ingredient *ingredients) {
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
				else if (!ranges_done) {
					curr_range->stop = num;
					Range *new_range = range_create();
					curr_range->next = new_range;
					curr_range = new_range;
				} else {
					curr_ingredient->value = num;
					Ingredient *new_ingredient = ingredient_create();
					curr_ingredient->next = new_ingredient;
					curr_ingredient = new_ingredient;
				}
				num = 0;
				break;
			default:
				break;
		}
		pch = ch;
	}
	fclose(fptr);
	// Remove empty first items
	Range *r = ranges;
	while (r->next != curr_range && r->next != (Range *)NULL) { r = r->next; }
	r->next = (Range *)NULL;
	free(curr_range);
	Ingredient *i = ingredients;
	while (i->next != curr_ingredient && i->next != (Ingredient *)NULL) { i = i->next; }
	i->next = (Ingredient *)NULL;
	free(curr_ingredient);
	return EXIT_SUCCESS;
}

int
main(void) {
	int count = 0;
	char *filename = "input.txt";
	Range *ranges = range_create();
	Ingredient *ingredients = ingredient_create();
	if (parse_file(filename, ranges, ingredients) != EXIT_SUCCESS) {
		perror("Error parsing file %s");
		return EXIT_FAILURE;
	}
	Ingredient *i = ingredients;
	while (i) {
		Range *r = ranges;
		while (r) {
			if (i->value >= r->start && i->value <= r->stop) {
				count++;
				break;
			}
			r = r->next;
		}
		i = i->next;
	}
	printf("Count: %d\n", count);
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
