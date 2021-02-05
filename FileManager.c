#include "FileManager.h"

u8 FileManager_loadFile(const u8* pm_path, u8* pm_Ram) {
	FILE* file;
	fopen_s(&file, pm_path, "rb");

	if (!file) { printf("---Error: Cant read file---"); return NULL; }

	// Get file's size
	fseek(file, 0, SEEK_END);
	fpos_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (fileSize > MAX_MEM_SIZE) { printf("---Error: File load, memory Overflow"); return NULL; }

	// Read the data and set it to buffer
	u8* buffer = (u8*)malloc(fileSize);
	if (buffer == NULL) { printf("---Error: Cant allocate memory"); return NULL; }
	fread(buffer, 1, fileSize, file);

	// Map the buffer to 0x200 on main memory
	for (u16 i = 0; i < fileSize; i++) {
		*(pm_Ram + i + 0x200) = *(buffer + i);
		//printf("0x%X: 0x%X\n", i, *(pm_Ram + 0x200 + i)); // _Debug
	}

	free(buffer);
	fclose(file);
	return 1;
}
