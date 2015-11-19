// fgfile.c by Joseph Ayo-Vaughan

#include <dirent.h>
#include <stdio.h>
#include "fgfile.h"
#include "jvutils.h"
#include "log.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <malloc/malloc.h>
#endif

#ifdef __gnu_linux__
#include <malloc.h>
#endif

#define FG_TESTING 0

#ifdef __gnu_linux__
char *strdup(const char *source) {
    int size = strlen(source) + 1;
    char *duplicate = malloc(size);
    if(duplicate)
    {
        strcpy(duplicate, source);
    }
    return duplicate;
}
#endif

FGFile* createFile(char *fileName, char *resourcePath) {
	FGFile *newFile = (FGFile*)malloc(sizeof(FGFile));
	newFile->fileName = strdup(fileName);
	stringToLowercase(fileName);
	strreplace(fileName, ' ', '-');
	newFile->niceName = strdup(fileName);
	newFile->resourcePath = strdup(resourcePath);
	newFile->relativePath = NULL;
	newFile->absolutePath = NULL;

	return newFile;
}

void randomizeFGFiles(FGFile **fileArray) {
	int randNumber;
	size_t fileArraySize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);
	int i;
	for(i = 0; i < arrayLength; ++i) {
		randNumber = randomNumber(arrayLength);
		FGFile *temp = fileArray[randNumber];
		fileArray[randNumber] = fileArray[i];
		fileArray[i] = temp;
	}
}

void listFGFiles(FGFile **fileArray) {
	FGFile *file;
	size_t fileArraySize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);
	int i;
	for(i = 0; i < arrayLength; ++i) {
		file = fileArray[i];
		printf("FGFile: %s\n", file->niceName);
	}
	printf("\n");
}

void releaseFGFiles(FGFile **fileArray) {
	FGFile *file;
	size_t fileArraySize = 0;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);

	int i;
	for(i = 0; i < arrayLength; ++i) {
		file = fileArray[i];
		if(file) {
			if(file->fileName) free(file->fileName);
			if(file->niceName) free(file->niceName);
			if(file->resourcePath) free(file->resourcePath);
			if(file->relativePath) free(file->relativePath);
			if(file->absolutePath) free(file->absolutePath);
			free(file);
		}
	}
}

int generateFGFilesFromResources(FGFile **fileArray, size_t length, char *folder, int includeHidden) {
	char *currentDirectory;
	int counter = 0;
	int hasSlash = 0;

	currentDirectory = getAbsoluteWorkingDirectory();

	char resourceDirectory[strlen(currentDirectory) + strlen(folder) + 2];

	if(folder[strlen(folder) - 1] == '/') hasSlash = 1;

	resourceDirectory[0] = '\0';
	(void)strncat(resourceDirectory, currentDirectory, strlen(currentDirectory));
	(void)strncat(resourceDirectory, "/", 1);
	(void)strncat(resourceDirectory, folder, strlen(folder));
	resourceDirectory[strlen(currentDirectory) + strlen(folder) + 1] = '\0';

	Log("Using files from %s\n\n", resourceDirectory);

	free(currentDirectory);

	DIR *directory;
	struct dirent *ent;
	if((directory = opendir(resourceDirectory)) != NULL) {
		int i = 0;
		while(((ent = readdir(directory)) != NULL) && (i < length)) {
			if(includeHidden) {
				char fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 2];
				fullPath[0] = '\0';
				(void)strncat(fullPath, folder, strlen(folder));
				if(!hasSlash) (void)strncat(fullPath, "/", 1);
				(void)strncat(fullPath, ent->d_name, strlen(ent->d_name));
				fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 1] = '\0';

				if(isRegularFile(fullPath)) {
					counter++;
					fileArray[i] = createFile(ent->d_name, fullPath);
					Log("Found file at: %s\n", fullPath);
				}
				++i;
			} else {
				if(ent->d_name[0] != '.') {
					char fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 2];
					fullPath[0] = '\0';
					(void)strncat(fullPath, folder, strlen(folder));
					if(!hasSlash) (void)strncat(fullPath, "/", 1);
					(void)strncat(fullPath, ent->d_name, strlen(ent->d_name));
					fullPath[strlen(folder) + strlen(ent->d_name) - hasSlash + 1] = '\0';

					if(isRegularFile(fullPath)) {
						counter++;
						fileArray[i] = createFile(ent->d_name, fullPath);
						Log("Found file at: %s\n", fullPath);
					}
					++i;
				}
			}
		}

		Log("\nFound %i files.\n", counter);
		closedir(directory);
		return 0;
	} else {
		perror("");
		LogErr("\nCould not open directory %s\n", resourceDirectory);
		return 1;
	}
}