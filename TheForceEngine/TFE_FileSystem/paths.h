#pragma once
#include <TFE_System/types.h>

enum TFE_PathType
{
	PATH_PROGRAM = 0,		// Program folder
	PATH_PROGRAM_DATA,		// ProgramData/ on Windows - save temporary or cached data here.
	PATH_USER_DOCUMENTS,	// Users/username/Documents/ on Windows - save settings here.
	PATH_SOURCE_DATA,		// This is the location of the source data, such as maps, textures, etc.
	PATH_EMULATOR,			// Path to the dosbox exe (for the editor).
	PATH_MOD,				// Use this to reference mods.
	PATH_COUNT
};

// Use an extended MAX_PATH, the Windows value is actually too small for some valid paths.
#define TFE_MAX_PATH 1024

namespace TFE_Paths
{
	// Paths, such as the source data or mods.
	void setPath(TFE_PathType pathType, const char* path);

	// Platform specific user data path.
	bool setProgramDataPath(const char* append);
	bool setUserDocumentsPath(const char* append);
	// Platform specific executable path.
	bool setProgramPath();
	
	const char* getPath(TFE_PathType pathType);
	bool hasPath(TFE_PathType pathType);
	void appendPath(TFE_PathType pathType, const char* filename, char* path, size_t bufferLen = TFE_MAX_PATH);
}
