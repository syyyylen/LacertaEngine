#include "WinDXUtilities.h"
#include "../Logger/Logger.h"
#include <d3dcompiler.h>
#include <string>
#include <atlbase.h>
#include <dxcapi.h>
#include <wrl/client.h>

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

const char* GetProfileFromType(ShaderType type)
{
    switch (type) {
        case ShaderType::Vertex: {
                return "vs_6_6";
        }
        case ShaderType::Pixel: {
                return "ps_6_6";
        }
        case ShaderType::Compute: {
                return "cs_6_6";
        }
    }
    return "???";
}

void WinDXUtilities::CompileShaderWithReflection(const std::string& path, ShaderType type, CompiledShader& outShader)
{
    using namespace Microsoft::WRL;

    HANDLE handle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!handle)
    {
        LOG(Error, "ShaderCompiler : Shader file cannot be read !");
        return;
    }
    int size = ::GetFileSize(handle, nullptr);
    if (size == 0)
    {
        LOG(Error, "ShaderCompiler : Shader file has size 0 and cannot be read !");
        return;
    }
    int bytesRead = 0;
    char *buffer = new char[size + 1];
    ::ReadFile(handle, reinterpret_cast<LPVOID>(buffer), size, reinterpret_cast<LPDWORD>(&bytesRead), nullptr);
    buffer[size] = '\0';
    CloseHandle(handle);

    std::string str(buffer);

    wchar_t wideTarget[512];
    swprintf_s(wideTarget, 512, L"%hs", GetProfileFromType(type));

    std::string entryPoint = "main";
    wchar_t wideEntry[512];
    swprintf_s(wideEntry, 512, L"%hs", entryPoint.c_str());

    ComPtr<IDxcUtils> utilis;
    ComPtr<IDxcCompiler> compiler;
    if (!SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utilis))))
        LOG(Error, "ShaderCompiler : Dxc shader compilation failed !");

    if (!SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
        LOG(Error, "ShaderCompiler : Dxc shader compilation failed !");

    ComPtr<IDxcIncludeHandler> includeHandler;
    if (!SUCCEEDED(utilis->CreateDefaultIncludeHandler(&includeHandler)))
        LOG(Error, "ShaderCompiler : Dxc shader compilation failed !");

    ComPtr<IDxcBlobEncoding> sourceBlob;
    if (!SUCCEEDED(utilis->CreateBlob(str.c_str(), str.size(), 0, &sourceBlob)))
        LOG(Error, "ShaderCompiler : Dxc shader compilation failed !");

    LPCWSTR pArgs[] = {
        L"-Zs",
        L"-Fd",
        L"-Fre"
    };

    ComPtr<IDxcOperationResult> result;
    if (!SUCCEEDED(compiler->Compile(sourceBlob.Get(), L"Shader", wideEntry, wideTarget, pArgs, ARRAYSIZE(pArgs), nullptr, 0, includeHandler.Get(), &result))) {
        LOG(Error, "ShaderCompiler : Dxc shader compilation failed !");
    }

    ComPtr<IDxcBlobEncoding> errors;
    result->GetErrorBuffer(&errors);

    if (errors && errors->GetBufferSize() != 0)
    {
        ComPtr<IDxcBlobUtf8> pErrorsU8;
        errors->QueryInterface(IID_PPV_ARGS(&pErrorsU8));
        std::string error((char*)pErrorsU8->GetStringPointer());
        LOG(Error, error);
    }

    HRESULT status;
    result->GetStatus(&status);

    ComPtr<IDxcBlob> shaderBlob;
    result->GetResult(&shaderBlob);

    outShader.Type = type;
    outShader.Bytecode.resize(shaderBlob->GetBufferSize() / sizeof(uint32_t));
    memcpy(outShader.Bytecode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

    LOG(Debug, "ShaderCompiler : compiled : " + path);
}
    
}
