#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Range {
	uint64_t start;
	uint64_t stop;
	struct Range *next;
	struct Range *prev;
} Range;

Range*
range_create(void) {
	Range *range = malloc(sizeof(Range));
	range->start = 0;
	range->stop = 0;
	range->next = (Range *)NULL;
	range->prev = (Range *)NULL;
	return range;
}

void
range_print(Range *range) {
	printf("R: %lu-%lu\n", range->start, range->stop);
}

bool
range_overlaps(Range *a, Range *b) {
	if ((a->start >= b->start - 1 && a->start <= b->stop + 1)
		|| (a->stop >= b->start - 1 && a->stop <= b->stop + 1)
		|| (b->start >= a->start - 1 && b->start <= a->stop + 1)
		|| (b->stop >= a->start - 1 && b->stop <= a->stop + 1)) {
		return true;
	}
	return false;
}

void
range_remove(Range *r) {
	if (r->prev) { r->prev->next = r->next; }
	if (r->next) { r->next->prev = r->prev; }
	free(r);
}

void
range_insert(Range *new_range, Range *ranges) {
	Range *test_range = ranges;
	while (test_range) {
		Range *next = test_range->next;
		if (range_overlaps(new_range, test_range)) {
			if (test_range->start < new_range->start) {
				new_range->start = test_range->start;
			}
			if (test_range->stop > new_range->stop) {
				new_range->stop = test_range->stop;
			}
			range_remove(test_range);
		}
		test_range = next;
	}
	test_range = ranges;
	while (test_range->next) { test_range = test_range->next; }
	new_range->prev = test_range;
	test_range->next = new_range;
}

int
parse_file(char *filename, Range *ranges) {
	FILE *fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	int ch, pch;
	uint64_t num = 0;
	bool ranges_done = false;
	Range *new_range = range_create();
	while (!ranges_done && (ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '0' ... '9':
				num = num * 10 + (ch - '0');
				break;
			case '-':
				new_range->start = num;
				num = 0;
				break;
			case '\n':
				if (pch == '\n') {
					ranges_done = true;
					break;
				}
				new_range->stop = num;
				range_insert(new_range, ranges);
				new_range = range_create();
				num = 0;
				break;
			default:
				break;
		}
		pch = ch;
	}
	fclose(fptr);
	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[]) {
	uint64_t count = 0;
	char *filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	Range *ranges = range_create();
	if (parse_file(filename, ranges) != EXIT_SUCCESS) {
		perror("Error parsing file %s");
		return EXIT_FAILURE;
	}
	// Remove empty first items
	if (!ranges->start && !ranges->stop && ranges->next) {
		Range *r = ranges->next;
		range_remove(ranges);
		ranges = r;
	}
	// Main logic
	// Cleanup
	while(ranges) {
		if (ranges->start || ranges->stop) {
			uint64_t s = ranges->stop - ranges->start + 1;
			count += s;
			printf("%lu-%lu\t%lu\t%lu\n", ranges->start, ranges->stop, s, count);
		}
		Range *next = ranges->next;
		free(ranges);
		ranges = next;
	}
	printf("Count: %lu\n", count);
	return EXIT_SUCCESS;
}
