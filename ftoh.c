#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <malloc/malloc.h>
#include "fgfile.c" // BAD!!!

#define FG_SIZE 1000
#define TESTING 1

char* createHtmlFile(char *fileName, char *folder) {
	FILE *htmlFile;
	char *htmlExt = ".html";
	char *subpath;
	char *finalPath;
	int inFolder = 0;

	char *currdir = getAbsoluteWorkingDirectory();

	if(folder != NULL) inFolder = strlen(folder) + 1;

	// construct final save path
	finalPath = malloc(strlen(currdir) + inFolder + strlen(fileName) + strlen(htmlExt) + 2);
	finalPath[0] = '\0';
	(void)strncat(finalPath, currdir, strlen(currdir));
	(void)strncat(finalPath, "/", 1);
	if(inFolder) (void)strncat(finalPath, folder, strlen(folder));
	if(inFolder) (void)strncat(finalPath, "/", 1);
	(void)strncat(finalPath, fileName, strlen(fileName));
	(void)strncat(finalPath, htmlExt, strlen(htmlExt));
	finalPath[strlen(currdir) + inFolder + strlen(fileName) + strlen(htmlExt) + 1] = '\0';

	free(currdir);

	if(TESTING) printf("Saving html file at %s\n", finalPath);
	htmlFile = fopen(finalPath, "w");

	if(htmlFile == NULL) {
		fprintf(stderr, "Error creating HTML file at %s. Returning NULL.\n\n", finalPath);
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

	size_t ptrSize = malloc_size(fileArr);
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
		fprintf(file, "%s%s\"%s\">%s%s%s\n", beginHeading3, beginAnchor, fgfile->resourcePath, fgfile->legalName, closeAnchor, closeHeading3);

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
					randNum = arc4random_uniform(arrLength);
					randFile = fileArr[arc4random_uniform(arrLength)];

					if(randNum < arrLength / 4) {
						// insert random mp3 links
						fprintf(file, " %s\"%s\">%s%s %s ", beginAnchor, randFile->resourcePath, randFile->fileName, closeAnchor, line);
					} else if(randNum > (arrLength / 4) && randNum < ((arrLength * 2) / 4)) {
						// insert random broken links
						randUrl = randomURLString(arc4random_uniform(20));
						fprintf(file, " %s\"%s.mp3\">%s%s %s ", beginAnchor, randUrl, randFile->fileName, closeAnchor, line);
						free(randUrl);
					} else if(randNum > ((arrLength * 2) / 4) && randNum < ((arrLength * 3) / 4)) {
						// insert random links to other html pages
						fprintf(file, " %s\"%s\">%s.html%s %s ", beginAnchor, randFile->filePath, randFile->fileName, closeAnchor, line);
					} else {
						fprintf(file, "%s", line);
					}
				}
			}
			fprintf(file, "%s\n", closeParagraph);
		} else {
			fprintf(stderr, "Error. Cannot open dummy.txt\n");
		}

		fprintf(file, "%s\n", closeHtml);

		fclose(dummy);
		fclose(file);

		return 0;
	} else {
		fprintf(stderr, "Error marking up html at %s\n", filePath);
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
		printf("Error index.html\n");
	}
	if(!filesListFile) {
		printf("Error files list\n");
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
	size_t ptrSize = malloc_size(fileArr);
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
		printf("Error index.html\n");
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
	size_t ptrSize = malloc_size(fileArr);
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
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
					fprintf(stderr, "Error creating html file.\n");
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
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
					fprintf(stderr, "Error creating html file.\n");
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
			if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
			if((filePath = createHtmlFile(fileArr[positionTracker + i]->legalName, line)) == NULL) {
				fprintf(stderr, "Error creating html file.\n");
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
	size_t ptrSize = malloc_size(fileArr);
	int arrLength = ptrSize/sizeof(fileArr[0]);
	long path_max;
	size_t path_size;
	FILE *dirListFile;
	FILE *filesListFile;

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
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) fprintf(stderr, "Error marking up html file.\n");
			}
			positionTracker += lengthTracker;
			lengthTracker = 0;
			break;
		} else {
			for(i = 0; i < per; ++i) {
				if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
				if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) fprintf(stderr, "Error marking up html file.\n");
			}
			lengthTracker-=per;
			positionTracker += per;

		}
	}

	if(lengthTracker != 0) { // if there are still files left
		for(i = 0; i < lengthTracker; ++i) {
			if(TESTING) printf("%s will go in %s\n", fileArr[positionTracker + i]->legalName, line);
			if(markupFile(fileArr[positionTracker + i], fileArr, line) != 0) fprintf(stderr, "Error marking up html file.\n");
		}
		positionTracker += lengthTracker;
		lengthTracker = 0;
	}

	fclose(filesListFile);

	createPortalFromFilesList(filesListStr);

	return 0;
}

int main(int argc, char *argv[]) {
	FGFile **fileArr = malloc(FG_SIZE * sizeof(FGFile *));

	if(!initFilesInDir(fileArr, FG_SIZE, "audio")) return EXIT_FAILURE;
	randomizeFiles(fileArr);
	if(createRandomDirectories("html", 100, "html_dirs.txt") == 0) {
		organizeWithDirectoriesFile(fileArr, 10, "html_dirs.txt", "html_files.txt");
	}

	releaseFiles(fileArr);
	free(fileArr);

	return 0;
}


