#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

int imgd (const char* const* imgs, const char* dir_name, int size);
int imgd_rand (const char* const* imgs, const char* dir_name, int size);
int fcount (const char* dir_name);
void feh (const char* fname, const char* dir_name);
const char* const*  fnames (const char* dir_name, int size);
void sigpoll (int poll_factor);
void sig_usr1 (int signum);

static int sig_usr1_flag = 0;

int fcount(const char* dir_name) {
    /** Count the number of files in a directory, dir_name **/
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dir_name)) != NULL) {
        // count files in default dir
        unsigned fc = 0;
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

const char* const* fnames(const char* dir_name, int size) {
    /** build array of size, size, of file names in the directory, dir_name **/
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dir_name)) != NULL) {
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
        return (const char* const*)imgs;
    } else {
        return NULL;
    }
}

void feh(const char* fname, const char* dir_name) {
    char feh[256];
    sprintf(feh, "feh --bg-fill %s/%s", dir_name, fname);
    system(feh);
}

int imgd(const char* const* imgs, const char* dir_name, int size) {
    printf("starting bgimgd...\n");
    int cur = 0;
    int polfact = (60 * interval) / sig_poll_interval;
    while (1) {
        if (cur == size)
            cur = 0;
        if (imgs[cur] == NULL) {
            cur++;
            continue;
        } else {
            feh(imgs[cur], dir_name);                         
            cur++;                                           
            sigpoll(polfact);
        }
    }                                                   
    return 0;
}

int imgd_rand(const char* const* imgs, const char* dir_name, int size) {
    printf("starting bgimgd in random mode...\n");
    int polfact = (60 * interval) / sig_poll_interval;
    while (1) {
        time_t t;
        srand((unsigned) time(&t));
        int r = rand() % size;

        if (imgs[r] == NULL) 
            continue;
        else {
            feh(imgs[r], dir_name);
            sigpoll(polfact);
        }
    }
    return 0;
}

void sigpoll(int poll_factor) {
    for (int i = 0; i < poll_factor; i++) {
        if (sig_usr1_flag) {
            sig_usr1_flag = 0;
            return;
        }
        printf("sigpoll: %d/%d, sec: %d\n", 
                i, poll_factor, sig_poll_interval);
        sleep(sig_poll_interval);
    }
}

void sig_usr1(int signum) {
    if (signum == SIGUSR1) {
        printf("user advance queue signal received...\n");
        sig_usr1_flag = 1;
    }
}

int main(int argc, char** argv) {
    int verbose = 0;
    int random = 0;
    
    unsigned dirlen = strlen(bgdir);
    char* dir = malloc(dirlen + 1);
    strcpy(dir, bgdir);

    // register signal handling
    signal(SIGUSR1, sig_usr1);

    // shell arg handling
    if (argc == 1) {
        int fc = fcount(bgdir);
        const char* const* imgs = fnames(bgdir, fc);
        return imgd(imgs, bgdir, fc);
    } else if (argc == 2) {
        if (argv[1][0] == '-' && strstr(argv[1], "r"))
            random = 1;
    } else if (argc == 3) {
        if (argv[1][0] == '-' && strstr(argv[1], "d")) {
            unsigned altdirlen = strlen(argv[2]);
            if (altdirlen) {
                if (altdirlen > dirlen)
                    dir = realloc(dir, altdirlen + 1);
                strcpy(dir, argv[2]);
            }
        }
    } else {
        return 1;
    }

    // start non-standard daemon
    int fc = fcount(dir);
    const char* const* imgs = fnames(dir, fc);
    if (random)
        imgd_rand(imgs, dir, fc);
    else 
        imgd(imgs, dir, fc);

    return 0;
}

