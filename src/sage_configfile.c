/**
 * sage_configfile.h
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Configuration file management
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 24/02/2025)
 */

#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <clib/dos_protos.h>

#include <sage/sage_debug.h>
#include <sage/sage_logger.h>
#include <sage/sage_error.h>
#include <sage/sage_memory.h>
#include <sage/sage_configfile.h>

UBYTE line_buffer[SCFG_BUFFER_SIZE];
STRPTR empty_string = "";

/******************************************************************************/

/**
 * Dump a config structure (debug)
 */
VOID SAGE_DumpConfiguration(SAGE_Configuration *config)
{
  SAGE_ConfSection *section;
  SAGE_ConfParameter *parameter;
  
  SAGE_DebugLog("** Configuration dump");
  if (config->parameters != NULL) {
    parameter = config->parameters;
    while (parameter != NULL) {
      SAGE_DebugLog("%s=%s", parameter->param_name, parameter->param_value);
      parameter = (SAGE_ConfParameter *)parameter->next_param;
    }
  }
  if (config->sections != NULL) {
    section = config->sections;
    while (section != NULL) {
      SAGE_DebugLog("[%s]", section->section_name);
      if (section->parameters != NULL) {
        parameter = section->parameters;
        while (parameter != NULL) {
          SAGE_DebugLog("%s=%s", parameter->param_name, parameter->param_value);
          parameter = (SAGE_ConfParameter *)parameter->next_param;
        }
      }
      section = (SAGE_ConfSection *)section->next_section;
    }
  }
}

/******************************************************************************/

/**
 * Find a section in config file
 *
 * @param section Section name
 * @param fdesc   File descriptor
 *
 * @return Operation success
 */
BOOL SAGE_FindSection(STRPTR section, BPTR fdesc)
{
  UWORD index;

  while (FGets(fdesc, line_buffer, SCFG_BUFFER_SIZE)) {
    if (line_buffer[0] == '[') {
      index = 0;
      while (line_buffer[index] != ']' && line_buffer[index] != '\0') {
        index++;
      }
      if (line_buffer[index] == ']') {
        if (strncmp(line_buffer+1, section, index-1) == 0) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Find a parameter in config file
 *
 * @param parameter Parameter name
 * @param buffer    Buffer for parameter
 * @param size      Buffer size
 * @param fdesc     File descriptor
 *
 * @return Operation success
 */
BOOL SAGE_FindParameter(STRPTR parameter, STRPTR buffer, ULONG size, BPTR fdesc)
{
  UWORD index, debut;

  while (FGets(fdesc, line_buffer, SCFG_BUFFER_SIZE)) {
    // New section, param is not here
    if (line_buffer[0] == '[') {
      return FALSE;
    }
    // Line is not a comment
    if (line_buffer[0] != ';') {
      index = 0;
      while (line_buffer[index] != '=' && line_buffer[index] != '\0') {
        index++;
      }
      if (line_buffer[index] == '=') {
        // Param has been found
        if (strncmp(line_buffer, parameter, index) == 0) {
          debut = ++index;
          while (line_buffer[index] != '\n' && line_buffer[index] != '\0') {
            index++;
          }
          line_buffer[index] = '\0';
          if (strlen(line_buffer+debut) > size) {
            strncpy(buffer, line_buffer+debut, size-1);
            buffer[size-1] = '\0';
          } else {
            strcpy(buffer, line_buffer+debut);
          }
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Get a parameter value from a config file
 *
 * @param filename    Config file name
 * @param section     Section name or NULL
 * @param parameter   Parameter name
 * @param defaut      Default value or NULL
 * @param buffer      Buffer for parameter
 * @param buffer_size Buffer size
 *
 * @return Operation success
 */
BOOL SAGE_GetParameterFromFile(STRPTR filename, STRPTR section, STRPTR parameter, STRPTR defaut, STRPTR buffer, LONG buffer_size)
{
  BPTR fdesc;

  if (filename == NULL || parameter == NULL || buffer == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (strlen(defaut) >= buffer_size) {
    SAGE_SetError(SERR_BUFFERSIZE);
    return FALSE;
  }
  if (defaut != NULL) {
    strcpy(buffer, defaut);
  } else {
    buffer[0] = '\0';
  }
  fdesc = Open(filename, MODE_OLDFILE);
  if (!fdesc) {
    SAGE_SetError(SERR_FILENOTFOUND);
    return FALSE;
  }
  if (section != NULL) {
    if (!SAGE_FindSection(section, fdesc)) {
      Close(fdesc);
      SAGE_SetError(SERR_NOSECTION);
      return FALSE;
    }
  }
  if (SAGE_FindParameter(parameter, buffer, buffer_size, fdesc)) {
    Close(fdesc);
    return TRUE;
  }
  Close(fdesc);
  SAGE_SetError(SERR_NOPARAMETER);
  return FALSE;
}

/**
 * Create a new parameter
 */
SAGE_ConfParameter *SAGE_CreateParameter(STRPTR name, STRPTR value)
{
  SAGE_ConfParameter *parameter;
  
  parameter = SAGE_AllocMem(sizeof(SAGE_ConfParameter));
  if (parameter != NULL) {
    parameter->param_name = SAGE_AllocMem(strlen(name)+1);
    parameter->param_value = SAGE_AllocMem(strlen(value)+1);
    if (parameter->param_name != NULL && parameter->param_value != NULL) {
      strcpy(parameter->param_name, name);
      strcpy(parameter->param_value, value);
      parameter->next_param = NULL;
      return parameter;
    }
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Release a parameter
 */
VOID SAGE_ReleaseParameter(SAGE_ConfParameter *parameter)
{
  if (parameter->next_param != NULL) {
    SAGE_ReleaseParameter((SAGE_ConfParameter *)parameter->next_param);
  }
  SAGE_FreeMem(parameter->param_name);
  SAGE_FreeMem(parameter->param_value);
  SAGE_FreeMem(parameter);
}

/**
 * Create a new section
 */
SAGE_ConfSection *SAGE_CreateSection(STRPTR name)
{
  SAGE_ConfSection *section;
  
  section = SAGE_AllocMem(sizeof(SAGE_ConfSection));
  if (section != NULL) {
    section->section_name = SAGE_AllocMem(strlen(name)+1);
    if (section->section_name != NULL) {
      strcpy(section->section_name, name);
      section->next_section = NULL;
      return section;
    }
  }
  SAGE_SetError(SERR_NO_MEMORY);
  return FALSE;
}

/**
 * Release a section
 */
VOID SAGE_ReleaseSection(SAGE_ConfSection *section)
{
  if (section->next_section != NULL) {
    SAGE_ReleaseSection((SAGE_ConfSection *)section->next_section);
  }
  if (section->parameters != NULL) {
    SAGE_ReleaseParameter(section->parameters);
  }
  SAGE_FreeMem(section->section_name);
  SAGE_FreeMem(section);
}

/**
 * Add a section to the config
 */
BOOL SAGE_AddSection(SAGE_Configuration *config, STRPTR name)
{
  SAGE_ConfSection *section, *parent;

  section = SAGE_CreateSection(name);
  if (section != NULL) {
    SD(SAGE_DebugLog("Add a section %s", section->section_name);)
    if (config->sections == NULL) {
      config->sections = section;
    } else {
      parent = config->sections;
      while (parent->next_section != NULL) {
        parent = (SAGE_ConfSection *)parent->next_section;
      }
      parent->next_section = section;
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * Get the section with supplied name
 */
SAGE_ConfSection *SAGE_GetSection(SAGE_Configuration *config, STRPTR name)
{
  SAGE_ConfSection *confsec;

  if (config->sections != NULL) {
    confsec = config->sections;
    while (confsec != NULL) {
      if (strcmp(name, confsec->section_name) == 0) {
        return confsec;
      }
      confsec = confsec->next_section;
    }
  }
  SAGE_SetError(SERR_NOSECTION);
  return NULL;
}

/**
 * Push a parameter to the config or to the last section
 */
BOOL SAGE_PushParameter(SAGE_Configuration *config, STRPTR name, STRPTR value)
{
  SAGE_ConfSection *section;
  SAGE_ConfParameter *parameter, *parent;

  parameter = SAGE_CreateParameter(name, value);
  if (parameter != NULL) {
    SD(SAGE_DebugLog("Push a parameter %s=%s", parameter->param_name, parameter->param_value);)
    if (config->sections == NULL) {
      if (config->parameters == NULL) {
        config->parameters = parameter;
      } else {
        parent = config->parameters;
        while (parent->next_param != NULL) {
          parent = (SAGE_ConfParameter *)parent->next_param;
        }
        parent->next_param = parameter;
      }
    } else {
      section = config->sections;
      while (section->next_section != NULL) {
        section = (SAGE_ConfSection *)section->next_section;
      }
      if (section->parameters == NULL) {
        section->parameters = parameter;
      } else {
        parent = section->parameters;
        while (parent->next_param != NULL) {
          parent = (SAGE_ConfParameter *)parent->next_param;
        }
        parent->next_param = parameter;
      }
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * Get the parameter from the supplied section with supplied name
 *
 * @param config  Config structure
 * @param section Section name or NULL
 * @param name    Parameter name
 *
 * @return Configuration parameter
 */
SAGE_ConfParameter *SAGE_GetParameter(SAGE_Configuration *config, STRPTR section, STRPTR name)
{
  SAGE_ConfSection *confsec;
  SAGE_ConfParameter *confparam;
  
  if (section != NULL) {
    confsec = SAGE_GetSection(config, section);
    if (confsec == NULL) {
      SAGE_SetError(SERR_NOSECTION);
      return NULL;
    }
    confparam = confsec->parameters;
  } else {
    confparam = config->parameters;
  }
  if (confparam != NULL) {
    while (confparam != NULL) {
      if (strcmp(name, confparam->param_name) == 0) {
        return confparam;
      }
      confparam = confparam->next_param;
    }
  }
  SAGE_SetError(SERR_NOPARAMETER);
  return NULL;
}

/**
 * Update/create a parameter in a config
 *
 * @param config  Config structure
 * @param section Section name or NULL
 * @param name    Parameter name
 * @param value   Parameter value or NULL
 *
 * @return Operation success
 */
BOOL SAGE_SetParameter(SAGE_Configuration *config, STRPTR section, STRPTR name, STRPTR value)
{
  SAGE_ConfSection *confsec;
  SAGE_ConfParameter *confparam, *parent;

  confparam = SAGE_GetParameter(config, section, name);
  if (confparam != NULL) {
    // Parameter exists, update it
    SAGE_FreeMem(confparam->param_value);
    confparam->param_value = SAGE_AllocMem(strlen(value)+1);
    if (confparam->param_value != NULL) {
      strcpy(confparam->param_value, value);
      return TRUE;
    }
    SAGE_SetError(SERR_NO_MEMORY);
  } else {
    // Parameter did not exists, create it
    confparam = SAGE_CreateParameter(name, value);
    if (confparam != NULL) {
      if (section == NULL) {
        // Add it to global config
        if (config->parameters == NULL) {
          config->parameters = confparam;
        } else {
          parent = config->parameters;
          while (parent->next_param != NULL) {
            parent = (SAGE_ConfParameter *)parent->next_param;
          }
          parent->next_param = confparam;
        }
      } else {
        // Add it to a section
        confsec = SAGE_GetSection(config, section);
        if (confsec == NULL) {
          // Section did not exists, create it
          if (!SAGE_AddSection(config, section)) {
            return FALSE;
          }
          confsec = SAGE_GetSection(config, section);
        }
        // Add the parameter to the section
        if (confsec->parameters == NULL) {
          confsec->parameters = confparam;
        } else {
          parent = confsec->parameters;
          while (parent->next_param != NULL) {
            parent = (SAGE_ConfParameter *)parent->next_param;
          }
          parent->next_param = confparam;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Remove a parameter from config
 *
 * @param config  Config structure
 * @param section Section name or NULL
 * @param name    Parameter name
 *
 * @return Operation success
 */
BOOL SAGE_RemoveParameter(SAGE_Configuration *config, STRPTR section, STRPTR name)
{
  SAGE_ConfSection *confsec;
  SAGE_ConfParameter *confparam, *parent;

  if (section != NULL) {
    confsec = SAGE_GetSection(config, section);
    if (confsec == NULL) {
      return FALSE;
    }
    parent = confsec->parameters;
  } else {
    parent = config->parameters;
  }
  confparam = SAGE_GetParameter(config, section, name);
  if (confparam == NULL) {
    return FALSE;
  }
  if (confparam == parent) {
    // First of the list
    if (section == NULL) {
      config->parameters = confparam->next_param;
    } else {
      confsec->parameters = confparam->next_param;
    }
  } else {
    // Search the parameter
    while (confparam != parent->next_param) {
      parent = parent->next_param;
    }
    parent->next_param = confparam->next_param;
  }
  confparam->next_param = NULL;
  SAGE_ReleaseParameter(confparam);
  return TRUE;
}

/**
 * Load a configuration file
 *
 * @param filename Config file name
 *
 * @return Configuration file structure
 */
SAGE_Configuration *SAGE_LoadConfigurationFile(STRPTR filename)
{
  BPTR fdesc;
  SAGE_Configuration *config;
  UWORD index, pstart;

  if (filename == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return NULL;
  }
  fdesc = Open(filename, MODE_OLDFILE);
  if (!fdesc) {
    SAGE_SetError(SERR_FILENOTFOUND);
    return NULL;
  }
  config = (SAGE_Configuration *)SAGE_AllocMem(sizeof(SAGE_Configuration));
  if (config == NULL) {
    Close(fdesc);
    SAGE_SetError(SERR_NO_MEMORY);
    return NULL;
  }
  while (FGets(fdesc, line_buffer, SCFG_BUFFER_SIZE)) {
    if (line_buffer[0] != ';' && line_buffer[0] != ' ' && line_buffer[0] != '\t' && line_buffer[0] != '\0') {
      if (line_buffer[0] == '[') {
        index = 0;
        while (line_buffer[index] != ']' && line_buffer[index] != '\0') {
          index++;
        }
        if (line_buffer[index] == ']') {
          line_buffer[index] = '\0';
          SAGE_AddSection(config, line_buffer+1);
        }
      } else {
        index = 0;
        while (line_buffer[index] != '=' && line_buffer[index] != '\0') {
          index++;
        }
        if (line_buffer[index] == '=') {
          line_buffer[index] = '\0';
          pstart = ++index;
          while (line_buffer[index] != '\n' && line_buffer[index] != '\0') {
            index++;
          }
          line_buffer[index] = '\0';
          SAGE_PushParameter(config, line_buffer, line_buffer+pstart);
        }
      }
    }
  }
  Close(fdesc);
  SD(SAGE_DumpConfiguration(config);)
  return config;
}

/**
 * Save a configuration file
 *
 * @param config   Configuration structure
 * @param filename Config file name
 *
 * @return Operation success
 */
BOOL SAGE_SaveConfigurationFile(SAGE_Configuration *config, STRPTR filename, STRPTR header)
{
  BPTR fdesc;
  SAGE_ConfSection *confsec;
  SAGE_ConfParameter *confparam;

  if (filename == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  fdesc = Open(filename, MODE_NEWFILE);
  if (!fdesc) {
    SAGE_SetError(SERR_FILENOTFOUND);
    return FALSE;
  }
  if (header != NULL) {
    sprintf(line_buffer, "; %s\n", header);
    FPuts(fdesc, line_buffer);
  }
  if (config->parameters != NULL) {
    sprintf(line_buffer, "\n");
    FPuts(fdesc, line_buffer);
    confparam = config->parameters;
    while (confparam != NULL) {
      sprintf(line_buffer, "%s=%s\n", confparam->param_name, confparam->param_value);
      FPuts(fdesc, line_buffer);
      confparam = confparam->next_param;
    }
  }
  if (config->sections != NULL) {
    confsec = config->sections;
    while (confsec != NULL) {
      sprintf(line_buffer, "\n[%s]\n", confsec->section_name);
      FPuts(fdesc, line_buffer);
      if (confsec->parameters != NULL) {
        confparam = confsec->parameters;
        while (confparam != NULL) {
          sprintf(line_buffer, "%s=%s\n", confparam->param_name, confparam->param_value);
          FPuts(fdesc, line_buffer);
          confparam = confparam->next_param;
        }
      }
      confsec = confsec->next_section;
    }
  }
  Close(fdesc);
  return TRUE;
}

/**
 * Release a configuration file
 *
 * @param config Configuration structure
 */
VOID SAGE_ReleaseConfigurationFile(SAGE_Configuration *config)
{
  if (config->parameters != NULL) {
    SAGE_ReleaseParameter(config->parameters);
  }
  if (config->sections != NULL) {
    SAGE_ReleaseSection(config->sections);
  }
  SAGE_FreeMem(config);
}

/**
 * Get a parameter value from a config
 *
 * @param config  Config structure
 * @param section Section name or NULL
 * @param name    Parameter name
 * @param defaut  Default value or NULL
 *
 * @return Parameter value
 */
STRPTR SAGE_GetParameterValue(SAGE_Configuration *config, STRPTR section, STRPTR name, STRPTR defaut)
{
  SAGE_ConfParameter *confparam;

  if (config == NULL || name == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  confparam = SAGE_GetParameter(config, section, name);
  if (confparam == NULL) {
    if (defaut != NULL) {
      return defaut;
    }
    return empty_string;
  }
  return confparam->param_value;
}

/**
 * Set a parameter value to a config
 *
 * @param config  Config structure
 * @param section Section name or NULL
 * @param name    Parameter name
 * @param value   Parameter value or NULL
 *
 * @return Operation success
 */
BOOL SAGE_SetParameterValue(SAGE_Configuration *config, STRPTR section, STRPTR name, STRPTR value)
{
  if (config == NULL || name == NULL) {
    SAGE_SetError(SERR_NULL_POINTER);
    return FALSE;
  }
  if (value == NULL) {
    return SAGE_RemoveParameter(config, section, name);
  }
  return SAGE_SetParameter(config, section, name, value);
}
