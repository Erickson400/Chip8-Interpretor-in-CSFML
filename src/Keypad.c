#include "Keypad.h"

void Keypad_UpdateInput(Keypad* pm_keypad, const sfEvent pm_event) {

	// If not a key then skip this function
	u8 Iskey = (pm_event.type == sfEvtKeyPressed || pm_event.type == sfEvtKeyReleased);
	if (!Iskey) return;

	// Check every key if its pressed or released
	u8 i;
	for (i = 0; i < 16; i++) {

		// if the keycode is the same as key
		if (pm_event.key.code == pm_keypad->keysMap[i]) {
			if (pm_event.type == sfEvtKeyPressed)
				pm_keypad->keysData[i] = 1;
			else if (pm_event.type == sfEvtKeyReleased)
				pm_keypad->keysData[i] = 0;
		}
	}

	// For Debugging
	//printf("----------\n");
	//for (i = 0; i < 16; i++) {
		//printf("%d\n", param_keypad->keysData[i]);
	//}
}
Keypad Keypad_create() {
	int local_keys[16] = {
						sfKeyX,
						sfKeyNum1,
						sfKeyNum2,
						sfKeyNum3,
						sfKeyQ,
						sfKeyW,
						sfKeyE,
						sfKeyA,
						sfKeyS,
						sfKeyD,
						sfKeyZ,
						sfKeyC,
						sfKeyNum4,
						sfKeyR,
						sfKeyF,
						sfKeyV
	};
	Keypad local_keypad = { *local_keys };

	// Copies the keys to the new struct
	u8 i;
	for (i = 0; i < 16; i++) {
		local_keypad.keysData[i] = 0; // Clears the data just in case
		local_keypad.keysMap[i] = local_keys[i];
	}

	return local_keypad;
}
