#pragma once

#ifdef LE_BUILD_DLL
	#define LACERTAENGINE_API __declspec(dllexport)
#else
	#define LACERTAENGINE_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <windows.h>