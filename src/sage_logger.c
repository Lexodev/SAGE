/**
 * sage_logger.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Message logger management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 24.2 June 2024 (updated: 27/06/2024)
 */

#include <stdio.h>
#include <stdarg.h>

#include <sage/sage_context.h>
#include <sage/sage_debug.h>
#include <sage/sage_logger.h>

/** @var Log level */
LONG SAGE_LogLevel = SLOG_ALL;

/** SAGE context */
extern SAGE_Context SageContext;

/**
 * Set the log level
 * 
 * @param level Log level
 */
VOID SAGE_SetLogLevel(LONG level)
{
  SAGE_LogLevel = level;
}

/**
 * Display a log message to the console
 *
 * @param prefix Message prefix
 * @param format Message string
 * @param args   Variable list of arguments
 */
VOID SAGE_MessageLog(char *prefix, char *format, va_list args)
{
  printf("[%s] ", prefix);
  vprintf(format, args);
  printf("\n");
}

/**
 * Display an application message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_AppliLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_APPLICATION) {
    SAGE_MessageLog("APP", format, args);
  }
  va_end(args);
}

/**
 * Display a fatal message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_FatalLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_FATAL) {
    SAGE_MessageLog("FTL", format, args);
  }
  va_end(args);
}

/**
 * Display an error message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_ErrorLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_ERROR) {
    SAGE_MessageLog("ERR", format, args);
  }
  va_end(args);
}

/**
 * Display a warning message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_WarningLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_WARNING) {
    SAGE_MessageLog("WRN", format, args);
  }
  va_end(args);
}

/**
 * Display an information message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_InfoLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_INFO) {
    SAGE_MessageLog("INF", format, args);
  }
  va_end(args);
}

/**
 * Display a debug message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_DebugLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_DEBUG) {
    SAGE_MessageLog("DBG", format, args);
  }
  va_end(args);
}

/**
 * Display a trace message
 * 
 * @param format Message string
 * @param ...    Variable list of arguments
 */
VOID SAGE_TraceLog(char *format, ...)
{
  va_list args;

  va_start(args, format);
  if (SAGE_LogLevel <= SLOG_TRACE && SageContext.TraceDebug) {
    SAGE_MessageLog("TRC", format, args);
  }
  va_end(args);
}

/**
 * Tell if the log has the required level
 * 
 * @param LONG Level required
 * 
 * @return Log has the required level
 */
BOOL SAGE_HasLogLevel(LONG level)
{
  if (SAGE_LogLevel <= level) {
    return TRUE;
  }
  return FALSE;
}
