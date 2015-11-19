// jvutils.h; utility functions

#include <sys/stat.h>

//
// NUMBERS
//

/**
 * generate a random number less than the upper bound
 * param upperbound: the upper bound
 * returns a random number less than the upper bound
 **/
int randomNumber(int upperBound);

/**
 * generate a random number between two numbers
 * param lowerbound: the lower bound
 * param upperbound: the upper bound
 * returns a random number between the lower bound and upper bound
 **/
int randomNumberWithUpperAndLower(int lowerBound, int upperBound);


//
// TIME
//

/**
 * generate a time seed by hashing time_t making it portable (http://eternallyconfuzzled.com/arts/jsw_art_rand.aspx)
 * returns a time seed
 **/
unsigned time_seed();


//
// STRINGS
//

/**
 * compare two strings
 * param a: first string for comparison
 * param b: second string for comparision
 * returns <0 if 'a' is less than b, >0 if 'a' is greater than 'b', and 0 is 'a' and 'b' are equal
 **/
int compareStrings(const char *a, const char *b);

/**
 * replace characters in a string with other character
 * param source: the string to work with
 * param chr: the character to replace
 * param replacement: the character to replace with
 **/
void strreplace(char *source, char chr, char replacement);

/**
 * convert all characters in a string to their lowercase couterparts
 * param source: string to convert
 **/
void stringToLowercase(char *source);


//
// FILES & DIRECTORIES
//

/**
 * check if specified path is a directory
 * param path: the path to test
 * returns non-zero on success, 0 on error
 **/
int isDirectory(const char *path);

/**
 * check if specified path is a normal file
 * param path: the path to test
 * returns non-zero on success, 0 on error
 **/
int isRegularFile(const char *path);

/**
 * create a directory
 * param path: the location at which to create the directory
 * param mode: the permission to create the directory with
 * returns 0 on success, non-zero on error
 **/
int createDir(const char *path, mode_t mode);

/**
 * create a random string
 * param length: the number of characters in the string
 * returns the random string
 **/
char* randomURLString(int length);

/**
 * create directories with random names and write their locations to file
 * param path: the path at which to create the directories; if path does not exist, it is created with mode 0700
 * param number: the number of directories to create
 * param fileStr: the name of the file to write the random directories path
 * returns 0 on success, non-zero on error
 **/
int createRandomDirectories(char *path, int number, char* fileStr);

/**
 * get the absolute (non relative) path of current directory
 * returns the absolute path to current directory
 **/
char* getAbsoluteWorkingDirectory();