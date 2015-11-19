#include <dirent.h>
#include "fgfile.h"
#include "log.h"
#include "jvutils.c" // BAD!!

#if defined(__APPLE__) && defined(__MACH__)
#include <malloc/malloc.h>
#endif

#ifdef __gnu_linux__
#include <malloc.h>
#endif

#define FG_TESTING 0

#ifdef __gnu_linux__
char *strdup(const char *str) {
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}
#endif

FGFile* createFile(char *fileName, char *resourcePath) {
	FGFile *newFile = (FGFile*)malloc(sizeof(FGFile));
	newFile->fileName = strdup(fileName);
	stringToLowercase(fileName);
	replaceSpaces(fileName, '-');
	newFile->legalName = strdup(fileName);
	newFile->resourcePath = strdup(resourcePath);
	newFile->filePath = NULL;

	return newFile;
}

void randomizeFiles(FGFile **fileArr) {
	int rn;
	size_t fileArrSize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	fileArrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = fileArrSize/sizeof(fileArr[0]);
	int i;
	for(i = 0; i < arrLength; ++i) {
		rn = rand() / (RAND_MAX / arrLength);
		FGFile *temp = fileArr[rn];
		fileArr[rn] = fileArr[i];
		fileArr[i] = temp;
	}
}

void printFilesForward(FGFile **fileArr) {
	FGFile *file;
	size_t fileArrSize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	fileArrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = fileArrSize/sizeof(fileArr[0]);
	int i;
	for(i = 0; i < arrLength; ++i) {
		file = fileArr[i];
		printf("FGFILE: %s\n", file->legalName);
	}
	printf("\n");
}

void releaseFiles(FGFile **fileArr) {
	FGFile *file;
	size_t fileArrSize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	fileArrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = fileArrSize/sizeof(fileArr[0]);
	for(int i = 0; i < arrLength; ++i) {
		file = fileArr[i];
		if(file) {
			if(file->fileName) free(file->fileName);
			if(file->legalName) free(file->legalName);
			if(file->resourcePath) free(file->resourcePath);
			if(file->filePath) free(file->filePath);
			free(file);
		}
	}
}

// returns 0 on success
int initFilesInDir(FGFile **fileArr, size_t length, char *folder) {
	char *buf;
	char *currdir;
	int counter = 0;
	int hasSlash = 0;

	currdir = getAbsoluteWorkingDirectory();

	char audioDir[strlen(currdir) + strlen(folder) + 2];

	if(folder[strlen(folder) - 1] == '/') hasSlash = 1;

	audioDir[0] = '\0';
	(void)strncat(audioDir, currdir, strlen(currdir));
	(void)strncat(audioDir, "/", 1);
	(void)strncat(audioDir, folder, strlen(folder));
	audioDir[strlen(currdir) + strlen(folder) + 1] = '\0';

	Log("Using files from %s\n\n", audioDir);

	free(currdir);

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(audioDir)) != NULL) {
		int i = 0;
		while(((ent = readdir(dir)) != NULL) && (i < length)) {
			// check if item is a file that doesn't
			// begin with a period(.);
			// create FGFile with it's resource path
			// put FGFile objects into array
			if(ent->d_name[0] != '.') {
				// construct full path of file
				char fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 2];
				fullPath[0] = '\0';
				(void)strncat(fullPath, folder, strlen(folder));
				if(!hasSlash) (void)strncat(fullPath, "/", 1);
				(void)strncat(fullPath, ent->d_name, strlen(ent->d_name));
				fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 1] = '\0';

				if(isRegularFile(fullPath)) {
					counter++;
					fileArr[i] = createFile(ent->d_name, fullPath);
					Log("Found file at: %s\n", fullPath);
				}
				++i;
			}
		}

		Log("\nFound %i files.\n", counter);
		closedir(dir);
		return 1;
	} else {
		perror("");
		LogErr("\nCould not open directory %s\n", audioDir);
		return 0;
	}
}