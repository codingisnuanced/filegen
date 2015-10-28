#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "jvutils.h"
#include <malloc/malloc.h>

//
// STRINGS & COMPARISONS
//

int alphabetLookup(int c) {
	const char * alphabeticals = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
	static int initialized;
	static char table[CHAR_MAX+1];

	if(!initialized) {
		// leave all non-alphabeticals in their relative order, but below alphabeticals
		int i, j;
		for(i = j = 1; i < CHAR_MAX+1; ++i) {
			if(strchr(alphabeticals, i)) continue;
			table[i] = j++; // only increment j when a non-alpha is found
			// table will now be filled with non-alphabeticals starting from ASCII code 1 (because 0 is \0)
		}

		// now run through the alphabeticals
		for(i = 0; alphabeticals[i]; ++i) { // as long as there is a value for i in alphabeticals
			// use the int value of char at i in alphabeticals as the index (chars are ints)
			// set the value at index in table to j
			// Remember j will initially be at the index after non-alphabeticals at this point
			int index = (int)alphabeticals[i]; // begins at 0
			table[index] = j++;
		}
		initialized = 1;
	}
	// return the computed ordinal of the provided character
	// return the int value if not in our table
	if(c < 0 || c > CHAR_MAX) return c;
	return table[c];
}

// better sorting as A is less than b with strcmp()
int compareStrings(const char *a, const char *b) {
	// get collated table value of character
	// a and A will not have the same collated table value
	int ax = alphabetLookup(*a);
	int bx = alphabetLookup(*b);

	// A has more weight than a
	// A has less weigth than b
	int weight = 0;

	// while there are characters to compare
	do {
		char al = tolower(*a);
		char bl = tolower(*b);

		if(ax != bx) { // characters differ i.e. a and A differ
			if(al != bl) {
				return alphabetLookup(al) - alphabetLookup(bl);
			}

			if(weight == 0) {
				weight = ax - bx;
			}
		}
		// go to the next character
		ax = alphabetLookup(*++a);
		bx = alphabetLookup(*++b);
	} while(ax && bx);

	// if the words are equal, use the weight as a tie-breaker
	return (ax != bx) ? !bx - !ax : weight;
}

void replaceSpaces(char *string, char replacement) {
	for(int i = 0; i <= strlen(string); ++i) {
		if(string[i] == ' ') string[i] = replacement;
	}
}

void stringToLowercase(char *string) {
	for(int i = 0; i <= strlen(string); ++i) {
		if(isupper(string[i])) string[i] = tolower(string[i]);
	}
}


// length: number of characters per string
char* randomURLString(int length) {
	//73 chars
	// const char *validChars = "$-_.+!*'(),1234567890aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";

	// 68 chars
	const char *validChars = "$-_+()1234567890aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
	int i;
	char *name = malloc(length+1);
	name[0] = '\0';
	for(i = 0; i < length; ++i) {
		name[i] = validChars[arc4random_uniform(68)];
	}
	name[length] = '\0';
	return name;
}


//
// FILES & DIRECTORIES FUNCTIONS
//
// returns 0  on success
int createDir(const char *path, mode_t mode) {
	struct stat st = {0};

	int sret = stat(path, &st);

	if(sret == -1) {
		int ret = mkdir(path, mode);
		return ret;
	} else {
		return sret;
	}
}

int isDirectory(const char *path) {
	// stat will exit non-zero on success, and 0 if an error occurs.
	struct stat statbuf;
	if(stat(path, &statbuf) != 0) {
		return 0;
	}
	return S_ISDIR(statbuf.st_mode);
}

int isRegularFile(const char *path) {
	// stat will exit 0 on success, and >0 if an error occurs.
	struct stat path_stat;
	if(stat(path, &path_stat) != 0) {
		return 0;
	}
	return S_ISREG(path_stat.st_mode);
}

char* getAbsoluteWorkingDirectory() {
	char *buf;
	long path_max;
	size_t path_size;
	char *currdir;

	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	buf = malloc(path_size);

	currdir = getcwd(buf, path_size);

	return currdir;
}

// creartes directories with random names
// returns pointer to pointer of directories created
// char** createRandomDirectories(char *folder, int number) {
// 	int i;
// 	char *randomString;
// 	char **directories = malloc(number * sizeof(char*));
// 	char *finalPath;
// 	for(i = 0; i < number; ++i) {
// 		finalPath = malloc(strlen(folder) + 7);
// 		finalPath[0] = '\0';
// 		randomString = randomURLString(5);
// 		(void)strncat(finalPath, folder, strlen(folder));
// 		(void)strncat(finalPath, "/", 1);
// 		(void)strncat(finalPath, randomString, strlen(randomString));
// 		finalPath[strlen(folder) + 6] = '\0';
// 		directories[i] = finalPath;
// 		free(randomString);
// 	}
// 	return directories;
// }

// creates directories with random names
// writes folder paths on a line to file
// returns 0 on success
int createRandomDirectories(char *folder, int number, char* fileStr) {
	int i;
	char *randomString;
	char *finalPath;
	// char *currdir = getAbsoluteWorkingDirectory();
	FILE *file = fopen(fileStr, "w");

	if(!isDirectory(folder)) {
		printf("%s is not a directory. Creating directory...\n", folder);
		if(createDir(folder, 0700) != 0) {
			fprintf(stderr, "Error creating directory: %s\n", folder);
			return -1;
		}
	}

	if(!file) return -1;
	for(i = 0; i < number; ++i) {
		randomString = randomURLString(7);
		finalPath = malloc(strlen(folder) + strlen(randomString) + 2);
		finalPath[0] = '\0';
		// (void)strncat(finalPath, currdir, strlen(currdir));
		// (void)strncat(finalPath, "/", 1);
		(void)strncat(finalPath, folder, strlen(folder));
		(void)strncat(finalPath, "/", 1);
		(void)strncat(finalPath, randomString, strlen(randomString));
		finalPath[strlen(folder) + strlen(randomString) + 1] = '\0';

		if(createDir(finalPath, 0700) != 0) {
			fprintf(stderr, "Error creating directory: %s\n", finalPath);
		} else {
			fprintf(file, "%s\n", finalPath);
		}

		free(randomString);
		free(finalPath);
	}
	// free(currdir);
	fclose(file);

	return 0;
}

// void releaseDirectories(char **directories) {
// 	size_t ptrSize = malloc_size(directories);
// 	int arrLength = ptrSize/sizeof(directories[0]);
// 	int i;
// 	for(i = 0; i < arrLength; ++i) {
// 		free(directories[i]);
// 	}

// 	free(directories);
// }

