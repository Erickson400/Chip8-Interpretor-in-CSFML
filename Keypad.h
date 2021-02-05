#pragma once
#ifndef KEYPAD_H
#define KEYPAD_H

#include <SFML/Graphics.h>
#include "Types.h"

typedef struct {
	u8 keysMap[16];
	u8 keysData[16];
} Keypad;

void Keypad_UpdateInput(Keypad* pm_keypad, const sfEvent pm_event);
Keypad Keypad_create();

#endif