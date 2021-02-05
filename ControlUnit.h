#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include <stdio.h>
#include <stdlib.h>

#include "Display.h"
#include "FileManager.h"
#include "Keypad.h"
#include "Types.h"

#define CYCLE_SPEED 10
#define STACK_MAX_SIZE 20
#define _FILE_ "c8games/TEST"

typedef struct {
	Display display;
	Keypad keypad;
	u8* RAM;
	u16 stack[STACK_MAX_SIZE];
	u8 StackIndex; // The Stack Index points to the top element
	u8 Register[16];
	u8 Delay;
	u8 DelaySound;
	u16 IReg;
	u16 PC;
	u8 isRunnable;
} CPU;

void CPU_UpdateInput(CPU* pm_CPU, const sfEvent pm_event);
void CPU_Render(CPU* pm_CPU, sfRenderWindow* pm_window);
CPU CPU_create();
void CPU_destroy(CPU* pm_CPU);
void CPU_Update(CPU* pm_CPU);

#endif // !CONTROLUNIT_H


