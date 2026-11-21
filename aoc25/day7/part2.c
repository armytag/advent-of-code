#include <stdio.h>
#include <stdlib.h>

typedef struct Path {
	struct Path *prev;
	struct Path *next;
	int pos;
} Path;

Path*
path_create()
{
	Path *p = malloc(sizeof(Path));
	p->next = (Path *)NULL;
	p->prev = (Path *)NULL;
	p->pos = 0;
	return p;
}

Path*
path_remove(Path *path)
{
	// Remove path from old list
	if (path->prev) { path->prev->next = path->next; }
	if (path->next) { path->next->prev = path->prev; }
	return path;
}

Path*
path_join(Path *old_paths, Path *new_paths) {
	if (!new_paths) {
		return old_paths;
	}
	if (!old_paths) {
		old_paths = new_paths;
		return old_paths;
	}
	Path *end = old_paths;
	// Find end of old_paths
	while (end->next) {
		end = end->next;
	}
	end->next = new_paths;
	new_paths->prev = end;
	return old_paths;
}

void
path_print_all(Path *paths) {
	Path *p = paths;
	printf("Paths: ");
	while (p) {
		printf("%d ", p->pos);
		p = p->next;
	}
	printf("\n");
}

int
main(int argc, char *argv[])
{
	int ch;
	int col = 0;
	int line = 0;
	Path *paths;
	FILE *fptr;
	char *filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "example.txt";
	}
	fptr = fopen(filename, "r");
	if (!fptr) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	// Main logic
	paths = (Path *)NULL;
	Path *new_paths = (Path *)NULL;
	while ((ch = getc(fptr)) != EOF) {
		col++;
		Path *p;
		switch (ch) {
			case '\n':
				col = 0;
				if (!paths) { paths = new_paths; }
				else { path_join(paths, new_paths); }
				new_paths = (Path *)NULL;
				// path_print_all(paths);
				line++;
				printf("Finished line %d\n", line);
				break;
			case 'S':
				printf("Creating beam at position %d\n", col);
				p = path_create();
				p->pos = col;
				new_paths = path_join(new_paths, p);
				break;
			case '^':
				p = paths;
				while (p) {
					Path *n = p->next;
					if (p->pos == col) {
						// printf("Splitting at column %d\n", p->pos);
						// Create two new paths
						Path *left = path_create();
						Path *right = path_create();
						left->pos = col - 1;
						right->pos = col + 1;
						// Add two new paths to new parent
						new_paths = path_join(new_paths, left);
						new_paths = path_join(new_paths, right);
						// Remove path from old list
						path_remove(p);
						if (paths == p) {
							paths = n;
						}
						// Free old path
						free(p);
					}
					p = n;
				}
				break;
			default:
				break;
		}
	}
	path_print_all(paths);
	// Cleanup
	fclose(fptr);
	unsigned long num_paths = 0;
	while (paths) {
		num_paths++;
		Path *n = paths->next;
		free(paths);
		paths = n;
	}
	printf("\n");
	printf("Paths: %lu\n", num_paths);
	return EXIT_SUCCESS;
}
