#pragma once
#include "Core.h"

#define HR(hr) if (!SUCCEEDED(hr)) { std::string errorMsg = std::system_category().message(hr); LOG(Error, errorMsg); }