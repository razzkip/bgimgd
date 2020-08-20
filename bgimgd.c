#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

int imgd(char** imgs, const char* dirname, int size);
int imgd_rand(char** imgs, const char* dirname, int size);
int fcount(const char* dirname);
void feh(const char* fname, const char* dirname);
char** fnames(const char* dirname, int size);

int fcount(const char* dirname) {
	/** Count the number of files in a directory, dirname **/
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(dirname)) != NULL) {
		// count files in default dir
		unsigned int fc = 0;
		while ((ent = readdir(dir)) != NULL && fc < INT_MAX) {
			if (ent->d_type == DT_REG)
				fc++;
		}
		rewinddir(dir);
		return fc;
	} else {
		rewinddir(dir);
		return -1;
	}
}

char** fnames(const char* dirname, int size) {
	/** build array of size, size, of file names in the directory, dirname **/
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(dirname)) != NULL) {
		char** imgs = malloc(sizeof(char*) * size);
		for (int i = 0; i < size; i++) {
			imgs[i] = malloc(sizeof(char) * NAME_MAX);
			ent = readdir(dir);
			if (strncmp(ent->d_name, "..", 2) == 0
					|| strncmp(ent->d_name, ".", 1) == 0) {
				imgs[i] = NULL;
				continue;
			}
	                                                        
			strncpy(imgs[i], ent->d_name, NAME_MAX);
			imgs[i][NAME_MAX - 1] = '\0';
		}
	
		rewinddir(dir);
		return imgs;
	} else {
		return NULL;
	}
}

void feh(const char* fname, const char* dirname) {
	char feh[256];
	sprintf(feh, "feh --bg-fill %s/%s", dirname, fname);
	system(feh);
}

int imgd(char** imgs, const char* dirname, int size) {
	unsigned int cur = 0;
	while (1) {
		if (cur == size)
			cur = 0;
                                                               
		if (imgs[cur] == NULL) {
			cur++;
			continue;
		}

		feh(imgs[cur], dirname);
		cur++;
		
		sleep((int)(interval * 60));
	}
	return 0;
}

int imgd_rand(char** imgs, const char* dirname, int size) {
	while (1) {
		time_t t;
		srand((unsigned) time(&t));
		int r = rand() % size;

		if (imgs[r] == NULL) {
			continue;
		}

		feh(imgs[r], dirname);

		sleep((int)(interval * 60));
	}
	return 0;
}

int main(int argc, char** argv) {
	int verbose = 0;

	if (argc == 1) {
		int fc = fcount(bgdir);
		char** imgs = fnames(bgdir, fc);
		return imgd(imgs, bgdir, fc);
	} else if (argc == 2) {
		if (argv[1][0] == '-' && strstr(argv[1], "r")) {
			// random order
		int fc = fcount(bgdir);
		char** imgs = fnames(bgdir, fc);
		return imgd_rand(imgs, bgdir, fc);	
		}
	}

	return 0;
}


