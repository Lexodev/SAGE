/**
 * scrolltext.c
 * 
 * SAGE (Simple Amiga Game Engine) project
 * Demo of simple text scrolling
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 September 2024
 */

#include <stdio.h>
#include <stdlib.h>

#include <sage/sage.h>

#define SCREEN_WIDTH          320L
#define SCREEN_HEIGHT         240L
#define SCREEN_DEPTH          16L

#define FONT_WIDTH            32
#define FONT_HEIGHT           32
#define FONT_NUM              60
#define FONTPIC_WIDTH         320
#define FONTPIC_HEIGHT        192

#define TEXTFIELD_WIDTH       SCREEN_WIDTH+FONT_WIDTH
#define TEXTFIELD_HEIGHT      FONT_HEIGHT
#define TEXTFIELD_LAYER       0

#define TEXTSCROLL_SPEED      2
#define TEXTSCROLL_POSX       0
#define TEXTSCROLL_POSY       120

SAGE_Picture *fonts;

STRPTR message = "LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT. SUSPENDISSE ET LOREM MASSA ! NULLAM ALIQUAM, LECTUS IN CONGUE LUCTUS, MI LIGULA PULVINAR METUS, UT VEHICULA ANTE EST EU DOLOR ? FUSCE NEC CONGUE NUNC, VEL IMPERDIET EROS. PROIN VITAE METUS VITAE RISUS PULVINAR AUCTOR. NUNC SUSCIPIT ELIT A ERAT ALIQUET 5821 ELEMENTUM. VESTIBULUM ET TURPIS (SAPIEN). NAM AC LACINIA ERAT, ET ULTRICES AUGUE.        ";
UWORD message_pos = 0, font_posx[FONT_NUM], font_posy[FONT_NUM], layer_posx = SCREEN_WIDTH+FONT_WIDTH, char_posx = SCREEN_WIDTH, char_load = 0;

BOOL OpenScreen(VOID)
{
  SAGE_AppliLog("Opening screen");
  if (SAGE_OpenScreen(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SSCR_STRICTRES)) {
    SAGE_HideMouse();
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitTextfieldLayer(VOID)
{
  SAGE_AppliLog("Create text field layer");
  if (SAGE_CreateLayer(TEXTFIELD_LAYER, TEXTFIELD_WIDTH, TEXTFIELD_HEIGHT)) {
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL InitFonts(VOID)
{
  UWORD x, y, idx, line, column;

  SAGE_AppliLog("Load fonts picture");
  if ((fonts = SAGE_LoadPicture("fonts_32_32.png")) != NULL) {
    SAGE_AppliLog("Precalc fonts coordinates");
    idx = 0;
    y = 0;
    for (line = 0; line < (FONTPIC_HEIGHT / FONT_HEIGHT); line++) {
      x = 0;
      for (column = 0;column < (FONTPIC_WIDTH / FONT_WIDTH); column++) {
        font_posx[idx] = x;
        font_posy[idx] = y;
        x += FONT_WIDTH;
        idx++;
      }
      y += FONT_HEIGHT;
    }
    return TRUE;
  }
  SAGE_DisplayError();
  return FALSE;
}

BOOL Init(VOID)
{
  if (!OpenScreen()) {
    return FALSE;
  }
  if (!InitTextfieldLayer()) {
    return FALSE;
  }
  if (!InitFonts()) {
    return FALSE;
  }
  return TRUE;
}

VOID Update(VOID)
{
  UBYTE new_char;
  UWORD char_index;
  
  // Should we load a new char from the message
  if (char_load == 0) {
    new_char = message[message_pos];
    // Have we reach the end of the message ?
    if (new_char == 0) {
      // Wrap the message;
      message_pos = 0;
      new_char = message[message_pos];
    }
    char_load = FONT_WIDTH;
    message_pos++;          // Next char
    char_index = new_char - ' ';   // Space is our first char in the font picture
    // If we have some chars that are not in our fonts
    if (char_index >= FONT_NUM) {
      char_index = 0;
    }
    // Copy the char to the layer
    SAGE_BlitPictureToLayer(fonts, font_posx[char_index], font_posy[char_index], FONT_WIDTH, FONT_HEIGHT, TEXTFIELD_LAYER, char_posx, 0);
    char_posx += FONT_WIDTH;
    if (char_posx > SCREEN_WIDTH) {
      char_posx = 0;
    }
  }
  char_load -= TEXTSCROLL_SPEED;
  layer_posx += TEXTSCROLL_SPEED;
  if (layer_posx >= (SCREEN_WIDTH+FONT_WIDTH)) {
    layer_posx = 0;
  }
}

VOID Render(VOID)
{
  // Set the text layer view (using the wrapping feature of layers to simulate infite scroll)
  SAGE_SetLayerView(TEXTFIELD_LAYER, layer_posx, 0, SCREEN_WIDTH, TEXTFIELD_HEIGHT);
  // Blit the text layer to the screen
  SAGE_BlitLayerToScreen(TEXTFIELD_LAYER, TEXTSCROLL_POSX, TEXTSCROLL_POSY);
  // Switch screen buffers
  SAGE_RefreshScreen();
}

VOID Restore(VOID)
{
  // Release the graphics
  SAGE_ReleasePicture(fonts);
  SAGE_ReleaseLayer(TEXTFIELD_LAYER);
  // Show the mouse
  SAGE_ShowMouse();
  // And close the screen
  SAGE_CloseScreen();
}

void main(void)
{
  SAGE_Event *event = NULL;
  BOOL finish;

  //SAGE_SetLogLevel(SLOG_WARNING);
  SAGE_AppliLog("** SAGE library Scrolltext demo V1.0 **");
  SAGE_AppliLog("Initialize SAGE");
  // Init the SAGE system with only video module
  if (SAGE_Init(SMOD_VIDEO)) {
    // Init all demo elements
    if (Init()) {
      finish = FALSE;
      while (!finish) {
        // Read all events raised by the screen
        while ((event = SAGE_GetEvent()) != NULL) {
          if (event->type == SEVT_MOUSEBT) {
            // If we click on mouse button, we stop the loop
            finish = TRUE;
          } else if (event->type == SEVT_RAWKEY && event->code == SKEY_FR_ESC) {
            // If we press the ESC key, we stop the loop
            finish = TRUE;
          }
        }
        // Update the demo data
        Update();
        // Render the demo
        Render();
      }
    } else {
      SAGE_AppliLog("Init error");
      SAGE_DisplayError();
    }
    Restore();
  }
  SAGE_AppliLog("Closing SAGE");
  SAGE_Exit();
  SAGE_AppliLog("End of demo");
}
