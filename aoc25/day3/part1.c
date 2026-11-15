# include<stdio.h>
# include<stdlib.h>
# include<stdint.h>

int main() {
	int ch = 0, total = 0, left = 0, right = 0;
	FILE *fptr = fopen("input.txt", "r");
	if (!fptr) {
		printf("File not found");
		return EXIT_FAILURE;
	}
	while((ch = fgetc(fptr)) != EOF) {
		int v = ch - '0';
		switch (ch) {
			case '0' ... '9':
				if (right > left) {
					left = right;
					right = v;
				} else if (v >= right) {
					right = v;
				}
				break;
			case '\n':
				total += left * 10 + right;
				printf("%d%d\t%d\n", left, right, total);
				left = 0;
				right = 0;
				break;
			default:
				break;
		}
	}
	printf("Total: %d\n", total);
	fclose(fptr);
	return EXIT_SUCCESS;
}
