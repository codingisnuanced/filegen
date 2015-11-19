// fgfile.h by Joseph Ayo-Vaughan

#include <stdlib.h>
#include <string.h>

/**
 * Structure which defines a file generated from a resource
 * attribute fileName: the name of the file when scanned
 * attribute niceName: a URI-friendly version of the fileName
 * attribute relativePath: the relative path the generated file
 * attribute absolutePath: the absolute path the generated file
 * attribute resourcePath: the path to the resource generated from
 **/
typedef struct FGFile {
	char *fileName;
	char *niceName;
	char *relativePath;
	char *absolutePath;
	char *resourcePath;
} FGFile;

/**
 * create a filegen file
 * parameter fileName: the name of the filegen file
 * parameter resourcePath: the path to the resource generated from
 * returns the filgen file
 **/
// FGFile* createFile(char *fileName, char *resourcePath);

/**
 * create filegen files from resources
 * parameter fileArray: the filegen array to store filgen files
 * parameter numberOfFiles: number of resources to generate files from
 * parameter folder: folder containing resources to create filegen files from
 * parameter includeHidden: specify whether or not to include dot files
 * returns 0 on success, non-zero on error
 **/
int generateFGFilesFromResources(FGFile **fileArray, size_t numberOfFiles, char *folder, int includeHidden);

/**
 * print out the names of all files in a filegen array
 * parameter fileArray: the filegen array to print file names from
 **/
void listFGFiles(FGFile **fileArray);

/**
 * randomly reorganize filegen files in a filegen array
 * parameter fileArray: the filegen array to randomize
 **/
void randomizeFGFiles(FGFile **fileArray);

/**
 * free memory associated with all files in a FGFile array and then the array
 * parameter fileArray: the file array to free
 **/
void releaseFGFiles(FGFile **ptr);