#include "Display.h"

void Display_render(Display* pm_display, sfRenderWindow* pm_window) {
	sfTexture_updateFromImage(pm_display->local_texture, pm_display->buffer, 0, 0);
	sfRenderWindow_drawSprite(pm_window, pm_display->local_sprite, NULL);
}
u8 Display_flipPixel(Display* pm_display, u8 x, u8 y, u8 IsTrue) {
	// This function includes the XOR setting
	// NOTE: cordinates must be within the 63x31 range

	// leave if not want to change, or if pixel outisde of Image
	if (!IsTrue) return 0;
	if (x > 63 || y > 31) {
		printf("---ERROR: Tried fliping pixel outside Image. (%u, %u)---\n", x % 0x100, y % 0x100);
		return 0;
	}

	// True if goes from White to Black
	u8 local_return = 0;

	// If the pixel is filled
	sfColor MainColor = sfWhite;
	sfColor EmptyColor = sfBlack;

	u32 pixelColor = sfColor_toInteger(sfImage_getPixel(pm_display->buffer, x, y));
	u32 emptyToInt = sfColor_toInteger(EmptyColor);

	if (pixelColor == emptyToInt) { // If canvas empty
		sfImage_setPixel(pm_display->buffer, x, y, MainColor);
		local_return = 0;
	}
	else {
		sfImage_setPixel(pm_display->buffer, x, y, EmptyColor);
		local_return = 1;
	}

	return local_return;
}
void Display_clear(Display* pm_display) {
	for (u8 y = 0; y < 32; y++) {
		for (u8 x = 0; x < 64; x++) {
			sfImage_setPixel(pm_display->buffer, x, y, sfBlack);
		}
	}
}
Display Display_create() {
	Display localDis;

	localDis.buffer = sfImage_create(64, 32);
	localDis.local_texture = sfTexture_create(64, 32);
	localDis.local_sprite = sfSprite_create();

	sfSprite_setTexture(localDis.local_sprite, localDis.local_texture, sfTrue);
	Display_clear(&localDis);

	return localDis;
}
void Display_destroy(Display* pm_display) {
	sfImage_destroy(pm_display->buffer);
	sfTexture_destroy(pm_display->local_texture);
	sfSprite_destroy(pm_display->local_sprite);
}
