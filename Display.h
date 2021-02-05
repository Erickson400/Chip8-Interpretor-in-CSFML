#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <SFML/Graphics.h>
#include "Types.h"

typedef struct {
	sfImage* buffer;
	sfTexture* local_texture;
	sfSprite* local_sprite;
} Display;

void Display_render(Display* pm_display, sfRenderWindow* pm_window);
u8 Display_flipPixel(Display* pm_display, u8 x, u8 y, u8 IsTrue);
void Display_clear(Display* pm_display);
Display Display_create();
void Display_destroy(Display* pm_display);

#endif // !DISPLAY_H



