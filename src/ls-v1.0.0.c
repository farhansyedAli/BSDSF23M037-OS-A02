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

void do_ls(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory : %s\n", dir);
        return;
    }
    errno = 0;
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        printf("%s\n", entry->d_name);
    }

    if (errno != 0)
    {
        perror("readdir failed");
    }

    closedir(dp);
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


