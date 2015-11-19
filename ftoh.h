// ftoh.h by Joseph Ayo-Vaughan; files to HTML generator

#include "fgfile.h"

/**
 ** create a .html file
 ** param fileName: name of the html file minus ".html"
 ** param folder: directory to store html file. set to NULL to save in current directory
 ** returns non-absolute save location of file
 **/
char* createHtmlFile(char *fileName, char *folder);

/**
 ** markup file with html
 ** param fgfile: FGFile to write html markup in
 ** param fileArr: FGFile array to use for generated anchor tags
 ** returns 0 on success, non-zero on failure
 **/
int markupFile(FGFile *fgfile, FGFile **fileArr);

/**
 ** create an html file with links to all generated html files
 ** param filesListStr: a file with a list of generated html files save locations
 ** returns 0 on success, non-zero on failure
 **/
int createPortalFromFilesList(char *filesListStr);

/**
 ** create, save html files to directories given and write save locations to a file
 ** markup files when done
 ** param fileArr: an array of FGFiles
 ** param per: number of files to save in each directory
 ** param dirListStr: name of the file containing a directories list
 ** param filesListStr: name of the file to write save locations of the generated html files
 ** returns 0 on success, non-zero on failure
 **/
int organizeWithDirectoriesFile(FGFile **fileArr, int per, char *dirListStr, char *filesListStr);