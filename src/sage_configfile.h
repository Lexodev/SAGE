/**
 * sage_configfile.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Configuration file management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 January 2022
 */

#ifndef _SAGE_CONFIGFILE_H_
#define _SAGE_CONFIGFILE_H_

#include <exec/types.h>

#define SCFG_BUFFER_SIZE      1024

typedef struct {
  STRPTR param_name;
  STRPTR param_value;
  APTR next_param;
} SAGE_ConfParameter;

typedef struct {
  STRPTR section_name;
  SAGE_ConfParameter * parameters;
  APTR next_section;
} SAGE_ConfSection;

typedef struct {
  SAGE_ConfParameter * parameters;
  SAGE_ConfSection * sections;
} SAGE_Configuration;

/** Get a parameter value from a config file */
BOOL SAGE_GetParameterFromFile(STRPTR, STRPTR, STRPTR, STRPTR, STRPTR, LONG);

/** Load a configuration file */
SAGE_Configuration * SAGE_LoadConfigurationFile(STRPTR);

/** Save a configuration file */
BOOL SAGE_SaveConfigurationFile(SAGE_Configuration *, STRPTR, STRPTR);

/** Release a configuration file */
VOID SAGE_ReleaseConfigurationFile(SAGE_Configuration *);

/** Get a parameter value from a config */
STRPTR SAGE_GetParameterValue(SAGE_Configuration *, STRPTR, STRPTR, STRPTR);

/** Set a parameter value to a config */
BOOL SAGE_SetParameterValue(SAGE_Configuration *, STRPTR, STRPTR, STRPTR);

#endif
