// log.h by Joseph Ayo-Vaughan; logging utility

// all Log() and LogErr messages will be appended too this file
#define LOGFILE "ftoh.log"

// keeps track of whether the log file is created or not
extern int LogCreated;

// keeps track of the number of errors
extern int LogCount;

/**
 ** logs a message to LOGFILE
 ** param format: format of message
 ** param ...: 
 **/
void Log(char *format, ...);

/**
 ** logs an error message to LOGFILE
 ** param format: format of message
 ** param ...: 
 **/
void LogErr(char *format, ...);