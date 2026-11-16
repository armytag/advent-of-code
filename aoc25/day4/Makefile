all: part1.out part2.out

part1.out: part1.c ../../utils/fileio.c
	clang -Wall -Wextra -Werror -I../../utils part1.c ../../utils/fileio.c -o part1.out

part2.out: part2.c
	clang -Wall -Werror part2.c -o part2.out

