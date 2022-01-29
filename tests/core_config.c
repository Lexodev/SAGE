/**
 * core_config.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Config file functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 December 2020
 */

#include <stdio.h>

#include "/src/sage.h"

UBYTE my_buffer[256];

void main(void)
{
  SAGE_Configuration * my_conf;

  printf("--------------------------------------------------------------------------------\n");
  printf("* SAGE library CORE test (CONFIG) / %s\n", SAGE_GetVersion());
  printf("--------------------------------------------------------------------------------\n");
  if (SAGE_Init(SMOD_NONE)) {
    if (!SAGE_GetParameterFromFile("/data/test.ini", NULL, "loglevel", "ALL", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 1 : loglevel=%s\n", my_buffer);
    if (!SAGE_GetParameterFromFile("/data/test.ini", "VIDEO", "resolution", "640x480", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 2 : [VIDEO] resolution=%s\n", my_buffer);
    if (!SAGE_GetParameterFromFile("/data/test.ini", "PATH", "sounds", NULL, my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 3 : [PATH] sounds=%s\n", my_buffer);
    if (!SAGE_GetParameterFromFile("/data/test.ini", "AUDIO", "sounds", "4", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 4 : [AUDIO] sounds=%s\n", my_buffer);
    if (!SAGE_GetParameterFromFile("/data/test.ini", "NETWORK", "server", NULL, my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 5 : [NETWORK] server=%s\n", my_buffer);
    if (!SAGE_GetParameterFromFile("/data/test.ini", NULL, "debug", "FALSE", my_buffer, 256)) {
      SAGE_DisplayError();
    }
    printf("Test 6 : debug=%s\n", my_buffer);
    if ((my_conf = SAGE_LoadConfigurationFile("/data/test.ini")) == NULL) {
      SAGE_DisplayError();
    } else {
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, "AUDIO", "sounds", "4"));
      printf("Test 1 : [AUDIO] sounds=%s\n", my_buffer);
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, "NETWORK", "server", NULL));
      printf("Test 2 : [NETWORK] server=%s\n", my_buffer);
      strcpy(my_buffer, SAGE_GetParameterValue(my_conf, NULL, "debug", "FALSE"));
      printf("Test 3 : debug=%s\n", my_buffer);
      printf("Test 4 : Update [VIDEO] depth=8\n");
      if (!SAGE_SetParameterValue(my_conf, "VIDEO", "depth", "8")) {
        SAGE_DisplayError();
      }
      printf("Test 5 : Create [NETWORK] port=6200\n");
      if (!SAGE_SetParameterValue(my_conf, "NETWORK", "port", "6200")) {
        SAGE_DisplayError();
      }
      printf("Test 6 : Create [AUDIO] stereo=TRUE\n");
      if (!SAGE_SetParameterValue(my_conf, "AUDIO", "stereo", "TRUE")) {
        SAGE_DisplayError();
      }
      printf("Test 7 : Create name=Lexo\n");
      if (!SAGE_SetParameterValue(my_conf, NULL, "name", "Lexo")) {
        SAGE_DisplayError();
      }
      printf("Test 8 : Remove [AUDIO] sounds\n");
      if (!SAGE_SetParameterValue(my_conf, "AUDIO", "sounds", NULL)) {
        SAGE_DisplayError();
      }
      printf("Test 9 : Remove [NETWORK] server\n");
      if (!SAGE_SetParameterValue(my_conf, "NETWORK", "server", NULL)) {
        SAGE_DisplayError();
      }
      printf("Saving configuration\n");
      if (!SAGE_SaveConfigurationFile(my_conf, "test.cfg", "Config file de test")) {
        SAGE_DisplayError();
      }
    }
    SAGE_ReleaseConfigurationFile(my_conf);
  }
  SAGE_Exit();
  printf("End of test\n");
}
