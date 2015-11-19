#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "log.c"
#include "fgfile.c"

#if defined(__APPLE__) && defined(__MACH__)
#include <malloc/malloc.h>
#endif

#ifdef __gnu_linux__
#include <malloc.h>
#endif

// #define FG_SIZE 100
#define TESTING 0



char* createHtmlFile(char *fileName, char *folder) {
	FILE *htmlFile;
	char *htmlExt = ".html";
	char *subpath;
	char *finalPath;
	int inFolder = 0;

	if(folder != NULL) inFolder = strlen(folder) + 1;

	// construct final save path
	finalPath = malloc(inFolder + strlen(fileName) + strlen(htmlExt) + 1);
	finalPath[0] = '\0';
	if(inFolder) (void)strncat(finalPath, folder, strlen(folder));
	if(inFolder) (void)strncat(finalPath, "/", 1);
	(void)strncat(finalPath, fileName, strlen(fileName));
	(void)strncat(finalPath, htmlExt, strlen(htmlExt));
	finalPath[inFolder + strlen(fileName) + strlen(htmlExt)] = '\0';

	Log("Saving html file at %s\n", finalPath);
	htmlFile = fopen(finalPath, "w");

	if(htmlFile == NULL) {
		LogErr("Could not create HTML file at %s. Returning NULL.\n", finalPath);
		return NULL;
	}

	fclose(htmlFile);

	return finalPath;
}

int markupFile(FGFile *fgfile, FGFile **fileArr, char *filePath) {
	FILE *file;
	FILE *dummy;
	FGFile *randFile;

	char line[30];
	char *randUrl;
	int randNum;

	size_t ptrSize;
#if defined(__APPLE__) && defined(__MACH__)
	ptrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	ptrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = ptrSize/sizeof(fileArr[0]);

	char *beginHtml = "<!DOCTYPE html>\n<html>\n";
	char *beginHead = "<head>\n";
	char *closeHead = "</head>\n";
	char *beginBody = "<body>\n";
	char *closeHtml = "</body>\n</html>";
	char *beginAnchor = "<a href=";
	char *closeAnchor = "</a>";
	char *beginParagraph = "<p>";
	char *closeParagraph = "</p>";
	char *beginHeading1 = "<h1>";
	char *closeHeading1 = "</h1>";
	char *beginHeading3 = "<h3>";
	char *closeHeading3 = "</h3>";

	int pTagOpen = 0;

	if((file = fopen(fgfile->filePath, "w")) != NULL) {

		fprintf(file, "%s%s%s%s\n", beginHtml, beginHead, closeHead, beginBody);

		fprintf(file, "%s%s%s\n", beginHeading1, fgfile->fileName, closeHeading1);
		fprintf(file, "%s%s\"/%s\">%s%s%s\n", beginHeading3, beginAnchor, fgfile->resourcePath, fgfile->legalName, closeAnchor, closeHeading3);

		dummy = fopen("dummy.txt", "r");
		if(dummy) {
			while(fgets(line, sizeof(line), dummy)) {
				if(!pTagOpen) {
					fprintf(file, "%s\n", beginParagraph);
					pTagOpen = 1;
				}

				if(strchr(line, '\n') != NULL && line[0] != '\n') {
					fprintf(file, "%s", line);
					if(pTagOpen) {
						fprintf(file, "%s\n", closeParagraph);
						pTagOpen = 0;
					}
				} else if(line[0] != '\n') {
					randNum = rand() / (RAND_MAX / arrLength);
					randFile = fileArr[rand() / (RAND_MAX / arrLength)];

					if(randNum < arrLength / 4) {
						// insert random mp3 links
						fprintf(file, " %s\"/%s\">%s%s %s ", beginAnchor, randFile->resourcePath, randFile->fileName, closeAnchor, line);
					} else if(randNum > (arrLength / 4) && randNum < ((arrLength * 2) / 4)) {
						// insert random broken links
						randUrl = randomURLString(rand() / (RAND_MAX / 20));
						fprintf(file, " %s\"/%s.mp3\">%s%s %s ", beginAnchor, randUrl, randFile->fileName, closeAnchor, line);
						free(randUrl);
					} else if(randNum > ((arrLength * 2) / 4) && randNum < ((arrLength * 3) / 4)) {
						// insert random links to other html pages
						fprintf(file, " %s\"/%s\">%s.html%s %s ", beginAnchor, randFile->filePath, randFile->fileName, closeAnchor, line);
					} else {
						fprintf(file, "%s", line);
					}
				}
			}
			fprintf(file, "%s\n", closeParagraph);
		} else {
			LogErr("Cannot open dummy.txt\n");
		}

		fprintf(file, "%s\n", closeHtml);

		fclose(dummy);
		fclose(file);

		return 0;
	} else {
		LogErr("Error marking up html at %s\n", filePath);
		return -1;
	}
}

// creates index.html with references to all other html pages
// returns 0 on success
int createPortalFromFilesList(char *filesListStr) {
	long path_max;
	size_t path_size;

	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	char line[path_size];

	Log("\nCreating portal at index.html\n");
	char *indexStr = createHtmlFile("index", NULL);

	char *beginHtml = "<!DOCTYPE html>\n<html>\n";
	char *beginHead = "<head>\n";
	char *closeHead = "</head>\n";
	char *beginBody = "<body>\n";
	char *closeHtml = "</body>\n</html>";
	char *beginParagraph = "<p>";
	char *closeParagraph = "</p>";
	char *beginAnchor = "<a href=";
	char *closeAnchor = "</a>";
	char *beginHeading1 = "<h1>";
	char *closeHeading1 = "</h1>";

	FILE *indexFile = fopen(indexStr , "w");
	FILE *filesListFile = fopen(filesListStr, "r");

	if(!indexFile) {
		return -1;
		LogErr("Error index.html\n");
	}
	if(!filesListFile) {
		LogErr("Error files list\n");
		return -1;
	}

	fprintf(indexFile, "%s%s%s%s%sautogen index.html%s\n", beginHtml, beginHead, closeHead, beginBody, beginHeading1, closeHeading1);

	while(fgets(line, sizeof(line), filesListFile)) {
		strtok(line, "\n"); // remove newline character;
		fprintf(indexFile, "%s%s\"%s\">%s%s%s\n", beginParagraph, beginAnchor, line, line, closeAnchor, closeParagraph);
	}

	fprintf(indexFile, "%s\n", closeHtml);

	fclose(filesListFile);
	fclose(indexFile);
	free(indexStr);

	return 0;
}

int createPortalFromArray(FGFile **fileArr) {
	size_t ptrSize;
#if defined(__APPLE__) && defined(__MACH__)
	ptrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	ptrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = ptrSize/sizeof(fileArr[0]);
	long path_max;
	size_t path_size;
	int i;

	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	char line[path_size];

	Log("\nCreating portal at index.html\n");
	char *indexStr = createHtmlFile("index", NULL);

	char *beginHtml = "<!DOCTYPE html>\n<html>\n";
	char *beginHead = "<head>\n";
	char *closeHead = "</head>\n";
	char *beginBody = "<body>\n";
	char *closeHtml = "</body>\n</html>";
	char *beginParagraph = "<p>";
	char *closeParagraph = "</p>";
	char *beginAnchor = "<a href=";
	char *closeAnchor = "</a>";
	char *beginHeading1 = "<h1>";
	char *closeHeading1 = "</h1>";

	FILE *indexFile = fopen(indexStr , "w");

	if(!indexFile) {
		return -1;
		LogErr("Error index.html\n");
	}

	fprintf(indexFile, "%s%s%s%s%sautogen index.html%s\n", beginHtml, beginHead, closeHead, beginBody, beginHeading1, closeHeading1);

	for(i = 0; i < arrLength; ++i) {
		fprintf(indexFile, "%s%s\"%s\">%s%s%s\n", beginParagraph, beginAnchor, line, line, closeAnchor, closeParagraph);
	}
	fprintf(indexFile, "%s\n", closeHtml);

	fclose(indexFile);
	free(indexStr);

	return 0;
}

int saveFilesWithDirectoriesFile(FGFile **fileArr, int per, char *dirListStr, char *filesListStr) {
		size_t ptrSize;
#if defined(__APPLE__) && defined(__MACH__)
	ptrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	ptrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = ptrSize/sizeof(fileArr[0]);
	long path_max;
	size_t path_size;

	FILE *dirListFile;
	FILE *filesListFile;
	char *filePath;

	int i;
	int lengthTracker = arrLength;
	int positionTracker = 0;

	remove(filesListStr); // delete files list if present

	dirListFile = fopen(dirListStr, "r");
	filesListFile = fopen(filesListStr, "w");

	if(!dirListFile) return -1;
	if(!filesListFile) return -1;


	path_max = pathconf(".", _PC_PATH_MAX);

	if(path_max == -1) {
		path_size = 1024;
	} else if(path_max > 10240) {
		path_size = 10240;
	} else {
		path_size = path_max;
	}

	char line[path_size];

	while(fgets(line, sizeof(line), dirListFile)) {
		strtok(line, "\n"); // remove newline character;
		if(lengthTracker < per) {
			for(i = 0; i < lengthTracker; ++i) {
				// Log("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
					LogErr("Error creating html file at: %s\n", fileArr[positionTracker + i]->legalName);
				} else {
					fileArr[positionTracker + i]->filePath = filePath;
					fprintf(filesListFile, "%s\n", filePath);
				}
			}
			positionTracker += lengthTracker;
			lengthTracker = 0;
			break;
		} else {
			for(i = 0; i < per; ++i) {
				// Log("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
					LogErr("Error creating html file at: %s\n", fileArr[positionTracker + i]->legalName);
				} else {
					fileArr[positionTracker + i]->filePath = filePath;
					fprintf(filesListFile, "%s\n", filePath);
				}
			}
			lengthTracker-=per;
			positionTracker += per;

		}
	}

	if(lengthTracker != 0) { // if there are still files left
		for(i = 0; i < lengthTracker; ++i) {
			// Log("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
			if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
				LogErr("Error creating html file.\n");
			} else {
				fileArr[positionTracker + i]->filePath = filePath;
				fprintf(filesListFile, "%s\n", filePath);
			}
		}
		positionTracker += lengthTracker;
		lengthTracker = 0;
	}

	fclose(dirListFile);
	fclose(filesListFile);

	return 0;
}

int organizeWithDirectoriesFile(FGFile **fileArr, int per, char *dirListStr, char *filesListStr) {
		size_t ptrSize;
#if defined(__APPLE__) && defined(__MACH__)
	ptrSize = malloc_size(fileArr);
#endif

#ifdef __gnu_linux__
	ptrSize = malloc_usable_size(fileArr);
#endif
	int arrLength = ptrSize/sizeof(fileArr[0]);
	FILE *dirListFile;
	FILE *filesListFile;
	long path_max;
	size_t path_size;

	if(saveFilesWithDirectoriesFile(fileArr, per, dirListStr, filesListStr) != 0) return -1;

	filesListFile = fopen(filesListStr, "r");

	if(!filesListFile) return -1;

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
	while(fgets(line, sizeof(line), filesListFile)) {
		strtok(line, "\n"); // remove newline character;
		if(lengthTracker < per) {
			for(i = 0; i < lengthTracker; ++i) {
				if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) LogErr("Error marking up html file.\n");
			}
			positionTracker += lengthTracker;
			lengthTracker = 0;
			break;
		} else {
			for(i = 0; i < per; ++i) {
				if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) LogErr("Error marking up html file.\n");
			}
			lengthTracker-=per;
			positionTracker += per;

		}
	}

	if(lengthTracker != 0) { // if there are still files left
		for(i = 0; i < lengthTracker; ++i) {
			if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) LogErr("Error marking up html file.\n");
		}
		positionTracker += lengthTracker;
		lengthTracker = 0;
	}

	fclose(filesListFile);

	createPortalFromFilesList(filesListStr);

	return 0;
}

int main(int argc, char *argv[]) {
	time_t t;
	FGFile **fileArr;
	char option;
	char *directory = "\0";
	int numFiles = 0;
	int numFolders = 0;
	int filesPer = 0;
	int verbose = TESTING;
	int randomize = TESTING;

	while((option = getopt(argc, argv, "vrf:d:n:p:")) != EOF) {
		switch(option) {
			case 'v':
				verbose = 1;
				break;

			case 'r':
				randomize = 1;
				break;

			case 'd':
				directory = optarg;
				break;

			case 'f':
				if(optarg != NULL) {
					numFiles = atoi(optarg);
				} else {
					fprintf(stderr, "-%c needs a parameter", option);
					return EXIT_FAILURE;
				}
				break;

			case 'n':
				if(optarg != NULL) {
					numFolders = atoi(optarg);
				} else {
					fprintf(stderr, "-%c needs a parameter", option);
					return EXIT_FAILURE;
				}
				break;

			case 'p':
				if(optarg != NULL) {
					filesPer = atoi(optarg);
				} else {
					fprintf(stderr, "-%c needs a parameter", option);
					return EXIT_FAILURE;
				}
				break;

			default:
				fprintf(stderr, "Unknown option: -%c. Terminating.\n", option);
				return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;

	if(directory[0] == '\0' || !numFiles || !numFolders || !filesPer) {
		fprintf(stderr, "Need options -dfnp\n");
		return EXIT_FAILURE;
	}

	fileArr = malloc(numFiles * sizeof(FGFile *));

	if(directory != NULL) {
		if(!initFilesInDir(fileArr, numFiles, directory)) return EXIT_FAILURE;
	} else {
		if(!initFilesInDir(fileArr, numFiles, "audio")) return EXIT_FAILURE;
	}

	if(verbose) {
		printf("Printing files...\n");
		printf("\n");
		printFilesForward(fileArr);
	}

	if(randomize) {
		Log("\nRandomizing files...\n");
		if(verbose) printf("Randomizing files...\n");
		randomizeFiles(fileArr);
		if(verbose) {
			printf("Printing files...\n");
			printf("\n");
			printFilesForward(fileArr);
		}
	}

	srand((unsigned) time(&t));

	if(createRandomDirectories("html", numFolders, "html_dirs.txt") == 0) {
		organizeWithDirectoriesFile(fileArr, filesPer, "html_dirs.txt", "html_files.txt");
	}

	releaseFiles(fileArr);
	free(fileArr);

	return 0;
}


