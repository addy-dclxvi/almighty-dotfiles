/* See LICENSE file for copyright and license details. */
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define FLAG(x)  (flag[(x)-'a'])

static void test(const char *, const char *);

static bool match = false;
static bool flag[26];
static struct stat old, new;

int
main(int argc, char *argv[]) {
	struct dirent *d;
	char buf[BUFSIZ], *p;
	DIR *dir;
	int opt;

	while((opt = getopt(argc, argv, "abcdefghln:o:pqrsuwx")) != -1)
		switch(opt) {
		case 'n': /* newer than file */
		case 'o': /* older than file */
			if(!(FLAG(opt) = !stat(optarg, (opt == 'n' ? &new : &old))))
				perror(optarg);
			break;
		default:  /* miscellaneous operators */
			FLAG(opt) = true;
			break;
		case '?': /* error: unknown flag */
			fprintf(stderr, "usage: %s [-abcdefghlpqrsuwx] [-n file] [-o file] [file...]\n", argv[0]);
			exit(2);
		}
	if(optind == argc)
		while(fgets(buf, sizeof buf, stdin)) {
			if((p = strchr(buf, '\n')))
				*p = '\0';
			test(buf, buf);
		}
	for(; optind < argc; optind++)
		if(FLAG('l') && (dir = opendir(argv[optind]))) {
			/* test directory contents */
			while((d = readdir(dir)))
				if(snprintf(buf, sizeof buf, "%s/%s", argv[optind], d->d_name) < sizeof buf)
					test(buf, d->d_name);
			closedir(dir);
		}
		else
			test(argv[optind], argv[optind]);

	return match ? 0 : 1;
}

void
test(const char *path, const char *name) {
	struct stat st, ln;

	if(!stat(path, &st) && (FLAG('a') || name[0] != '.')          /* hidden files      */
	&& (!FLAG('b') || S_ISBLK(st.st_mode))                        /* block special     */
	&& (!FLAG('c') || S_ISCHR(st.st_mode))                        /* character special */
	&& (!FLAG('d') || S_ISDIR(st.st_mode))                        /* directory         */
	&& (!FLAG('e') || access(path, F_OK) == 0)                    /* exists            */
	&& (!FLAG('f') || S_ISREG(st.st_mode))                        /* regular file      */
	&& (!FLAG('g') || st.st_mode & S_ISGID)                       /* set-group-id flag */
	&& (!FLAG('h') || (!lstat(path, &ln) && S_ISLNK(ln.st_mode))) /* symbolic link     */
	&& (!FLAG('n') || st.st_mtime > new.st_mtime)                 /* newer than file   */
	&& (!FLAG('o') || st.st_mtime < old.st_mtime)                 /* older than file   */
	&& (!FLAG('p') || S_ISFIFO(st.st_mode))                       /* named pipe        */
	&& (!FLAG('r') || access(path, R_OK) == 0)                    /* readable          */
	&& (!FLAG('s') || st.st_size > 0)                             /* not empty         */
	&& (!FLAG('u') || st.st_mode & S_ISUID)                       /* set-user-id flag  */
	&& (!FLAG('w') || access(path, W_OK) == 0)                    /* writable          */
	&& (!FLAG('x') || access(path, X_OK) == 0)) {                 /* executable        */
		if(FLAG('q'))
			exit(0);
		match = true;
		puts(name);
	}
}
