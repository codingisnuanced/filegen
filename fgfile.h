typedef struct FGFile {
	char *fileName;
	char *legalName;
	char *resourcePath;
	char *filePath;
} FGFile;
FGFile* createFile(char *fileName, char *resourcePath);
void printFilesForward(FGFile **ptr);
void releaseFiles(FGFile **ptr);
void randomizeFiles(FGFile **ptr);
int initFilesInDir(FGFile **fileArr, size_t length, char *folder);