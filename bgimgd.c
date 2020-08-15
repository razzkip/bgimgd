#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

int main(int argc, char** argv) {
	int verbose = 0;

	if (argc == 1) {
		DIR* dir;
		struct dirent* ent;
		if ((dir = opendir(bgdir)) != NULL) {
			// count files in default dir
			unsigned int fc = 0;
			while ((ent = readdir(dir)) != NULL && fc < UINT_MAX) {
				if (ent->d_type == DT_REG) {
					fc++;
				}
			}
			
			// get file names
			rewinddir(dir);
			char** imgs = malloc(sizeof(char*) * fc);
			for (int i = 0; i < fc; i++) {
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

			// start daemon loop
			int init = 0;
			unsigned int cur = 0;
			while (1) {
				if (cur == fc)
					cur = 0;

				if (imgs[cur] == NULL) {
					cur++;
					continue;
				}

				char feh[256];
				strncpy(feh, "feh --bg-fill ", 255);
				strncat(feh, bgdir, 255);
				strncat(feh, "/", 255);
				strncat(feh, imgs[cur], 255);
				system(feh);

				cur++;
				
				sleep((int)(interval * 60));
			}
		} else {
			printf("invalid base dir...\n");
		}
	}

	return 0;
}


