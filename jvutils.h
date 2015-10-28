int compareFiles(const void *a, const void *b);
int alphabetLookup(int c);
int compareStrings(const char *a, const char *b);
void replaceSpaces(char *string, char replacement);
void stringToLowercase(char *string);
int isDirectory(const char *path);
int isRegularFile(const char *path);
int createDir(const char *path, mode_t mode);
char* randomURLString(int length);
// char** createRandomDirectories(char *path, int number);
// void releaseDirectories(char **directories);
int createRandomDirectories(char *folder, int number, char* fileStr);
int getFilesWithExtension(char *folder, char *fileStr);
char* getAbsoluteWorkingDirectory();