#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <stdlib.h>
#include <stdio.h>

#include "Types.h"

#define MAX_MEM_SIZE 0xFFF

u8 FileManager_loadFile(const u8* pm_path, u8* pm_Ram);

#endif // !FILEMANAGER_H
