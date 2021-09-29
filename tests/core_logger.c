/**
 * core_logger.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Test logger functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 April 2020
 */

#include "/src/sage.h"

void main(void)
{
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (LOGGER) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    SAGE_AppliLog("Set log level to ALL");
    SAGE_SetLogLevel(SLOG_ALL);
    SAGE_AppliLog("Log all type of level");
    SAGE_AppliLog("This is an application message !");
    SAGE_FatalLog("This is a fatal error message !");
    SAGE_ErrorLog("This is an error message !");
    SAGE_WarningLog("This is a warning message !");
    SAGE_InfoLog("This is an info message, the answer is %d.", 42);
    SAGE_DebugLog("This is a debug message.");
    SAGE_TraceLog("This is a trace message.");
    if (SAGE_HasLogLevel(SLOG_INFO)) {
      SAGE_AppliLog("Info level is active");
    } else {
      SAGE_AppliLog("Info level is not active");
    }
    SAGE_AppliLog("Set log level to WARNING");
    SAGE_SetLogLevel(SLOG_WARNING);
    SAGE_AppliLog("Log an error message");
    SAGE_ErrorLog("This is an error message !");
    if (SAGE_HasLogLevel(SLOG_ERROR)) {
      SAGE_AppliLog("Error level is active");
    } else {
      SAGE_AppliLog("Error level is not active");
    }
    SAGE_AppliLog("Log a debug message");
    SAGE_DebugLog("This is a debug message.");
    if (SAGE_HasLogLevel(SLOG_DEBUG)) {
      SAGE_AppliLog("Debug level is active");
    } else {
      SAGE_AppliLog("Debug level is not active");
    }
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
