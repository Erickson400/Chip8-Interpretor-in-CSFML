#include "ControlUnit.h"

void CPU_UpdateInput(CPU* pm_CPU, const sfEvent pm_event) {
	if (!pm_CPU->isRunnable) return;
	Keypad_UpdateInput(&pm_CPU->keypad, pm_event);
}
void CPU_Render(CPU* pm_CPU, sfRenderWindow* pm_window) {
	if (!pm_CPU->isRunnable) return;
	Display_render(&pm_CPU->display, pm_window);
}
CPU CPU_create() {
	CPU local_CPU; 
	local_CPU.isRunnable = 1;

	// Allocate memory for RAM on heap
	local_CPU.RAM = (char*)malloc(0xFFF);
	if (local_CPU.RAM != NULL) {
		
		// Load Rom into Memory
		if (FileManager_loadFile(_FILE_, local_CPU.RAM)) {

			// Load font into Memory
			char font[80] = {
				0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
				0x20, 0x60, 0x20, 0x20, 0x70, // 1
				0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
				0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
				0x90, 0x90, 0xF0, 0x10, 0x10, // 4
				0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
				0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
				0xF0, 0x10, 0x20, 0x40, 0x40, // 7
				0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
				0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
				0xF0, 0x90, 0xF0, 0x90, 0x90, // A
				0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
				0xF0, 0x80, 0x80, 0x80, 0xF0, // C
				0xE0, 0x90, 0x90, 0x90, 0xE0, // D
				0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
				0xF0, 0x80, 0xF0, 0x80, 0x80  // F
			};
			for (u8 i = 0; i < 80; i++) {
				*(local_CPU.RAM + i) = font[i];
			}

			//Inizialize the Keypad and Display
			local_CPU.display = Display_create();
			local_CPU.keypad = Keypad_create();
			local_CPU.StackIndex = 0;
			local_CPU.PC = 0x200;
			local_CPU.Delay = 0;
			local_CPU.DelaySound = 0;

		} else {
			local_CPU.isRunnable = 0;
			free(local_CPU.RAM);
			return local_CPU;
		}
	} else {
		printf("---ERROR: Allocating Ram in CPU"); 
		local_CPU.isRunnable = 0; 
		return local_CPU;
	}

	return local_CPU;
}
void CPU_destroy(CPU* pm_CPU) {
	if (!pm_CPU->isRunnable) return;
	free(pm_CPU->RAM);
	Display_destroy(&pm_CPU->display);
}

static void _CPU_Decode(CPU* pm_CPU, const u8 pm_byte1, const u8 pm_byte2) {

	// Fetch
	u8 nibble[4] = {
		(pm_byte1 >> 4) & 0b1111,
		pm_byte1 & 0b1111,
		(pm_byte2 >> 4) & 0b1111,
		pm_byte2 & 0b1111
	};

	//printf("Byte1: %X, Nibble1: %X, Nibble2: %X\n", pm_byte1, nibble[0], nibble[1]);
	//printf("Byte2: %X, Nibble1: %X, Nibble2: %X\n", pm_byte2, nibble[2], nibble[3]);
	//printf("---------------\n");

	// Increment PC
	if (pm_CPU->PC < 0xFFD) { pm_CPU->PC += 2; }
	else { printf("---ALERT: PC has reached the end of RAM\n"); }

	// Pre-calcuated variables
	u8 VX	=  nibble[1];
	u8 VY	=  nibble[2];
	u8 N	=  nibble[3];
	u8 NN	= (nibble[2] * 0x10) + nibble[3];
	u16 NNN	= (nibble[1] * 0x100) + (nibble[2] * 0x10) + nibble[3];

	// Execute the instrunction
	switch (nibble[0]) 
	{
	case 0:
		switch (NNN)
		{
		case 0x0E0: // Clear screen
			Display_clear(&pm_CPU->display);
			break;
		case 0x0EE: // Pop Stack
			if (pm_CPU->StackIndex > 0) pm_CPU->StackIndex -= 1;
			pm_CPU->PC = pm_CPU->stack[pm_CPU->StackIndex];
			break;
		default:
			printf("---ALERT: Execute machine language command called! %x \n", NNN);
			break;
		}
		break;
	case 1: // Set PC
		pm_CPU->PC = NNN;
		break;
	case 2: // Push to Stack
		pm_CPU->stack[pm_CPU->StackIndex] = pm_CPU->PC;
		if (pm_CPU->StackIndex < STACK_MAX_SIZE - 1) pm_CPU->StackIndex += 1;
		
		pm_CPU->PC = NNN;
		break;
	case 3: // Skip if equal
		if (pm_CPU->Register[VX] == NN) pm_CPU->PC += 2;
		break;
	case 4: // Skip if NOT equal
		if (pm_CPU->Register[VX] != NN) pm_CPU->PC += 2;
		break;
	case 5: // Skip if Register X is same as Y
		if (pm_CPU->Register[VX] == pm_CPU->Register[VY]) pm_CPU->PC += 2;
		break;
	case 6: // Set register X to NN
		pm_CPU->Register[VX] = NN;
		break;
	case 7: // Add NN to register X
		pm_CPU->Register[VX] += NN;
		break;
	case 8: // Bitwise operations and Register maths
		switch (nibble[3]) 
		{
		case 0: // Set register VY to register VX
			pm_CPU->Register[VX] = pm_CPU->Register[VY];
			break;
		case 1: // OR
			pm_CPU->Register[VX] |= pm_CPU->Register[VY];
			break;
		case 2: // AND
			pm_CPU->Register[VX] &= pm_CPU->Register[VY];
			break;
		case 3: // XOR
			pm_CPU->Register[VX] ^= pm_CPU->Register[VY];
			break;
		case 4: // Add reg VX and reg VY, and set VF to 1 if a carry occurs
			{
			register u16 tmp_num = pm_CPU->Register[VX] + pm_CPU->Register[VY];
			pm_CPU->Register[VX] += pm_CPU->Register[VY];
			pm_CPU->Register[15] = tmp_num > 0xFF ? 1 : 0;
			}
			break;
		case 5: // Subtract reg VX and reg VY, and set VF to 0 if a carry occurs
			pm_CPU->Register[15] = pm_CPU->Register[VX] > pm_CPU->Register[VY] ? 1 : 0;
			pm_CPU->Register[VX] -= pm_CPU->Register[VY];
			break;
		case 6: // Shift reg VY RIGHT and store it in reg VX, then store least significant bit in VF
			pm_CPU->Register[15] = pm_CPU->Register[VX] & 1;
			pm_CPU->Register[VX] = pm_CPU->Register[VY] >> 1;
			break;
		case 7: // Set reg VX to the value of reg VY minus VX, set VF to 0 if a borrow accurs
			pm_CPU->Register[15] = pm_CPU->Register[VX] > pm_CPU->Register[VY] ? 0 : 1;
			pm_CPU->Register[VX] = pm_CPU->Register[VY] - pm_CPU->Register[VX]; //If X barrows if its greater
			break;
		case 0xE: // Shift reg VY LEFT and store it in reg VX, then store least significant bit in VF
			pm_CPU->Register[15] = (pm_CPU->Register[VX] >> 7) & 1;
			pm_CPU->Register[VX] = pm_CPU->Register[VY] << 1;
			break;
		}
		break;
	case 9: // Skip if reg VX and reg VY are NOT equal
		if (pm_CPU->Register[VX] != pm_CPU->Register[VY]) pm_CPU->PC += 2;
		break;
	case 0xA: // Store memory address NNN in register I
		pm_CPU->IReg = NNN;
		break;
	case 0xB: // Jump to address NNN + V0
		pm_CPU->IReg = NNN + pm_CPU->Register[0];
		break;
	case 0xC: // Set VX to a random number with a mask of NN
		pm_CPU->Register[VX] = (rand() % 0x100) & NN;
		break;
	case 0xD: //Draw
		{ 
		u32 x = pm_CPU->Register[VX] % 64;
		u32 y = pm_CPU->Register[VY] % 32;
		pm_CPU->Register[15] = 0;

		for (u32 i = 0; i < N; i++) { // row
			u32 wrap_row = (y + i) % 32;
			u32 row_byte = pm_CPU->RAM[pm_CPU->IReg + i];

			for (u32 j = 0; j < 8; j++) { // pixel / horizontal
				u32 wrap_pixel = (x + (7 - j)) % 64;

				u32 Pixel_Value = (row_byte >> j) & 1;
				if (Display_flipPixel(&pm_CPU->display, wrap_pixel, wrap_row, Pixel_Value)) {
					pm_CPU->Register[15] = 1;
				}
			}
		}

		}
		break;
	case 0xE: // Key Input
		if (pm_CPU->Register[VX] > 15) break;

		switch (NN)
		{
		case 0x9E:
			if (pm_CPU->keypad.keysData[pm_CPU->Register[VX]]) pm_CPU->PC += 2;
			break;
		case 0xA1:
			if (!pm_CPU->keypad.keysData[pm_CPU->Register[VX]]) pm_CPU->PC += 2;
			break;
		}
		break;
	case 0xF:
		switch (NN) 
		{
		case 0x7: // Set reg VX to delay timer
			pm_CPU->Register[VX] = pm_CPU->Delay;
			break;
		case 0x0A: // Wait for a keypress and store the result in register VX
			pm_CPU->PC -= 2;
			for (u8 i = 0; i < 16; i++) {
				if (pm_CPU->keypad.keysData[i]) {
					pm_CPU->PC += 2;
					pm_CPU->Register[VX] = pm_CPU->keypad.keysData[i];
				}
			}
			break;
		case 0x15: // Set the delay timer to the value of reg VX
			pm_CPU->Delay = pm_CPU->Register[VX];
			break;
		case 0x18: // Set the sound timer to the value of reg VX
			pm_CPU->DelaySound = pm_CPU->Register[VX];
			break;
		case 0x1E: //  	Add the value stored in reg VX to reg I
			pm_CPU->IReg += pm_CPU->Register[VX];
			break;
		case 0x29: // Set reg I to Chip8 Character address
			pm_CPU->IReg = (pm_CPU->Register[VX] & 0b1111) * 5;
			break;
		case 0x33:
			pm_CPU->RAM[pm_CPU->IReg] = ((pm_CPU->Register[VX] / 100) % 10);
			pm_CPU->RAM[pm_CPU->IReg + 1] = ((pm_CPU->Register[VX] / 10) % 10);
			pm_CPU->RAM[pm_CPU->IReg + 2] = (pm_CPU->Register[VX] % 10);
			break;
		case 0x55:
			{
			u8 i = 0;
			while (i <= VX) {
				pm_CPU->RAM[pm_CPU->IReg + i] = pm_CPU->Register[i];
				i++;
			}
			
			}
			break;
		case 0x65:
			{
			u8 i = 0;
			while (i <= VX) {
				pm_CPU->Register[i] = pm_CPU->RAM[pm_CPU->IReg + i];
				i++;
			}

			}
			break;

		}
		break;
	}
}
void CPU_Update(CPU* pm_CPU) {
	if (!pm_CPU->isRunnable) return;

	for (u8 i = 0; i < CYCLE_SPEED; i++) {
		// Clock Cycle
		_CPU_Decode(pm_CPU, pm_CPU->RAM[pm_CPU->PC], pm_CPU->RAM[pm_CPU->PC + 1]);
		if (pm_CPU->Delay > 0) pm_CPU->Delay -= 1;
		if (pm_CPU->DelaySound > 0) pm_CPU->DelaySound -= 1;
	}
}



