#include "WinDXUtilities.h"
#include "../Logger/Logger.h"
#include <d3dcompiler.h>

namespace LacertaEngine
{
    
bool WinDXUtilities::CompileShader(LPCWSTR VSFilePath, LPCWSTR PSFilePath, ShaderBlobs& outBlobs)
{
    ID3DBlob* vertexErrorBlob = nullptr;
    ID3DBlob* vertexBlob;

    HRESULT hr = D3DCompileFromFile(VSFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertexBlob, &vertexErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDXUtilities : Failed vertex shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (vertexErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(vertexErrorBlob->GetBufferPointer()), vertexErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            vertexErrorBlob->Release();
        }
        return false;
    }

    outBlobs.VertexBlob = vertexBlob;

    ID3DBlob* pixelErrorBlob = nullptr;
    ID3DBlob* pixelBlob;

    hr = D3DCompileFromFile(PSFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pixelBlob, &pixelErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDXUtilities : Failed pixel shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (pixelErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(pixelErrorBlob->GetBufferPointer()), pixelErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            pixelErrorBlob->Release();
        }
        return false;
    }

    outBlobs.PixelBlob = pixelBlob;
    
    return true;
}

}
