#include <stdio.h>

int
main(int argc, char *argv[]) {
	printf("Part1\t%d", argc);
	if (argc) {
		printf("\t%s", argv[0]);
	}
	printf("\n");
}
