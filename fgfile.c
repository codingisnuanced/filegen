#include <dirent.h>
#include "fgfile.h"
#include "jvutils.c" // BAD!!
#include <malloc/malloc.h>

#define FG_TESTING 0

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

void randomizeFiles(FGFile **ptr) {
	int rn;
	size_t ptrSize = malloc_size(ptr);
	int arrLength = ptrSize/sizeof(ptr[0]);
	int i;
	for(i = 0; i < arrLength; ++i) {
		rn = arc4random_uniform(arrLength);
		FGFile *temp = ptr[rn];
		ptr[rn] = ptr[i];
		ptr[i] = temp;
	}
}

void printFilesForward(FGFile **ptr) {
	FGFile *file;
	int ptrSize = malloc_size(ptr);
	int arrLength = ptrSize/sizeof(ptr[0]);
	int i;
	for(i = 0; i < arrLength; ++i) {
		file = ptr[i];
		printf("FGFILE: %s\n", file->legalName);
	}
	printf("\n");
}

void releaseFiles(FGFile **ptr) {
	FGFile *file;
	int ptrSize = malloc_size(ptr);
	int arrLength = ptrSize/sizeof(ptr[0]);
	for(int i = 0; i < arrLength; ++i) {
		file = ptr[i];
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
	long path_max;
	size_t path_size;
	char *currdir;
	int counter = 0;

	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	// buf = malloc(path_size);

	currdir = getAbsoluteWorkingDirectory();

	char audioDir[strlen(currdir) + strlen(folder) + 2];

	audioDir[0] = '\0';
	(void)strncat(audioDir, currdir, strlen(currdir));
	(void)strncat(audioDir, "/", 1);
	(void)strncat(audioDir, folder, strlen(folder));
	audioDir[strlen(currdir) + strlen(folder) + 1] = '\0';

	if(FG_TESTING) printf("CREATING FOLDER AT: %s\n\n", audioDir);

	free(currdir);

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(audioDir)) != NULL) {
		int i = 0;
		while(((ent = readdir(dir)) != NULL) && (i < length)) {
			// check if item is a file, not .DS_Store, . or ..
			// create FGFile objects and set prev and next nodes
			// put FGFile objects into array
			if(ent->d_name[0] != '.') {
				// construct full path of file
				char fullPath[strlen(audioDir) + strlen(ent->d_name) + 2];
				fullPath[0] = '\0';
				(void)strncat(fullPath, audioDir, strlen(audioDir));
				(void)strncat(fullPath, "/", 1);
				(void)strncat(fullPath, ent->d_name, strlen(ent->d_name));
				fullPath[strlen(audioDir) + 1 + strlen(ent->d_name)] = '\0';

				if(isRegularFile(fullPath)) {
					counter++;
					fileArr[i] = createFile(ent->d_name, fullPath);
					if(FG_TESTING) printf("Found file at: %s\n", fullPath);
				}
				++i;
			}
		}

		if(FG_TESTING) printf("Found %i files.\n", counter);
		closedir(dir);
		return 1;
	} else {
		perror("");
		return 0;
	}
}