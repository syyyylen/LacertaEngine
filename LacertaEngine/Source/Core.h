#pragma once

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifdef LE_BUILD_DLL
	#define LACERTAENGINE_API __declspec(dllexport)
#else
	#define LACERTAENGINE_API __declspec(dllimport)
#endif

#define USE_D3D12_RHI 1

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <list>
