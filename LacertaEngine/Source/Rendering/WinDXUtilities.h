#pragma once
#include <d3dcommon.h>
#include "../Core.h"

namespace LacertaEngine
{

enum class ShaderType
{
    Vertex,
    Pixel,
    Compute
};

struct ShaderBlobs
{
    ID3DBlob* VertexBlob;
    ID3DBlob* PixelBlob;
};

struct CompiledShader
{
    ShaderType Type;
    std::vector<uint32_t> Bytecode;
};
    
class LACERTAENGINE_API WinDXUtilities
{
public:
    static bool CompileShader(LPCWSTR VSFilePath, LPCWSTR PSFilePath, ShaderBlobs& outBlobs);
    static void CompileShaderWithReflection(const std::string& path, ShaderType type, CompiledShader& outShader);
};

}