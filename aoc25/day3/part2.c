# include<stdio.h>
# include<stdlib.h>
# include<stdint.h>
# include<stdbool.h>

void shift_and_append(int v, int *digits, size_t len) {
	bool shifting = false;
	for (size_t i = 0; i < len - 1; i++) {
		if (shifting || digits[i+1] > digits[i]) {
			digits[i] = digits[i+1];
			shifting = true;
		}
	}
	if (shifting || v > digits[len-1]) {
		digits[len-1] = v;
	}
}

int main() {
	const size_t num_digits = 12;
	int *digits = (int *)calloc(num_digits, sizeof(int));
	int ch = 0;
	uint64_t total = 0;
	FILE *fptr = fopen("input.txt", "r");
	if (!fptr) {
		printf("File not found");
		return EXIT_FAILURE;
	}
	while((ch = fgetc(fptr)) != EOF) {
		int v = ch - '0';
		switch (ch) {
			case '0' ... '9':
				shift_and_append(v, digits, num_digits);
				break;
			case '\n':
				;
				uint64_t sum = 0;
				for (size_t i = 0; i < num_digits; i++) {
					printf("%d", digits[i]);
					sum = sum * 10 + digits[i];
					digits[i] = 0;
				}
				total += sum;
				printf("\t%lu\n", total);
				break;
			default:
				break;
		}
	}
	printf("Total: %lu\n", total);
	fclose(fptr);
	return EXIT_SUCCESS;
}
