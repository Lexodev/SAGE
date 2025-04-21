/**
 * sage_logger.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Message logger management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#ifndef _SAGE_LOGGER_H_
#define _SAGE_LOGGER_H_

#include <exec/exec.h>

/** Log level constants */
#define SLOG_ALL          0L
#define SLOG_TRACE        1L
#define SLOG_DEBUG        2L
#define SLOG_INFO         3L
#define SLOG_WARNING      4L
#define SLOG_ERROR        5L
#define SLOG_FATAL        6L
#define SLOG_OFF          9L
#define SLOG_APPLICATION  10L     // Unmaskable log level

/** Set the log level */
VOID SAGE_SetLogLevel(LONG);

/** Print an application log message */
VOID SAGE_AppliLog(char *, ...);

/** Print a fatal log message */
VOID SAGE_FatalLog(char *, ...);

/** Print an error log message */
VOID SAGE_ErrorLog(char *, ...);

/** Print a warning log message */
VOID SAGE_WarningLog(char *, ...);

/** Print an info log message */
VOID SAGE_InfoLog(char *, ...);

/** Print a debug log message */
VOID SAGE_DebugLog(char *, ...);

/** Print a trace log message */
VOID SAGE_TraceLog(char *, ...);

/** Tell if the log level is active */
BOOL SAGE_HasLogLevel(LONG);

#endif
