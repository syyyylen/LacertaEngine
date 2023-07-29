#include "LacertaEngine.h"

// TODO find a better solution 
// this include allow .h only files to be linked correctly DLL side
// If a .h only class isn't included anywhere in a cpp file DLL side then
// it'll only get compiled .exe side and cause a Linker error cause
// LACERTAENGINE_API will be defined as __declspec(dllimport)
// TODO find a better solution 
#include "Maths/Maths.h" 