/**
 * sage_loadobj.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Wavefront object loading
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 25.1 February 2025 (updated: 28/02/2025)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <dos/dos.h>

#include <sage/sage_debug.h>
#include <sage/sage_error.h>
#include <sage/sage_logger.h>
#include <sage/sage_memory.h>
#include <sage/sage_screen.h>
#include <sage/sage_3dtexture.h>
#include <sage/sage_loadobj.h>

#include <proto/dos.h>
#include <proto/exec.h>

/** Buffer for object line */
UBYTE obj_line_buffer[1024];

/** Buffer for line tokens */
WORD obj_nb_tokens;
STRPTR obj_line_token[16];

/*****************************************************************************
 *                   START DEBUG
 *****************************************************************************/

#if _SAGE_DEBUG_MODE_ == 1
/**
 * Dump an object (DEBUG)
 */
VOID SAGE_DumpOBJ(SAGE_WavefrontObject *object)
{
  LONG idx;

  SAGE_DebugLog("** Dump OBJ **");
  SAGE_DebugLog("- File directory %s", object->filedir);
  SAGE_DebugLog("- Object vertices %d", object->nb_vertices);
  for (idx = 0;idx < object->nb_vertices;idx++) {
    SAGE_DebugLog(" => vertex %d : x=%f, y=%f, z=%f", idx, object->vertices[idx].x, object->vertices[idx].y, object->vertices[idx].z);
  }
  SAGE_DebugLog("- Object texture vertices %d", object->nb_vertexts);
  for (idx = 0;idx < object->nb_vertexts;idx++) {
    SAGE_DebugLog(" => texture vertex %d : u=%f, v=%f", idx, object->vertexts[idx].u, object->vertexts[idx].v);
  }
  SAGE_DebugLog("- Object normals %d", object->nb_normals);
  for (idx = 0;idx < object->nb_normals;idx++) {
    SAGE_DebugLog(" => normal %d : x=%f, y=%f, z=%f", idx, object->normals[idx].x, object->normals[idx].y, object->normals[idx].z);
  }
  SAGE_DebugLog("- Object faces %d", object->nb_faces);
  for (idx = 0;idx < object->nb_faces;idx++) {
    if (object->faces[idx].is_quad) {
      SAGE_DebugLog(
        " => face %d is quad : p1=%d, p2=%d, p3=%d, p4=%d, t1=%d, t2=%d, t3=%d, t4=%d, material=%d",
        idx, object->faces[idx].p1, object->faces[idx].p2, object->faces[idx].p3, object->faces[idx].p4,
        object->faces[idx].t1, object->faces[idx].t2, object->faces[idx].t3, object->faces[idx].t4, object->faces[idx].material
      );
    } else {
      SAGE_DebugLog(
        " => face %d is triangle : p1=%d, p2=%d, p3=%d, t1=%d, t2=%d, t3=%d, material=%d",
        idx, object->faces[idx].p1, object->faces[idx].p2, object->faces[idx].p3,
        object->faces[idx].t1, object->faces[idx].t2, object->faces[idx].t3, object->faces[idx].material
      );
    }
  }
  SAGE_DebugLog("- Object materials %d (%s)", object->nb_materials, object->matlib);
  for (idx = 0;idx < object->nb_materials;idx++) {
    SAGE_DebugLog(
      " => material %d : name=%s, file=%s, color=0x%06X, trans=%d, tcolor=0x%06X", idx,
      object->materials[idx].name, object->materials[idx].file, object->materials[idx].color,
      (object->materials[idx].transparent ? 1 : 0), object->materials[idx].tcolor
    );
  }
}

/**
 * Dump tokens
 */
VOID SAGE_DumpTokens(VOID)
{
  WORD idx;
  
  SAGE_DebugLog("** Dump tokens **");
  for (idx = 0;idx < obj_nb_tokens;idx++) {
    SAGE_DebugLog(" => token %d : %s", idx, obj_line_token[idx]);
  }
}
#endif

/*****************************************************************************
 *                   END DEBUG
 *****************************************************************************/

/**
 * Release a OBJ object
 */
VOID SAGE_ReleaseOBJ(SAGE_WavefrontObject *object)
{
  SD(SAGE_DebugLog("Release OBJ");)
  if (object != NULL) {
    if (object->vertices != NULL) {
      SAGE_FreeMem(object->vertices);
    }
    if (object->vertexts != NULL) {
      SAGE_FreeMem(object->vertexts);
    }
    if (object->normals != NULL) {
      SAGE_FreeMem(object->normals);
    }
    if (object->faces != NULL) {
      SAGE_FreeMem(object->faces);
    }
    if (object->materials != NULL) {
      SAGE_FreeMem(object->materials);
    }
    SAGE_FreeMem(object);
  }
}

/**
 * Check if a line is empty or is a comment
 */
BOOL SAGE_OBJIsEmptyLine(STRPTR line)
{
  if (*line == '#' || *line == '\0') {
    return TRUE;
  }
  while (*line != '\0') {
    if (*line != ' ' && *line != '\t' && *line != '\n') {
      return FALSE;
    }
    line++;
  }
  return TRUE;
}

/**
 * Check if a character is a space
 */
BOOL SAGE_OBJCharIsSpace(UBYTE chr)
{
  if (chr == ' ' || chr == '\t') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Check if a character is alphanumeric
 */
BOOL SAGE_OBJCharIsAlphanum(UBYTE chr)
{
  if (chr > 32 && chr < 127) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Check if a character is a end of a line
 */
BOOL SAGE_OBJCharIsEOL(UBYTE chr)
{
  if (chr == '\n' || chr =='\0') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Extract tokens from a line
 */
WORD SAGE_OBJTokenizeLine(STRPTR line)
{
  obj_nb_tokens = 0;
  if (!SAGE_OBJIsEmptyLine(line)) {
    while (!SAGE_OBJCharIsEOL(*line)) {
      if (SAGE_OBJCharIsSpace(*line)) {
        line++;
      } else {
        obj_line_token[obj_nb_tokens] = line;
        while (SAGE_OBJCharIsAlphanum(*line)) line++;
        *line++ = '\0';
        obj_nb_tokens++;
      }
    }
  }
  //SD(SAGE_DumpTokens();)
  return obj_nb_tokens;
}

/**
 * Analyze a Wavefront material file to define number of items
 */
BOOL SAGE_AnalyzeOBJMatFile(BPTR fd, SAGE_WavefrontObject *object)
{
  STRPTR error;
  WORD tokens;

  SD(SAGE_DebugLog("* SAGE_AnalyzeOBJMatFile");)
  while ((error = FGets(fd, obj_line_buffer, 1024)) != 0) {
    tokens = SAGE_OBJTokenizeLine(obj_line_buffer);
    if (tokens > 0) {
      if (strcmp(obj_line_token[0], "newmtl") == 0) {
        object->nb_materials++;
      }
    }
  }
  if (IoErr() != 0) {
    SAGE_SetError(SERR_READFILE);
    return FALSE;
  }
  return TRUE;
}

/**
 * Analyze a Wavefront file to define number of items
 */
BOOL SAGE_AnalyzeOBJFile(BPTR fd, SAGE_WavefrontObject *object)
{
  BPTR material_fd;
  STRPTR error;
  WORD tokens;
  
  SD(SAGE_DebugLog("* SAGE_AnalyzeOBJFile");)
  while ((error = FGets(fd, obj_line_buffer, 1024)) != 0) {
    tokens = SAGE_OBJTokenizeLine(obj_line_buffer);
    if (tokens > 0) {
      if (strcmp(obj_line_token[0], "mtllib") == 0 && tokens > 1) {
        strcpy(object->matlib, object->filedir);
        strcat(object->matlib, obj_line_token[1]);
        material_fd = Open(object->matlib, MODE_OLDFILE);
        if (material_fd != 0) {
          if (!SAGE_AnalyzeOBJMatFile(material_fd, object)) {
            Close(material_fd);
            return FALSE;
          }
          Close(material_fd);
        } else {
          SAGE_SetError(SERR_OPENFILE);
          return FALSE;
        }
      } else if (strcmp(obj_line_token[0], "v") == 0) {
        object->nb_vertices++;
      } else if (strcmp(obj_line_token[0], "vn") == 0) {
        object->nb_normals++;
      } else if (strcmp(obj_line_token[0], "vt") == 0) {
        object->nb_vertexts++;
      } else if (strcmp(obj_line_token[0], "f") == 0) {
        object->nb_faces++;
      }
    }
  }
  if (IoErr() != 0) {
    SAGE_SetError(SERR_READFILE);
    return FALSE;
  }
  return TRUE;
}

/**
 * Set vertex coordinates
 */
VOID SAGE_SetOBJVertex(SAGE_WavefrontObject *object, WORD index, STRPTR x_val, STRPTR y_val, STRPTR z_val)
{
  object->vertices[index].x = (FLOAT)atof(x_val);
  object->vertices[index].y = (FLOAT)atof(y_val);
  object->vertices[index].z = (FLOAT)atof(z_val);
}

/**
 * Set vertex texture coordinates
 */
VOID SAGE_SetOBJVertexTexture(SAGE_WavefrontObject *object, WORD index, STRPTR u_val, STRPTR v_val)
{
  object->vertexts[index].u = (FLOAT)atof(u_val);
  if (object->vertexts[index].u < 0.0) object->vertexts[index].u *= -1.0;
  object->vertexts[index].v = (FLOAT)atof(v_val);
  if (object->vertexts[index].v < 0.0) object->vertexts[index].v *= -1.0;
}

/**
 * Set vertex normal coordinates
 */
VOID SAGE_SetOBJNormal(SAGE_WavefrontObject *object, WORD index, STRPTR x_val, STRPTR y_val, STRPTR z_val)
{
  object->normals[index].x = (FLOAT)atof(x_val);
  object->normals[index].y = (FLOAT)atof(y_val);
  object->normals[index].z = (FLOAT)atof(z_val);
}

/**
 * Get the active material index
 */
WORD SAGE_GetActiveMaterial(SAGE_WavefrontObject *object, STRPTR name)
{
  WORD index;
  
  for (index = 0;index < object->nb_materials;index++) {
    if (strcmp(name, object->materials[index].name) == 0) {
      return index;
    }
  }
  return -1;
}

/**
 * Split face data indexes
 */
WORD SAGE_SplitFaceData(STRPTR face, STRPTR *data)
{
  data[0] = face;
  while (*face != '/' && *face != '\0') face++;
  *face++ = '\0';
  data[1] = face;
  while (*face != '/' && *face != '\0') face++;
  if (*face == '\0') return 2;
  *face++ = '\0';
  data[2] = face;
  return 3;
}

/**
 * Set face informations
 */
VOID SAGE_SetOBJFace(SAGE_WavefrontObject *object, WORD index, STRPTR p1, STRPTR p2, STRPTR p3, STRPTR p4)
{
  WORD nb_data;
  STRPTR data[3];

  nb_data = SAGE_SplitFaceData(p1, data);
  if (nb_data > 1) {
    object->faces[index].p1 = atoi(data[0]) - 1;
    object->faces[index].t1 = atoi(data[1]) - 1;
  }
  nb_data = SAGE_SplitFaceData(p2, data);
  if (nb_data > 1) {
    object->faces[index].p2 = atoi(data[0]) - 1;
    object->faces[index].t2 = atoi(data[1]) - 1;
  }
  nb_data = SAGE_SplitFaceData(p3, data);
  if (nb_data > 1) {
    object->faces[index].p3 = atoi(data[0]) - 1;
    object->faces[index].t3 = atoi(data[1]) - 1;
  }
  if (p4 != NULL) {
    nb_data = SAGE_SplitFaceData(p4, data);
    if (nb_data > 1) {
      object->faces[index].p4 = atoi(data[0]) - 1;
      object->faces[index].t4 = atoi(data[1]) - 1;
    }
    object->faces[index].is_quad = TRUE;
  } else {
    object->faces[index].is_quad = FALSE;
  }
}

/**
 * Parse a Wavefront material file and fill the structure with data
 */
BOOL SAGE_ParseMaterialFile(BPTR fd, SAGE_WavefrontObject *object)
{
  WORD idx_material, tokens;
  LONG red, green, blue;
  STRPTR error;

  SD(SAGE_DebugLog("* SAGE_ParseMaterialFile");)
  idx_material = -1;
  while ((error = FGets(fd, obj_line_buffer, 1024)) != 0) {
    tokens = SAGE_OBJTokenizeLine(obj_line_buffer);
    if (tokens > 0) {
      if (strcmp(obj_line_token[0], "newmtl") == 0 && tokens > 1) {
        idx_material++;
        strcpy(object->materials[idx_material].name, obj_line_token[1]);
        strcpy(object->materials[idx_material].file, "");
        object->materials[idx_material].color = 0;
        object->materials[idx_material].texture = -1;
      } else if (strcmp(obj_line_token[0], "Tr") == 0 && tokens > 1) {
        if (atof(obj_line_token[1]) == 1.0F) {
          object->materials[idx_material].transparent = TRUE;
        }
      } else if (strcmp(obj_line_token[0], "Tf") == 0 && tokens > 3) {
        red = (LONG)(atof(obj_line_token[1]) * 255.0);
        green = (LONG)(atof(obj_line_token[2]) * 255.0);
        blue = (LONG)(atof(obj_line_token[3]) * 255.0);
        object->materials[idx_material].tcolor = (red << 16) + (green << 8) + blue;
      } else if (strcmp(obj_line_token[0], "Kd") == 0 && tokens > 3) {
        red = (LONG)(atof(obj_line_token[1]) * 255.0);
        green = (LONG)(atof(obj_line_token[2]) * 255.0);
        blue = (LONG)(atof(obj_line_token[3]) * 255.0);
        object->materials[idx_material].color = (red << 16) + (green << 8) + blue;
      } else if (strcmp(obj_line_token[0], "map_Kd") == 0 && tokens > 1) {
        strcpy(object->materials[idx_material].file, object->filedir);
        strcat(object->materials[idx_material].file, obj_line_token[1]);
      }
    }
  }
  if (IoErr() != 0) {
    SAGE_SetError(SERR_READFILE);
    return FALSE;
  }
  return TRUE;
}

/**
 * Parse a Wavefront file and fill the structure with data
 */
BOOL SAGE_ParseWavefrontFile(BPTR fd, SAGE_WavefrontObject *object)
{
  BPTR material_fd;
  WORD idx_vertex, idx_texture, idx_normal, idx_face, tokens, active_material;
  STRPTR error;

  SD(SAGE_DebugLog("* SAGE_ParseWavefrontFile first pass");)
  if (object->nb_materials > 0) {
    material_fd = Open(object->matlib, MODE_OLDFILE);
    if (material_fd != 0) {
      if (!SAGE_ParseMaterialFile(material_fd, object)) {
        Close(material_fd);
        return FALSE;
      }
      Close(material_fd);
    } else {
      SAGE_SetError(SERR_OPENFILE);
      return FALSE;
    }
  }
  idx_vertex = 0;
  idx_texture = 0;
  idx_normal = 0;
  idx_face = 0;
  active_material = S3DE_OBJNOMATERIAL;
  SD(SAGE_DebugLog("* SAGE_ParseWavefrontFile second pass");)
  while ((error = FGets(fd, obj_line_buffer, 1024)) != 0) {
    tokens = SAGE_OBJTokenizeLine(obj_line_buffer);
    if (tokens > 0) {
      if (strcmp(obj_line_token[0], "v") == 0 && tokens > 3) {
        SAGE_SetOBJVertex(object, idx_vertex, obj_line_token[1], obj_line_token[2], obj_line_token[3]);
        idx_vertex++;
      } else if (strcmp(obj_line_token[0], "vt") == 0 && tokens > 2) {
        SAGE_SetOBJVertexTexture(object, idx_texture, obj_line_token[1], obj_line_token[2]);
        idx_texture++;
      } else if (strcmp(obj_line_token[0], "vn") == 0 && tokens > 3) {
        SAGE_SetOBJNormal(object, idx_normal, obj_line_token[1], obj_line_token[2], obj_line_token[3]);
        idx_normal++;
      } else if (strcmp(obj_line_token[0], "usemtl") == 0 && tokens > 1) {
        active_material = SAGE_GetActiveMaterial(object, obj_line_token[1]);
      } else if (strcmp(obj_line_token[0], "f") == 0 && tokens > 3) {
        if (tokens == 4) {
          SAGE_SetOBJFace(object, idx_face, obj_line_token[1], obj_line_token[2], obj_line_token[3], NULL);
        } else {
          SAGE_SetOBJFace(object, idx_face, obj_line_token[1], obj_line_token[2], obj_line_token[3], obj_line_token[4]);
        }
        object->faces[idx_face].material = active_material;
        idx_face++;
      }
    }
  }
  if (IoErr() != 0) {
    return FALSE;
  }
  SD(SAGE_DumpOBJ(object));
  return TRUE;
}

/**
 * Load object materials
 */
BOOL SAGE_LoadOBJMaterial(SAGE_WavefrontObject *object)
{
  WORD idx_material, idx_texture;

  SD(SAGE_DebugLog("* SAGE_LoadOBJMaterial");)
  for (idx_material = 0;idx_material < object->nb_materials;idx_material++) {
    if (strlen(object->materials[idx_material].file) > 0) {
      SD(SAGE_DebugLog(" => mat %d : loading %s", idx_material, object->materials[idx_material].file);)
      idx_texture = SAGE_GetFreeTextureIndex();
      if (idx_texture == STEX_NOFREEINDEX) {
        SAGE_SetError(SERR_TEX_INDEX);
        return FALSE;
      }
      // Load the texture
      if (!SAGE_CreateTextureFromFile(idx_texture, object->materials[idx_material].file)) {
        return FALSE;
      }
      // Set texture transparency
      if (object->materials[idx_material].transparent) {
        SAGE_SetTextureTransparency(idx_texture, object->materials[idx_material].tcolor);
      }
      // Add texture to card
      if (!SAGE_AddTexture(idx_texture)) {
        SAGE_ReleaseTexture(idx_texture);
        return FALSE;
      }
      object->materials[idx_material].texture = idx_texture;
    } else {
      object->materials[idx_material].texture = STEX_USECOLOR;
    }
  }
  return TRUE;
}

/**
 * Build a full entity from a Wavefront object
 */
SAGE_Entity *SAGE_BuildEntityFromObject(SAGE_WavefrontObject *object)
{
  SAGE_Entity *entity;
  WORD idx, size;

  SD(SAGE_DebugLog("SAGE_BuildEntityFromObject");)
  entity = NULL;
  if (SAGE_LoadOBJMaterial(object)) {
    entity = SAGE_CreateEntity(object->nb_vertices, object->nb_faces);
    if (entity != NULL) {
      for (idx = 0;idx < object->nb_vertices;idx++) {
        entity->vertices[idx].x = object->vertices[idx].x;
        entity->vertices[idx].y = object->vertices[idx].y;
        entity->vertices[idx].z = object->vertices[idx].z;
      }
      for (idx = 0;idx < object->nb_faces;idx++) {
        size = 0;
        if (object->faces[idx].material != S3DE_OBJNOMATERIAL) {
          if (object->materials[object->faces[idx].material].texture != STEX_USECOLOR) {
            size = SAGE_GetTextureSize(object->materials[object->faces[idx].material].texture) - 1;
          }
        }
        entity->faces[idx].p1 = object->faces[idx].p1;
        entity->faces[idx].u1 = object->vertexts[object->faces[idx].t1].u * size;
        entity->faces[idx].v1 = object->vertexts[object->faces[idx].t1].v * size;
        entity->faces[idx].p2 = object->faces[idx].p2;
        entity->faces[idx].u2 = object->vertexts[object->faces[idx].t2].u * size;
        entity->faces[idx].v2 = object->vertexts[object->faces[idx].t2].v * size;
        entity->faces[idx].p3 = object->faces[idx].p3;
        entity->faces[idx].u3 = object->vertexts[object->faces[idx].t3].u * size;
        entity->faces[idx].v3 = object->vertexts[object->faces[idx].t3].v * size;
        if (object->faces[idx].is_quad) {
          entity->faces[idx].is_quad = TRUE;
          entity->faces[idx].p4 = object->faces[idx].p4;
          entity->faces[idx].u4 = object->vertexts[object->faces[idx].t4].u * size;
          entity->faces[idx].v4 = object->vertexts[object->faces[idx].t4].v * size;
        }
        if (object->faces[idx].material != S3DE_OBJNOMATERIAL) {
          entity->faces[idx].color = SAGE_RemapColor(object->materials[object->faces[idx].material].color);
          entity->faces[idx].texture = object->materials[object->faces[idx].material].texture;
        } else {
          entity->faces[idx].color = SAGE_RemapColor(0xffffff);
          entity->faces[idx].texture = STEX_USECOLOR;
        }
      }
      SAGE_SetEntityRadius(entity);
      SAGE_SetEntityNormals(entity);
    }
  }
  SAGE_ReleaseOBJ(object);
  return entity;
}

/**
 * Load a Wavefront object
 * 
 * @param file_handle Object file handle
 * @param file_path   Object file path
 * 
 * @return SAGE entity structure
 */
SAGE_Entity *SAGE_LoadOBJ(BPTR file_handle, STRPTR file_path)
{
  SAGE_WavefrontObject *object;
  STRPTR dirname;
  BOOL error;

  SD(SAGE_DebugLog("Load OBJ %s", file_path);)
  object = (SAGE_WavefrontObject *)SAGE_AllocMem(sizeof(SAGE_WavefrontObject));
  if (object == NULL) {
    return NULL;
  }
  // Get the file dir
  strcpy(object->filedir, file_path);
  dirname = FilePart(object->filedir);
  *dirname = '\0';
  // Analyze the Wavefront file
  if (SAGE_AnalyzeOBJFile(file_handle, object)) {
    error = FALSE;
    if (object->nb_vertices > 0) {
      object->vertices = (SAGE_OBJVertice *)SAGE_AllocMem(sizeof(SAGE_OBJVertice)*object->nb_vertices);
      if (object->vertices == NULL) error = TRUE;
    }
    if (object->nb_vertexts > 0 && !error) {
      object->vertexts = (SAGE_OBJVerticeTexture *)SAGE_AllocMem(sizeof(SAGE_OBJVerticeTexture)*object->nb_vertexts);
      if (object->vertexts == NULL) error = TRUE;
    }
    if (object->nb_normals > 0 && !error) {
      object->normals = (SAGE_OBJVertice *)SAGE_AllocMem(sizeof(SAGE_OBJVertice)*object->nb_normals);
      if (object->normals == NULL) error = TRUE;
    }
    if (object->nb_faces > 0 && !error) {
      object->faces = (SAGE_OBJFace *)SAGE_AllocMem(sizeof(SAGE_OBJFace)*object->nb_faces);
      if (object->faces == NULL) error = TRUE;
    }
    if (object->nb_materials > 0 && !error) {
      object->materials = (SAGE_OBJMaterial *)SAGE_AllocMem(sizeof(SAGE_OBJMaterial)*object->nb_materials);
      if (object->materials == NULL) error = TRUE;
    }
    if (error) {
      SAGE_FreeMem(object);
      return NULL;
    }
  } else {
    SAGE_FreeMem(object);
    return NULL;
  }
  Seek(file_handle, 0, OFFSET_BEGINNING);         // Back to beginning of file
  if (SAGE_ParseWavefrontFile(file_handle, object)) {
    return SAGE_BuildEntityFromObject(object);
  }
  SAGE_ReleaseOBJ(object);
  return NULL;
}
