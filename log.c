// log.c by Joseph Ayo-Vaughan

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

#define TRUE 1
#define FALSE 0

int LogCreated = FALSE;

void Log(char *format, ...) {
	va_list args;
	FILE *file;

	va_start(args, format);

	if(!LogCreated) {
		file = fopen(LOGFILE, "w");
		LogCreated = TRUE;
	} else {
		file = fopen(LOGFILE, "a");
	}

	if(file == NULL) {
		if(LogCreated) {
			LogCreated = FALSE;
			return;
		}
	} else {
		vfprintf(file, format, args);
		fclose(file);
	}

	if(file) fclose(file);
	va_end(args);
}

void LogErr(char *format, ...) {
	va_list args;
	FILE *file;
	char *error;

	error = "ERROR: ";
	char errorMessage[strlen(error) + strlen(format) + 1];
	errorMessage[0] = '\0';
	(void)strncat(errorMessage, error, strlen(error));
	(void)strncat(errorMessage, format, strlen(format));
	errorMessage[strlen(error) + strlen(format)] = '\0';

	va_start(args, format);

	if(!LogCreated) {
		file = fopen(LOGFILE, "w");
		LogCreated = TRUE;
	} else {
		file = fopen(LOGFILE, "a");
	}

	if(file == NULL) {
		if(LogCreated) {
			LogCreated = FALSE;
			return;
		}
	} else {
		vfprintf(file, errorMessage, args);
		fclose(file);
	}

	if(file) fclose(file);
	va_end(args);
}