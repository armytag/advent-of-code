#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define ULL unsigned long long
#define ULL_CHARS 24

ULL invalid_in_range(ULL low, ULL high) {
	ULL sum = 0;
	char *n_str = (char *)calloc(ULL_CHARS, sizeof(char));
	for (ULL num = low; num <= high; num++) {
		int digits = 0;
		// Reset n_str
		for (int i = 0; i < ULL_CHARS; i++) {
			n_str[i] = '\0';
		}

		for (ULL n = num; n > 0; n /= 10) {
			digits++;
			n_str[ULL_CHARS - digits] = (n % 10) + '0';
		}

		// Skip numbers with odd number of digits
		for (int d = 1; d <= (digits / 2); d++) {
			bool is_match = true;
			if (digits % d) { continue; }
			for (int i = 0; i < (digits - d); i++) {
				if (n_str[ULL_CHARS - digits + i] != n_str[ULL_CHARS - (digits - d) + i]) {
					is_match = false;
					break;
				}
			}
			if (is_match) {
				printf("\t%llu\n", num);
				sum += num;
				break;
			}
		}
	}
	free(n_str);
	return sum;
}

int main() {
	FILE *fptr;
	ULL num = 0, total = 0, low = 0, high = 0;
	fptr = fopen("input.txt", "r");
	int ch;
	while ((ch = fgetc(fptr)) != EOF) {
		switch (ch) {
			case '0' ... '9':
				num = num * 10 + (ch - '0');
				break;
			case '-':
				low = num;
				num = 0;
				break;
			case ',':
			case '\n':
				high = num;
				num = 0;
				total += invalid_in_range(low, high);
				printf("%llu-%llu\t%llu\n", low, high, total);
				break;
			default:
				break;
		}
	}
	fclose(fptr);
	printf("Total: %llu\n", total);
	return EXIT_SUCCESS;
}
