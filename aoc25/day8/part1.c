#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum Dimensions {
	DIM_X,
	DIM_Y,
	DIM_Z,
	DIM_COUNT
};

typedef struct Junction {
	struct Junction *next;
	struct Junction *head;
	int32_t x;
	int32_t y;
	int32_t z;
	int circuit_size;
} Junction;

typedef struct Distance {
	struct Distance *prev;
	struct Distance *next;
	Junction *p1;
	Junction *p2;
	int64_t dist;
} Distance;

const int MAX_PAIRS = 1000;

Junction*
junction_create()
{
	Junction *j = (Junction *)malloc(sizeof(Junction));
	j->next = (Junction *)NULL;
	j->head = j;
	j->x = 0;
	j->y = 0;
	j->z = 0;
	j->circuit_size = 1;
	return j;
}

uint64_t
junction_distance(Junction *p1, Junction *p2)
{
	int64_t dx = (p1->x - p2->x);
	int64_t dy = (p1->y - p2->y);
	int64_t dz = (p1->z - p2->z);
	return (int64_t)(dx*dx + dy*dy + dz*dz);
}

void
junction_print(Junction *j)
{
	printf("(%5u,%5u,%5u,)", j->x, j->y, j->z);
}

void
junction_print_full(Junction *j)
{
	junction_print(j);
	printf(" s:%3d, h:%p\n", j->circuit_size, j->head);
}

void
junction_print_all(Junction *junctions)
{
	Junction *j = junctions;
	printf("Junctions:\n");
	while (j) {
		junction_print_full(j);
		j = j->next;
	}
}

void
junction_cleanup(Junction *junctions)
{
	while (junctions) {
		Junction *n = junctions->next;
		free(junctions);
		junctions = n;
	}
}

Distance*
distance_create(Junction *p1, Junction *p2)
{
	Distance *d = (Distance *)malloc(sizeof(Distance));
	d->prev = d;
	d->next = d;
	d->p1 = p1;
	d->p2 = p2;
	d->dist = junction_distance(p1, p2);
	return d;
}

void
distance_remove(Distance *d)
{
	if (!d) { return; }
	if (d->next) { d->next->prev = d->prev; }
	if (d->prev) { d->prev->next = d->next; }
	free(d);
}

void
distance_insert_before(Distance *reference, Distance *new)
{
	if (!new) { return; }
	if (!reference) {
		reference = new;
		return;
	}
	Distance *prev = reference->prev;
	prev->next = new;
	new->prev = prev;
	reference->prev = new;
	new->next = reference;
}

Distance*
distance_try_insertion(Distance *distances, Distance *new, int threshold)
{
	if (!new) { return distances; }
	if (!distances) {
		distances = new;
		return distances;
	}
	if (new->dist < distances->dist) {
		distance_insert_before(distances, new);
		distances = new;
		return distances;
	}
	Distance *d = distances->next;
	for (int c = 0; c < threshold; c++) {
		if (d == distances || new->dist < d->dist) {
			distance_insert_before(d, new);
			break;
		}
		d = d->next;
	}
	return distances;
}

void
distance_print(Distance *d)
{
	junction_print(d->p1);
	printf(" -> ");
	junction_print(d->p2);
	printf(" d: %lu\n", d->dist);
}

void
distance_print_all(Distance *distances)
{
	Distance *d = distances;
	printf("Distances:\n");
	int i = 1;
	do {
		printf("%d\t", i);
		distance_print(d);
		d = d->next;
		i++;
	} while (d != distances);
}

void
distance_cleanup(Distance *distance)
{
	if (!distance) { return; }
	Distance *d = distance->next;
	while (d != distance) {
		Distance *n = d->next;
		free(d);
		d = n;
	}
	free(distance);
}

Junction*
parse_junctions(char *filename)
{
	Junction *junctions;
	int32_t num = 0;
	int dim = DIM_X;
	FILE *fptr = fopen(filename, "r");
	int ch;
	if (!fptr) {
		perror("Error opening file");
		return (Junction *)NULL;
	}
	junctions = junction_create();
	Junction *junc = junctions;
	Junction *last_junc = (Junction *)NULL;
	while ((ch = getc(fptr)) != EOF) {
		switch (ch) {
			case '\n':
				junc->z = num;
				Junction *j = junction_create();
				junc->next = j;
				last_junc = junc;
				junc = j;
				num = 0;
				dim = DIM_X;
				break;
			case '0' ... '9':
				num = num * 10 + (ch - '0');
				break;
			case ',':
				if (dim == DIM_X) {
					junc->x = num;
					dim = DIM_Y;
					num = 0;
				} else if (dim == DIM_Y) {
					junc->y = num;
					dim = DIM_Z;
					num = 0;
				} else {
					perror("Wrong number of dimensions");
					junction_cleanup(junctions);
					return (Junction *)NULL;
				}
				break;
			default:
				break;
		}
	}
	// Remove empty last junction
	if (!junc->x && !junc->y && !junc->z && last_junc->next == junc) {
		last_junc->next = (Junction *)NULL;
		free(junc);
	}
	fclose(fptr);
	return junctions;
}

Distance*
find_distances(Junction *junctions, int threshold)
{
	if (!junctions) {
		return (Distance *)NULL;
	}
	Distance *distances = (Distance *)NULL;
	Junction *p1 = junctions;
	while (p1) {
		Junction *p2 = p1->next;
		while (p2) {
			Distance *d = distance_create(p1, p2);
			// distance_print(d);
			distances = distance_try_insertion(distances, d, threshold);
			p2 = p2->next;
		}
		p1 = p1->next;
	}
	return distances;
}

int
main(int argc, char *argv[])
{
	Junction *junctions = (Junction *)NULL;
	Distance *distances = (Distance *)NULL;
	char *filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	// Parse file
	if (!(junctions = parse_junctions(filename))) {
		perror("Error parsing junctions");
		return EXIT_FAILURE;
	}
	// junction_print_all(junctions);
	// Find shortest distances
	if (!(distances = find_distances(junctions, MAX_PAIRS))) {
		perror("Error finding distances");
		return EXIT_FAILURE;
	}
	distance_print_all(distances);
	// Cleanup
	junction_cleanup(junctions);
	distance_cleanup(distances);
	return EXIT_SUCCESS;
}
