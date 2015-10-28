#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <regex.h>

char* createHtmlFile(char *savePath) {
	FILE *htmlFile;
	char *htmlExt = ".html";
	char *htmlFileName = malloc(strlen(savePath) + strlen(htmlExt) + 1);
	htmlFileName[0] = '\0';
	(void)strncat(htmlFileName, savePath, strlen(savePath));
	(void)strncat(htmlFileName, htmlExt, strlen(htmlExt));
	htmlFileName[strlen(savePath) + strlen(htmlExt)] = '\0';

	htmlFile = fopen(htmlFileName, "w");

	if(htmlFile == NULL) {
		fprintf(stderr, "Error creating HTML file at %s. Returning NULL.\n\n", savePath);
		return NULL;
	}

	fclose(htmlFile);

	return htmlFileName;
}

int parseMarkdownFile(char *path) {
	char line[40];
	FILE *markdownFile;
	char msgbuf[100];
	int reti;
	char *source = "[A link, this is](http://example.com)";
	char *headerTagPattern = "(#)([[:alnum:]_]+)";
	char *italicsPattern = "(*)([[:alnum:]_]+)(*)";
	char *boldPattern = "(**)([[:alnum:]_]+)(**)";
	char *boldItalicsPattern = "(*__)([[:alnum:]_]+)(__*)";
	char *boldItalicsPattern2 = "(**_)([[:alnum:]_]+)(_**)";
	char *linkPattern = "([.+)([[:alnum:]_]+)";
	size_t nmatch = 6;

	regex_t regex;
	regmatch_t groupArray[nmatch];

	reti = regcomp(&regex, linkPattern, REG_EXTENDED);
	if(reti) {
		regfree(&regex);
		fprintf(stderr, "Could not compile regex.\n");
		return -1;
	}

	// markdownFile = fopen(path, "r");
	// if(!markdownFile) {
	// 	fprintf(stderr, "Could not open markdown file.\n");
	// 	return -1;
	// }

	reti = regexec(&regex, source, nmatch, groupArray, 0);

	if(!reti) {
		printf("MATCH!\n");
		for(int i = 0; i < nmatch; ++i) {
			printf("%llu\n", groupArray[i].rm_so);
			printf("%s\n", &source[groupArray[i].rm_so]);
		}
	} else {
		printf("NO MATCH!\n");
	}

	regfree(&regex);

	return 0;
	// while(fgets(line, sizeof(line), markdownFile) != NULL) {

	// }
}

int organizeWithDirectoriesFile() {
	return 0;
}

char* createHeaderTag(char *markdown, int verbose) {
	char beginTag[5];
	char endTag[7];
	char *tag;
	char *tagContent;
	char headerChar;
	int textSize;
	int headerNum;

	beginTag[0] = '<';
	beginTag[1] = 'h';
	beginTag[3] = '>';
	beginTag[4] = '\0';

	endTag[0] = '<';
	endTag[1] = '/';
	endTag[2] = 'h';
	endTag[4] = '>';
	endTag[5] = '\n';
	endTag[6] = '\0';

	if(markdown[0] != '#') return NULL;

	if(markdown[0] == '#' && markdown[1] == '#' && markdown[2] == '#' && markdown[3] == '#' && markdown[4] == '#' && markdown[5] == '#') {
		headerNum = 6;
		headerChar = '6';

	}else if(markdown[0] == '#' && markdown[1] == '#' && markdown[2] == '#' && markdown[3] == '#' && markdown[4] == '#') {
		headerNum = 5;
		headerChar = '5';

	} else if(markdown[0] == '#' && markdown[1] == '#' && markdown[2] == '#' && markdown[3] == '#') {
		headerNum = 4;
		headerChar = '4';

	} else if(markdown[0] == '#' && markdown[1] == '#' && markdown[2] == '#') {
		headerNum = 3;
		headerChar = '3';

	} else if(markdown[0] == '#' && markdown[1] == '#') {
		headerNum = 2;
		headerChar = '2';

	}else {
		headerNum = 1;
		headerChar = '1';

	}

	beginTag[2] = headerChar;
	endTag[3] = headerChar;
	textSize = strlen(markdown) - headerNum;

	if((tag = malloc(strlen(beginTag) + textSize + strlen(endTag) + 1)) != NULL && (tagContent = malloc(textSize + 1)) != NULL) {
		memcpy(tagContent, markdown+headerNum, textSize);
		tagContent[textSize] = '\0';

		tag[0] = '\0';

		(void)strncat(tag, beginTag, strlen(beginTag));
		(void)strncat(tag, tagContent, strlen(tagContent));
		(void)strncat(tag, endTag, strlen(endTag));

		tag[strlen(beginTag) + textSize + strlen(endTag)] = '\0';

		free(tagContent);

		if(verbose) printf("Creating header tag: %s\n", tag);

		return tag;
	} else {
		fprintf(stderr, "Trouble with allocating memory. Returning NULL.\n");
		return NULL;
	}
}

int main(int argc, char *argv[])
{
	char line[500];
	char *beginHtml = "<!DOCTYPE html>\n<html>\n";
	char *beginHead = "<head>\n";
	char *closeHead = "</head>\n";
	char *beginBody = "<body>\n";
	char *closeHtml = "</body></html>";
	char *closeAnchor = "</a>";
	char *htmlExt = ".html";
	FILE *markdown;
	FILE *htmlFile;
	char *htmlFileString;
	char option;
	int testing = 0;
	int verbose = testing;

	while((option = getopt(argc, argv, "v")) != EOF) {
		if(option == 'v') {
			verbose = 1;
		} else {
			fprintf(stderr, "Unknown option: %s. Terminating.\n", optarg);
		}
	}

	argc -= optind;
	argv += optind;

	// Create a .html file from
	// the input file name.
	// Get last position of '.' and
	// create a substring of characters
	// leading to said position.
	// Append '.html' to file name
	// TODO: Replace all spaces with dashes.

	// const char *ptr = strrchr(argv[0], '.');
	// if(ptr != NULL) {
	// 	int index = ptr - argv[0];
	// 	char htmlNoExt[index+1];
	// 	memcpy(htmlNoExt, argv[0], index);
	// 	htmlNoExt[index] = '\0';

	// 	if((htmlFileString = malloc(strlen(htmlNoExt) + strlen(htmlExt) + 1)) != NULL) {
	// 		if(verbose) printf("Creating file: %s...\n", htmlFileString);
			
	// 		htmlFileString[0] = '\0';

	// 		(void)strncat(htmlFileString, htmlNoExt, strlen(htmlNoExt));
	// 		(void)strncat(htmlFileString, htmlExt, strlen(htmlExt));

	// 		htmlFileString[strlen(htmlNoExt) + strlen(htmlExt)] = '\0';
	// 	} else {
	// 		fprintf(stderr, "Trouble allocating memory. Terminating.");
	// 		return EXIT_FAILURE;
	// 	}
	// } else {
	// 	fprintf(stderr, "Input file must be a text file (.md, .txt, etc). Terminating.\n");
	// 	return EXIT_FAILURE;
	// }
	

	// if (!(markdown = fopen(argv[0], "r"))) {
	// 	fprintf(stderr, "Cannot open file: %s. Terminating.\n", argv[0]);
	// 	return EXIT_FAILURE;
	// }

	// if (!(htmlFile = fopen(htmlFileString, "w"))) {
	// 	fprintf(stderr, "Cannot open file: %s. Terminating.\n", htmlFileString);
	// 	return EXIT_FAILURE;
	// }
	
	// while(fgets(line, sizeof(line), markdown) != NULL) {
	// 	if (line[0] == '#') {
	// 		// not sure why fgets isn't null terminating
	// 		line[strlen(line)-1] = '\0';

	// 		char *tag = createHeaderTag(line, verbose);
	// 		free(tag);

	// 		// int randNum = arc4random_uniform(6)+1;
	// 		// printf("%i\n", randNum);
	// 	}
	// }

	// free(htmlFileString);

	parseMarkdownFile("path");
	
	return 0;
}






