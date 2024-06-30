#pragma once
#include <d3dcommon.h>
#include "../Core.h"

namespace LacertaEngine
{

struct ShaderBlobs
{
    ID3DBlob* VertexBlob;
    ID3DBlob* PixelBlob;
};
    
class LACERTAENGINE_API WinDXUtilities
{
public:
    static bool CompileShader(LPCWSTR VSFilePath, LPCWSTR PSFilePath, ShaderBlobs& outBlobs);
};

}