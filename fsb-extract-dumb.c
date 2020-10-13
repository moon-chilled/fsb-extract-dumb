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
	long ilength = ftell(fp);
	if (ilength <= 0) {
		printf("Truncated file?\n");
		fclose(fp);
		return 1;
	}
	size_t length = ilength;

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

	size_t l = strlen(argv[1]);
	char *ofname;
	if (!strcmp(argv[1]+l-5, ".bank")) {
		ofname = argv[1];
		strcpy(ofname+l-4, "fsb");
	} else {
		char fbuf[512];
		snprintf(fbuf, sizeof(fbuf), "%s.fsb", argv[1]);
		ofname = fbuf;
	}

	FILE *ofp = fopen(ofname, "w");
	if (!ofp) {
		printf("Could not create file '%s'\n", ofname);
		return 1;
	}

	if (fwrite(buf, 1, length, ofp) != length) {
		printf("Output failure (%s)\n", ofname);
		return 1;
	}

	printf("All done!  Your file is in '%s'.\n", ofname);
}
