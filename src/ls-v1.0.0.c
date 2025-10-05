#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>


extern int errno;

void do_ls(const char *dir);
void do_ls_long(const char *dir);

int main(int argc, char *argv[]) {
    int opt;
    int long_listing = 0;

    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                long_listing = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [directory...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        if (long_listing)
            do_ls_long(".");
        else
            do_ls(".");
    } else {
        for (int i = optind; i < argc; i++) {
            printf("Directory listing of %s:\n", argv[i]);
            if (long_listing)
                do_ls_long(argv[i]);
            else
                do_ls(argv[i]);
            puts("");
        }
    }
    return 0;
}

void do_ls(const char *dir) {
    DIR *dp;
    struct dirent *entry;

    dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    // Step 1: Read all filenames
    int capacity = 64;
    int count = 0;
    char **names = malloc(capacity * sizeof(char *));
    if (!names) {
        perror("malloc");
        closedir(dp);
        return;
    }

    int maxlen = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.') continue; // skip hidden files

        if (count >= capacity) {
            capacity *= 2;
            char **tmp = realloc(names, capacity * sizeof(char *));
            if (!tmp) {
                perror("realloc");
                for (int i = 0; i < count; i++) free(names[i]);
                free(names);
                closedir(dp);
                return;
            }
            names = tmp;
        }

        names[count] = strdup(entry->d_name);
        if (!names[count]) {
            perror("strdup");
            for (int i = 0; i < count; i++) free(names[i]);
            free(names);
            closedir(dp);
            return;
        }

        int len = strlen(entry->d_name);
        if (len > maxlen) maxlen = len;
        count++;
    }
    closedir(dp);

    if (count == 0) {
        free(names);
        return;
    }

    // Step 2: Get terminal width
    struct winsize w;
    int term_width = 80; // default
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        term_width = w.ws_col;

    // Step 3: Compute column layout
    int col_width = maxlen + 2;
    int cols = term_width / col_width;
    if (cols < 1) cols = 1;
    int rows = (count + cols - 1) / cols;

    // Step 4: Print down-then-across
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = c * rows + r;
            if (i < count)
                printf("%-*s", col_width, names[i]);
        }
        printf("\n");
    }

    // Step 5: Cleanup
    for (int i = 0; i < count; i++)
        free(names[i]);
    free(names);
}


void do_ls_long(const char *dir) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];

    dp = opendir(dir);
    if (dp == NULL) {
        perror("Cannot open directory");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        if (lstat(path, &statbuf) == -1) {
            perror("lstat");
            continue;
        }


        printf( (S_ISDIR(statbuf.st_mode)) ? "d" :
                (S_ISLNK(statbuf.st_mode)) ? "l" : "-");


        printf( (statbuf.st_mode & S_IRUSR) ? "r" : "-");
        printf( (statbuf.st_mode & S_IWUSR) ? "w" : "-");
        printf( (statbuf.st_mode & S_IXUSR) ? "x" : "-");
        printf( (statbuf.st_mode & S_IRGRP) ? "r" : "-");
        printf( (statbuf.st_mode & S_IWGRP) ? "w" : "-");
        printf( (statbuf.st_mode & S_IXGRP) ? "x" : "-");
        printf( (statbuf.st_mode & S_IROTH) ? "r" : "-");
        printf( (statbuf.st_mode & S_IWOTH) ? "w" : "-");
        printf( (statbuf.st_mode & S_IXOTH) ? "x" : "-");

        struct passwd *pw = getpwuid(statbuf.st_uid);
        struct group *gr = getgrgid(statbuf.st_gid);

        printf(" %2ld", statbuf.st_nlink);
        printf(" %-8s %-8s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
        printf(" %8ld", statbuf.st_size);

        char *time_str = ctime(&statbuf.st_mtime);
        time_str[strlen(time_str) - 1] = '\0';
        printf(" %s", time_str);

        printf(" %s\n", entry->d_name);
    }

    closedir(dp);
}


