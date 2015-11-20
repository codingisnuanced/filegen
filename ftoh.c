// ftoh.c by Joseph Ayo-Vaughan

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "ftoh.h"
#include "jvutils.h"
#include "log.h"

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
	char *htmlExtension = ".html";
	char *subpath;
	char *finalPath;
	int inFolder = 0;

	if(folder != NULL) inFolder = strlen(folder) + 1;

	// construct final save path
	finalPath = malloc(inFolder + strlen(fileName) + strlen(htmlExtension) + 1);
	finalPath[0] = '\0';
	if(inFolder) (void)strncat(finalPath, folder, strlen(folder));
	if(inFolder) (void)strncat(finalPath, "/", 1);
	(void)strncat(finalPath, fileName, strlen(fileName));
	(void)strncat(finalPath, htmlExtension, strlen(htmlExtension));
	finalPath[inFolder + strlen(fileName) + strlen(htmlExtension)] = '\0';

	Log("Saving html file at %s\n", finalPath);
	htmlFile = fopen(finalPath, "w");

	if(htmlFile == NULL) {
		LogErr("Could not create HTML file at %s. Returning NULL.\n", finalPath);
		return NULL;
	}

	fclose(htmlFile);

	return finalPath;
}

int markupFile(FGFile *fgfile, FGFile **fileArray) {
	FILE *file;
	FILE *dummy;
	FGFile *randFile;

	char line[30];
	char *randUrl;
	int randNum;

	size_t fileArraySize;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);

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

	int paragraphTagOpen = 0;

	Log("Marking up file %s\n", fgfile->niceName);

	if((file = fopen(fgfile->relativePath, "w")) != NULL) {

		fprintf(file, "%s%s%s%s\n", beginHtml, beginHead, closeHead, beginBody);

		fprintf(file, "%s%s%s\n", beginHeading1, fgfile->fileName, closeHeading1);
		fprintf(file, "%s%s\"/%s\">%s%s%s\n", beginHeading3, beginAnchor, fgfile->resourcePath, fgfile->niceName, closeAnchor, closeHeading3);

		dummy = fopen("dummy.txt", "r");
		if(dummy) {
			while(fgets(line, sizeof(line), dummy)) {
				if(!paragraphTagOpen) {
					fprintf(file, "%s\n", beginParagraph);
					paragraphTagOpen = 1;
				}

				if(strchr(line, '\n') != NULL && line[0] != '\n') {
					fprintf(file, "%s", line);
					if(paragraphTagOpen) {
						fprintf(file, "%s\n", closeParagraph);
						paragraphTagOpen = 0;
					}
				} else if(line[0] != '\n') {
					randNum = randomNumber(arrayLength);
					randFile = fileArray[randomNumber(arrayLength)];

					if(randNum < arrayLength / 4) {
						// insert random mp3 links
						fprintf(file, " %s\"/%s\">%s%s %s ", beginAnchor, randFile->resourcePath, randFile->fileName, closeAnchor, line);
					} else if(randNum > (arrayLength / 4) && randNum < ((arrayLength * 2) / 4)) {
						// insert random broken links
						randUrl = randomURLString(randomNumberWithUpperAndLower(5, 21));
						fprintf(file, " %s\"/%s.mp3\">%s%s %s ", beginAnchor, randUrl, randFile->fileName, closeAnchor, line);
						free(randUrl);
					} else if(randNum > ((arrayLength * 2) / 4) && randNum < ((arrayLength * 3) / 4)) {
						// insert random links to other html pages
						fprintf(file, " %s\"/%s\">%s.html%s %s ", beginAnchor, randFile->relativePath, randFile->fileName, closeAnchor, line);
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
		LogErr("Error marking up html at %s\n", fgfile->relativePath);
		return -1;
	}
}

int createPortalFromFilesList(char *filesListString) {
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
	char *indexString = createHtmlFile("index", NULL);

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

	FILE *indexFile = fopen(indexString , "w");
	FILE *filesListFile = fopen(filesListString, "r");

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
	free(indexString);

	return 0;
}

int saveFilesWithDirectoriesFile(FGFile **fileArray, int per, char *directoriesListString, char *filesListString) {
		size_t fileArraySize;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);
	long path_max;
	size_t path_size;

	FILE *directoriesListFile;
	FILE *filesListFile;
	char *filePath;

	int i;
	int lengthTracker = arrayLength;
	int positionTracker = 0;

	remove(filesListString); // delete files list if present

	directoriesListFile = fopen(directoriesListString, "r");
	filesListFile = fopen(filesListString, "w");

	if(!directoriesListFile) return -1;
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

	while(fgets(line, sizeof(line), directoriesListFile)) {
		strtok(line, "\n"); // remove newline character;
		if(lengthTracker < per) {
			for(i = 0; i < lengthTracker; ++i) {
				// Log("%s will go in %s\n", fileArray[positionTracker + i]->niceName, line);
				if((filePath = createHtmlFile(fileArray[positionTracker + i]->niceName, line)) == NULL) {
					LogErr("Error creating html file at: %s\n", fileArray[positionTracker + i]->niceName);
				} else {
					fileArray[positionTracker + i]->relativePath = filePath;
					fprintf(filesListFile, "%s\n", filePath);
				}
			}
			positionTracker += lengthTracker;
			lengthTracker = 0;
			break;
		} else {
			for(i = 0; i < per; ++i) {
				// Log("%s will go in %s\n", fileArray[positionTracker + i]->niceName, line);
				if((filePath = createHtmlFile(fileArray[positionTracker + i]->niceName, line)) == NULL) {
					LogErr("Error creating html file at: %s\n", fileArray[positionTracker + i]->niceName);
				} else {
					fileArray[positionTracker + i]->relativePath = filePath;
					fprintf(filesListFile, "%s\n", filePath);
				}
			}
			lengthTracker-=per;
			positionTracker += per;

		}
	}

	if(lengthTracker != 0) { // if there are still files left
		for(i = 0; i < lengthTracker; ++i) {
			// Log("%s will go in %s\n", fileArray[positionTracker + i]->niceName, line);
			if((filePath = createHtmlFile(fileArray[positionTracker + i]->niceName, line)) == NULL) {
				LogErr("Error creating html file.\n");
			} else {
				fileArray[positionTracker + i]->relativePath = filePath;
				fprintf(filesListFile, "%s\n", filePath);
			}
		}
		positionTracker += lengthTracker;
		lengthTracker = 0;
	}

	fclose(directoriesListFile);
	fclose(filesListFile);

	return 0;
}

int organizeWithDirectoriesFile(FGFile **fileArray, int per, char *directoriesListString, char *filesListString) {
		size_t fileArraySize;
#if defined(__APPLE__) && defined(__MACH__)
	fileArraySize = malloc_size(fileArray);
#endif

#ifdef __gnu_linux__
	fileArraySize = malloc_usable_size(fileArray);
#endif
	int arrayLength = fileArraySize/sizeof(fileArray[0]);
	FILE *directoriesListFile;

	if(saveFilesWithDirectoriesFile(fileArray, per, directoriesListString, filesListString) != 0) return -1;

	int i;
	for(i = 0; i < arrayLength; ++i) {
		if(markupFile(fileArray[i], fileArray) != 0) LogErr("Error marking up html file.\n");
	}

	createPortalFromFilesList(filesListString);

	return 0;
}

int main(int argc, char *argv[]) {
	time_t t;
	FGFile **fileArray;
	char option;
	char *directory = "\0";
	int numberOfFiles = 0;
	int numberOfFolders = 0;
	int filesPerFolder = 0;
	int verbose = TESTING;
	int randomize = TESTING;

	srand(time_seed());

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
					numberOfFiles = atoi(optarg);
				} else {
					fprintf(stderr, "-%c needs a parameter", option);
					return EXIT_FAILURE;
				}
				break;

			case 'n':
				if(optarg != NULL) {
					numberOfFolders = atoi(optarg);
				} else {
					fprintf(stderr, "-%c needs a parameter", option);
					return EXIT_FAILURE;
				}
				break;

			case 'p':
				if(optarg != NULL) {
					filesPerFolder = atoi(optarg);
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

	if(directory[0] == '\0' || !numberOfFiles || !numberOfFolders || !filesPerFolder) {
		fprintf(stderr, "Need options -dfnp\n");
		return EXIT_FAILURE;
	}

	fileArray = malloc(numberOfFiles * sizeof(FGFile *));

	if(directory != NULL) {
		if(generateFGFilesFromResources(fileArray, numberOfFiles, directory, 0) != 0) return EXIT_FAILURE;
	} else {
		if(generateFGFilesFromResources(fileArray, numberOfFiles, "audio", 0) != 0) return EXIT_FAILURE;
	}

	if(verbose) {
		printf("Printing files...\n");
		printf("\n");
		listFGFiles(fileArray);
	}

	if(randomize) {
		Log("\nRandomizing files...\n");
		if(verbose) printf("Randomizing files...\n");
		randomizeFGFiles(fileArray);
		if(verbose) {
			printf("Printing files...\n");
			printf("\n");
			listFGFiles(fileArray);
		}
	}

	if(createRandomDirectories("html", numberOfFolders, "html_dirs.txt") == 0) {
		organizeWithDirectoriesFile(fileArray, filesPerFolder, "html_dirs.txt", "html_files.txt");
	}

	releaseFGFiles(fileArray);
	free(fileArray);

	return 0;
}


