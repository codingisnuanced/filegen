// log.h; the header file which defines Log(); and LogErr();

// all Log(); messages will be appended too this file
#define LOGFILE "ftoh.log"

// keeps track of whether the log file is created or not
extern int LogCreated;

// logs a message to LOGFILE
void Log(char *format, ...);

// logs an error message to LOGFILE
void LogErr(char *format, ...);