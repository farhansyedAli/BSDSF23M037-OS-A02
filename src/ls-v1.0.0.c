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


#define COLOR_RESET   "\033[0m"
#define COLOR_DIR     "\033[1;34m"  // Blue for directories
#define COLOR_EXEC    "\033[1;32m"  // Green for executables
#define COLOR_LINK    "\033[1;36m"  // Cyan for symbolic links
#define COLOR_REG     "\033[0m"     // Default for regular files


extern int errno;

void do_ls(const char *dir, int recursive_flag);
void do_ls_long(const char *dir);
void do_ls_horizontal(const char *dir);


int compare_names(const void *a, const void *b) {
    const char *nameA = *(const char **)a;
    const char *nameB = *(const char **)b;
    return strcmp(nameA, nameB);
}

const char* get_color(struct stat *st) {
    if (S_ISDIR(st->st_mode))
        return COLOR_DIR;
    else if (S_ISLNK(st->st_mode))
        return COLOR_LINK;
    else if (st->st_mode & S_IXUSR)
        return COLOR_EXEC;
    else
        return COLOR_REG;
}


int main(int argc, char *argv[]) {
    int opt;
    int display_mode = 0;     // 0 = default (down-then-across), 1 = long, 2 = horizontal
    int recursive_flag = 0;   // 1 = recursive (-R)

    while ((opt = getopt(argc, argv, "lxR")) != -1) {
        switch (opt) {
            case 'l':
                display_mode = 1;
                break;
            case 'x':
                display_mode = 2;
                break;
            case 'R':
                recursive_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l | -x | -R] [directory...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        if (display_mode == 1)
            do_ls_long(".");
        else if (display_mode == 2)
            do_ls_horizontal(".");
        else
            do_ls(".", recursive_flag);
    } else {
        for (int i = optind; i < argc; i++) {
            printf("Directory listing of %s:\n", argv[i]);
            if (display_mode == 1)
                do_ls_long(argv[i]);
            else if (display_mode == 2)
                do_ls_horizontal(argv[i]);
            else
                do_ls(argv[i], recursive_flag);
            puts("");
        }
    }

    return 0;
}




void do_ls(const char *dir, int recursive_flag) {
    DIR *dp;
    struct dirent *entry;

    printf("\n%s:\n", dir); // Print directory header like ls -R

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

    // Sort filenames alphabetically before display
    qsort(names, count, sizeof(char *), compare_names);

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
            if (i < count) {
                struct stat st;
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

                if (lstat(path, &st) == -1) {
                    perror("lstat");
                    continue;
                }

                const char *color = get_color(&st);
                printf("%s%-*s%s", color, col_width, names[i], COLOR_RESET);
            }
        }
        printf("\n");
    }

    // Step 5: Recursion
    if (recursive_flag) {
        for (int i = 0; i < count; i++) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

            struct stat st;
            if (lstat(path, &st) == -1) continue;

            if (S_ISDIR(st.st_mode)) {
                if (strcmp(names[i], ".") != 0 && strcmp(names[i], "..") != 0) {
                    do_ls(path, recursive_flag); // recursive call
                }
            }
        }
    }

    // Step 6: Cleanup
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


void do_ls_horizontal(const char *dir) {
    DIR *dp;
    struct dirent *entry;

    dp = opendir(dir);
    if (!dp) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    // Step 1: Gather filenames
    int capacity = 64, count = 0, maxlen = 0;
    char **names = malloc(capacity * sizeof(char *));
    if (!names) {
        perror("malloc");
        closedir(dp);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        if (count >= capacity) {
            capacity *= 2;
            names = realloc(names, capacity * sizeof(char *));
            if (!names) {
                perror("realloc");
                closedir(dp);
                return;
            }
        }
        names[count] = strdup(entry->d_name);
        if (!names[count]) {
            perror("strdup");
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

    // Step 2: Determine terminal width
    struct winsize w;
    int term_width = 80; // fallback
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        term_width = w.ws_col;

    int col_width = maxlen + 2;
    int current_width = 0;

    // Step 3 & 4: Print filenames left-to-right
    for (int i = 0; i < count; i++) {
        if (current_width + col_width > term_width) {
            printf("\n");
            current_width = 0;
        }
        printf("%-*s", col_width, names[i]);
        current_width += col_width;
        free(names[i]);
    }
    printf("\n");
    free(names);
}


