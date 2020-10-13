#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc != 2 || (argc == 2 && !strcmp(argv[1], "-h"))) {
		printf("Usage: %s <file.bank>\n", argv[0]);
		return argc != 2;
	}

	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		printf("Could not open file '%s'\n", argv[1]);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	if (length <= 0) {
		printf("Truncated file?\n");
		fclose(fp);
		return 1;
	}

	rewind(fp);

	char *buf = malloc(length);
	if (!buf) {
		printf("Memory allocation failed\n");
		fclose(fp);
		return 1;
	}

	if (fread(buf, 1, length, fp) != length) {
		printf("Input failure (%s)\n", argv[1]);
		fclose(fp);
		return 1;
	}
	fclose(fp);

	if (memcmp(buf, "RIFF", 4)) {
		printf("Warning: file does not start with a RIFF header.  It may not actually be an FMOD bank file.\n");
	}

	for (; length && memcmp(buf, "FSB5", 4); length--, buf++);
	if (!length) {
		printf("Truncated file?\n");
		return 1;
	}

	char fbuf[512];
	snprintf(fbuf, sizeof(fbuf), "%s.fsb", argv[1]);
	FILE *ofp = fopen(fbuf, "w");
	if (!ofp) {
		printf("Could not create file '%s'\n", fbuf);
		return 1;
	}

	if (fwrite(buf, 1, length, ofp) != length) {
		printf("Output failure (%s)\n", fbuf);
		return 1;
	}

	printf("All done.  Your file is in '%s'.\n", fbuf);
}
