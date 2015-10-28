#include "fgfile.c" // BAD!!!

char* createMarkdownFile(char *str);
void markupFiles(FGFile **ptr, char *folder);
char* markupFile(FGFile *fgfile, FGFile **fileArr, char *folder);
int organizeWithDirectoriesFile(FGFile **fileArr, int per, char *dirListStr, char *record);