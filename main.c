#include <stdlib.h>
#include <stdio.h>
#include <SFML/Graphics.h>

#include "ControlUnit.h"

int main(void) {
	sfRenderWindow* window	= sfRenderWindow_create((sfVideoMode){ 1024, 512 }, "Chip8 Interpretor", sfClose, NULL);
	sfView* view			= sfView_create();
	CPU cpu					= CPU_create();

	//printf("%d", (int)sizeof(sos));

	sfView_setCenter(view, (sfVector2f) { 64/2, 32/2 });
	sfView_setSize(view, (sfVector2f) { 64, 32 });
	sfRenderWindow_setView(window, view);
	sfRenderWindow_setFramerateLimit(window, 60);
	sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);

	while (sfRenderWindow_isOpen(window)) {
		sfEvent event;
		while (sfRenderWindow_pollEvent(window, &event)) {
			CPU_UpdateInput(&cpu, event);
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		CPU_Update(&cpu);
		CPU_Render(&cpu, window);
		sfRenderWindow_display(window);
	}

	CPU_destroy(&cpu);
	sfView_destroy(view);
	sfRenderWindow_destroy(window);
	return 0;
}




