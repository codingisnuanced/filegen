#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "ftom.h"
#include <malloc/malloc.h>

#define FG_SIZE 1000
#define TESTING 1

char* createMarkdownFile(char *savePath) {
	FILE *mdFile;
	char *mdExt = ".md";
	char *mdFileName = malloc(strlen(savePath) + strlen(mdExt) + 1);
	mdFileName[0] = '\0';
	(void)strncat(mdFileName, savePath, strlen(savePath));
	(void)strncat(mdFileName, mdExt, strlen(mdExt));
	mdFileName[strlen(savePath) + strlen(mdExt)] = '\0';

	mdFile = fopen(mdFileName, "w");

	if(mdFile == NULL) {
		fprintf(stderr, "Error creating markdown file at %s. Returning NULL.\n\n", savePath);
		return NULL;
	}

	fclose(mdFile);

	return mdFileName;
}

// returns 0 on success;
// void markupFiles(FGFile **fileArr, char *folder) {
// 	FILE *mdFile;
// 	FILE *dummy;
// 	FGFile *fgfile;
// 	FGFile *randFile;
// 	char *mdFileName;
// 	char *savePath;
// 	char line[30];
// 	int randNum;
// 	size_t ptrSize = malloc_size(fileArr);
// 	int arrLength = ptrSize/sizeof(fileArr[0]);

// 	for(int i = 0; i < FG_SIZE; ++i) {
// 		fgfile = fileArr[i];

// 		// construct final save path
// 		if(folder[strlen(folder)] == '/') {
// 			savePath = malloc(strlen(folder) + strlen(fgfile->legalName) + 1);
// 			savePath[0] = '\0';
// 			(void)strncat(savePath, folder, strlen(folder));
// 			(void)strncat(savePath, fgfile->legalName, strlen(fgfile->legalName));
// 			savePath[strlen(folder) + strlen(fgfile->legalName)] = '\0';
// 		} else {
// 			savePath = malloc(strlen(folder) + strlen(fgfile->legalName) + 2);
// 			savePath[0] = '\0';
// 			(void)strncat(savePath, folder, strlen(folder));
// 			(void)strncat(savePath, "/", 1);
// 			(void)strncat(savePath, fgfile->legalName, strlen(fgfile->legalName));
// 			savePath[strlen(folder) + strlen(fgfile->legalName) + 1] = '\0';
// 		}

// 		// write to file
// 		if((mdFileName = createMarkdownFile(savePath)) != NULL) {
// 			mdFile = fopen(mdFileName, "w");
// 			randFile = fileArr[arc4random_uniform(arrLength)];

// 			fprintf(mdFile, "#%s\n", fgfile->fileName);
// 			fprintf(mdFile, "*__%s__*\n\n", fgfile->legalName);


// 			dummy = fopen("dummy.txt", "r");
// 			if(dummy) {
// 				while(fgets(line, sizeof(line), dummy)) {
// 					randNum = arc4random_uniform(arrLength);
// 					randFile = fileArr[arc4random_uniform(arrLength)];
// 					if(randNum < arrLength/2) {
// 						fprintf(mdFile, "%s [%s](http://odetosound.com/%s) ", line, randFile->fileName, randFile->legalName);
// 					} else {
// 						fprintf(mdFile, "%s", line);
// 					}
// 				}
// 			} else {
// 				fprintf(stderr, "Error. Cannot open dummy.txt\n");
// 			}

// 			free(mdFileName);
// 			fclose(dummy);
// 			fclose(mdFile);
// 		}

// 		free(savePath);
// 	}
// }

// fgfile: generated file
// fileArr: generated files array
// folder: directory to save file
// return: final save path of mardown file
char* markupFile(FGFile *fgfile, FGFile **fileArr, char *folder) {
	FILE *mdFile;
	FILE *dummy;
	FGFile *randFile;
	char *mdFileName;
	char *savePath;
	char line[30];
	int randNum;
	size_t ptrSize = malloc_size(fileArr);
	int arrLength = ptrSize/sizeof(fileArr[0]);

	// construct final save path
	if(folder[strlen(folder)] == '/') {
		savePath = malloc(strlen(folder) + strlen(fgfile->legalName) + 1);
		savePath[0] = '\0';
		(void)strncat(savePath, folder, strlen(folder));
		(void)strncat(savePath, fgfile->legalName, strlen(fgfile->legalName));
		savePath[strlen(folder) + strlen(fgfile->legalName)] = '\0';
	} else {
		savePath = malloc(strlen(folder) + strlen(fgfile->legalName) + 2);
		savePath[0] = '\0';
		(void)strncat(savePath, folder, strlen(folder));
		(void)strncat(savePath, "/", 1);
		(void)strncat(savePath, fgfile->legalName, strlen(fgfile->legalName));
		savePath[strlen(folder) + strlen(fgfile->legalName) + 1] = '\0';
	}

	mdFileName = createMarkdownFile(savePath);

	if(TESTING) printf("Saving markdown at %s\n", mdFileName);

	free(savePath);



	if((mdFileName != NULL) && ((mdFile = fopen(mdFileName, "w")) != NULL)) {

		// randFile = fileArr[arc4random_uniform(FG_SIZE)];

		fprintf(mdFile, "#%s\n", fgfile->fileName);
		fprintf(mdFile, "*__%s__*\n\n", fgfile->legalName);


		dummy = fopen("dummy.txt", "r");
		if(dummy) {
			while(fgets(line, sizeof(line), dummy)) {
				randNum = arc4random_uniform(arrLength);
				randFile = fileArr[arc4random_uniform(arrLength)];
				if(randNum < arrLength/2) {
					fprintf(mdFile, "%s [%s](%s) ", line, randFile->fileName, randFile->resourcePath);
				} else {
					fprintf(mdFile, "%s", line);
				}
			}
		} else {
			fprintf(stderr, "Error. Cannot open dummy.txt\n");
		}

		fclose(dummy);
		fclose(mdFile);

		return mdFileName;
	} else {
		if(TESTING) printf("Error saving markdown at %s\n", mdFileName);
		return NULL;
	}

	
}

// void organizeWithDirectoriesArray() {

// }

// returns 0 on success
int organizeWithDirectoriesFile(FGFile **fileArr, int per, char *dirListStr, char *record) {
	size_t ptrSize = malloc_size(fileArr);
	int arrLength = ptrSize/sizeof(fileArr[0]);
	long path_max;
	size_t path_size;
	FILE *dirListFile;
	FILE *recordFile;
	char *markdownPath;

	remove(record); // delete files list if present

	dirListFile = fopen(dirListStr, "r");
	recordFile = fopen(record, "a");

	if(!dirListFile) return -1;
	if(!recordFile) return -1;


	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	char line[path_size];

	int i;
	int lengthTracker = arrLength;
	int positionTracker = 0;
	while(fgets(line, sizeof(line), dirListFile)) {
		strtok(line, "\n"); // remove newline character;
		if(lengthTracker < per) {
			for(i = 0; i < lengthTracker; ++i) {
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((markdownPath = markupFile(fileArr[positionTracker + i], fileArr, line)) == NULL) {
					fprintf(stderr, "Error creating markdown file.\n");
				} else {
					fprintf(recordFile, "%s\n", markdownPath);
					free(markdownPath);
				}
			}
			positionTracker += lengthTracker;
			lengthTracker = 0;
			break;
		} else {
			for(i = 0; i < per; ++i) {
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((markdownPath = markupFile(fileArr[positionTracker + i], fileArr, line)) == NULL) {
					fprintf(stderr, "Error creating markdown file.\n");
				} else {
					fprintf(recordFile, "%s\n", markdownPath);
					free(markdownPath);
				}
			}
			lengthTracker-=per;
			positionTracker += per;

		}
	}

	if(lengthTracker != 0) { // if there are still files left
		for(i = 0; i < lengthTracker; ++i) {
			if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
			if((markdownPath = markupFile(fileArr[positionTracker + i], fileArr, line)) == NULL) {
				fprintf(stderr, "Error creating markdown file.\n");
			} else {
				fprintf(recordFile, "%s\n", markdownPath);
				free(markdownPath);
			}
		}
		positionTracker += lengthTracker;
		lengthTracker = 0;
	}

	fclose(dirListFile);
	fclose(recordFile);

	return 0;
}

int main(int argc, char *argv[]) {
	char option;
	int verbose = TESTING;
	int randomize = TESTING;
	FGFile **fileArr = malloc(FG_SIZE * sizeof(FGFile *));

	while((option = getopt(argc, argv, "vr")) != EOF) {
		switch(option) {
			case 'v':
				verbose = 1;
				break;

			case 'r':
				randomize = 1;
				break;

			default:
				fprintf(stderr, "Unknown option: %s. Terminating.\n", optarg);
				return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if(argv[0] != NULL) {
		if(!initFilesInDir(fileArr, FG_SIZE, argv[0])) return EXIT_FAILURE;
	} else {
		if(!initFilesInDir(fileArr, FG_SIZE, "audio")) return EXIT_FAILURE;
	}

	if(verbose) {
		printf("PRINTING FILES...\n");
		printf("\n");
		printFilesForward(fileArr);
	}

	if(randomize) {
		if(verbose) printf("RANDOMIZING FILES...\n");
		randomizeFiles(fileArr);
		if(verbose) {
			printf("PRINTING FILES...\n");
			printf("\n");
			printFilesForward(fileArr);
		}
	}

	// if(createDir("markdown", 0700) != 0) {
	// 	fprintf(stderr, "Error creating directory.\n");
	// 	return EXIT_FAILURE;
	// }

	// markupFiles(fileArr, "markdown");

	// char **dirs = createRandomDirectories("path", 10);
	// for(int i = 0; i < 10; ++i) {
	// 	printf("DIR: %s\n", dirs[i]);
	// }
	// releaseDirectories(dirs);

	if(createRandomDirectories("markdown", 100, "md_dirs.txt") == 0) {
		organizeWithDirectoriesFile(fileArr, 10, "md_dirs.txt", "md_files.txt");
	}

	releaseFiles(fileArr);
	free(fileArr);
	
	return 0;
}


