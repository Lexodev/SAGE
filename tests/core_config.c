/**
 * core_config.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Config file functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <sage/sage.h>

UBYTE my_buffer[256];

void main(void)
{
  SAGE_Configuration *my_conf;

  SAGE_AppliLog("--------------------------------------------------------------------------------");
  SAGE_AppliLog("* SAGE library CORE test (CONFIG) / %s", SAGE_GetVersion());
  SAGE_AppliLog("--------------------------------------------------------------------------------");
  if (SAGE_Init(SMOD_NONE)) {
    SAGE_AppliLog("*** Read config parameters ***");
    if (!SAGE_GetParameterFromFile("data/test.ini", NULL, "loglevel", "ALL", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 1 : loglevel=%s", my_buffer);
    if (!SAGE_GetParameterFromFile("data/test.ini", "VIDEO", "resolution", "640x480", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 2 : [VIDEO] resolution=%s", my_buffer);
    if (!SAGE_GetParameterFromFile("data/test.ini", "PATH", "sounds", NULL, my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 3 : [PATH] sounds=%s", my_buffer);
    if (!SAGE_GetParameterFromFile("data/test.ini", "AUDIO", "sounds", "4", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 4 : [AUDIO] sounds=%s", my_buffer);
    if (!SAGE_GetParameterFromFile("data/test.ini", "NETWORK", "server", NULL, my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 5 : [NETWORK] server=%s", my_buffer);
    if (!SAGE_GetParameterFromFile("data/test.ini", NULL, "debug", "FALSE", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    SAGE_AppliLog("Test 6 : debug=%s", my_buffer);
    SAGE_AppliLog("*** Load config file ***");
    if ((my_conf = SAGE_LoadConfigurationFile("data/test.ini")) == NULL) {
      SAGE_DisplayError();
    } else {
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, "AUDIO", "sounds", "4"));
      SAGE_AppliLog("Test 1 : [AUDIO] sounds=%s", my_buffer);
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, "NETWORK", "server", NULL));
      SAGE_AppliLog("Test 2 : [NETWORK] server=%s", my_buffer);
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, NULL, "debug", "FALSE"));
      SAGE_AppliLog("Test 3 : debug=%s", my_buffer);
      SAGE_AppliLog("Test 4 : Update [VIDEO] depth=8");
      if (!SAGE_SetParameterValue(my_conf, "VIDEO", "depth", "8")) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Test 5 : Create [NETWORK] port=6200");
      if (!SAGE_SetParameterValue(my_conf, "NETWORK", "port", "6200")) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Test 6 : Create [AUDIO] stereo=TRUE");
      if (!SAGE_SetParameterValue(my_conf, "AUDIO", "stereo", "TRUE")) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Test 7 : Create name=Lexo");
      if (!SAGE_SetParameterValue(my_conf, NULL, "name", "Lexo")) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Test 8 : Remove [AUDIO] sounds");
      if (!SAGE_SetParameterValue(my_conf, "AUDIO", "sounds", NULL)) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Test 9 : Remove [NETWORK] server");
      if (!SAGE_SetParameterValue(my_conf, "NETWORK", "server", NULL)) {
        SAGE_DisplayError();
      }
      SAGE_AppliLog("Saving configuration");
      if (!SAGE_SaveConfigurationFile(my_conf, "conf_test.cfg", "Config file de test")) {
        SAGE_DisplayError();
      }
    }
    SAGE_ReleaseConfigurationFile(my_conf);
  }
  SAGE_Exit();
  SAGE_AppliLog("End of test");
}
