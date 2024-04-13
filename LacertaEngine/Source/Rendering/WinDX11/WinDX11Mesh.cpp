﻿#include "WinDX11Mesh.h"
#include "../Drawcall.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../Libs/TinyObjLoader/include/tiny_obj_loader.h"
#include <locale>

#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX11Shape::WinDX11Shape(ID3D11Buffer* vbo, unsigned long verticesSize, ID3D11Buffer* ibo, unsigned long indexesSize)
    : m_vbo(vbo), m_verticesSize(verticesSize), m_ibo(ibo), m_indexesSize(indexesSize)
{
}

WinDX11Shape::~WinDX11Shape()
{
    m_vbo->Release();
    m_ibo->Release();
}

void WinDX11Shape::BindBuffers(Renderer* renderer)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    unsigned vertexLayoutStride = sizeof(SceneVertexMesh); // TODO why is it here ? 
    unsigned int offsets = 0;
    
    ctx->IASetVertexBuffers(0, 1, &m_vbo, &vertexLayoutStride, &offsets);
    ctx->IASetIndexBuffer(m_ibo, DXGI_FORMAT_R32_UINT, 0);
}

WinDX11Mesh::WinDX11Mesh()
{
}

WinDX11Mesh::~WinDX11Mesh()
{
    for(auto shape : m_shapesData)
        delete shape;
}

void WinDX11Mesh::CreateResource(const wchar_t* filePath, Renderer* renderer)
{
    SetFilePath(filePath);

    int utf8StrLen = WideCharToMultiByte(CP_UTF8, 0, filePath, -1, nullptr, 0, nullptr, nullptr);
    std::string inputfile(utf8StrLen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, filePath, -1, &inputfile[0], utf8StrLen, nullptr, nullptr);
    
    tinyobj::ObjReaderConfig reader_config;
    // reader_config.mtl_search_path = "./"; // Path to material files
    
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return;
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attributes = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    for(size_t s = 0; s < shapes.size(); s++)
    {
        std::vector<SceneVertexMesh> verticesList;
        std::vector<unsigned int> indicesList;
        
        size_t indexOffset = 0;
        verticesList.reserve(shapes[s].mesh.indices.size());
        indicesList.reserve(shapes[s].mesh.indices.size());
        
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t numFaceVerts = shapes[s].mesh.num_face_vertices[f];

            Vector3 verticesFace[3];
            Vector2 texcoordsFace[3];

            for(size_t v = 0; v < numFaceVerts; v++)
            {
                tinyobj::index_t index = shapes[s].mesh.indices[indexOffset + v];
                
                tinyobj::real_t vx = attributes.vertices[index.vertex_index * 3 + 0];
                tinyobj::real_t vy = attributes.vertices[index.vertex_index * 3 + 1];
                tinyobj::real_t vz = attributes.vertices[index.vertex_index * 3 + 2];

                tinyobj::real_t tx = 0;
                tinyobj::real_t ty = 0;
                if(!attributes.texcoords.empty())
                {
                    tx = attributes.texcoords[index.texcoord_index * 2 + 0];
                    ty = attributes.texcoords[index.texcoord_index * 2 + 1];
                }

                verticesFace[v] = Vector3(vx, vy, vz);
                texcoordsFace[v] = Vector2(tx, ty);
            }

            Vector3 tangent, binormal;
            ComputeTangents(verticesFace[0], verticesFace[1], verticesFace[2],
                texcoordsFace[0], texcoordsFace[1], texcoordsFace[2],
                tangent, binormal);

            for(size_t v = 0; v < numFaceVerts; v++)
            {
                tinyobj::index_t index = shapes[s].mesh.indices[indexOffset + v];
                
                tinyobj::real_t vx = attributes.vertices[index.vertex_index * 3 + 0];
                tinyobj::real_t vy = attributes.vertices[index.vertex_index * 3 + 1];
                tinyobj::real_t vz = attributes.vertices[index.vertex_index * 3 + 2];

                tinyobj::real_t tx = 0;
                tinyobj::real_t ty = 0;
                if(!attributes.texcoords.empty())
                {
                    tx = attributes.texcoords[index.texcoord_index * 2 + 0];
                    ty = attributes.texcoords[index.texcoord_index * 2 + 1];
                }

                tinyobj::real_t nx = attributes.normals[index.normal_index * 3 + 0];
                tinyobj::real_t ny = attributes.normals[index.normal_index * 3 + 1];
                tinyobj::real_t nz = attributes.normals[index.normal_index * 3 + 2];

                Vector3 v_tangent, v_binormal;
                v_binormal = Vector3::Cross(Vector3(nx, ny, nz), tangent);
                v_tangent = Vector3::Cross(v_binormal, Vector3(nx, ny, nz));

                SceneVertexMesh vertex;
                vertex.Position = Vector3(vx, vy, vz);
                vertex.Texcoord = Vector2(tx, ty);
                vertex.Normal = Vector3(nx, ny, nz);
                vertex.Tangent = v_tangent;
                vertex.Binormal = v_binormal;
                verticesList.push_back(vertex);

                indicesList.push_back((unsigned int)indexOffset + v);
            }

            indexOffset += numFaceVerts;
        }

        WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
        WinDX11Shape* Shape = new WinDX11Shape(driver->CreateVBO(verticesList), verticesList.size(), driver->CreateIBO(indicesList), indicesList.size());
        
        m_shapesData.emplace_back(Shape);
    }
}

void WinDX11Mesh::ComputeTangents(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector2 t0, const Vector2 t1, const Vector2 t2, Vector3& tangent, Vector3& binormal)
{
    Vector3 deltaPos1 = v1 - v0;
    Vector3 deltaPos2 = v2 - v0;

    Vector2 deltaUV1 = t1 - t0;
    Vector2 deltaUV2 = t2 - t0;

    float r = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV1.Y * deltaUV2.X);
    tangent = Vector3::Normalize(deltaPos1 * deltaUV2.Y - deltaPos2 * deltaUV1.Y);
    binormal = Vector3::Normalize(deltaPos2 * deltaUV1.X - deltaPos1 * deltaUV2.X);
}

void WinDX11Mesh::ImportMesh(std::string filePath, Renderer* renderer)
{
    Assimp::Importer importer;
    std::string str = "Assets/Meshes/SciFiHelmet.gltf";
    const aiScene* scene = importer.ReadFile(str, aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(Error, "Failed assimp import");
        return;
    }
    
    ProcessNode(renderer, scene->mRootNode, scene);
}

void WinDX11Mesh::ProcessPrimitive(Renderer* renderer, aiMesh* mesh, const aiScene* scene)
{
    std::vector<SceneVertexMesh> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        SceneVertexMesh vertex;

        vertex.Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        if (mesh->HasNormals())
        {
            vertex.Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.Binormal = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        
        if (mesh->mTextureCoords[0])
            vertex.Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        
        vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    WinDX11Shape* Shape = new WinDX11Shape(driver->CreateVBO(vertices), vertices.size(), driver->CreateIBO(indices), indices.size());
    m_shapesData.emplace_back(Shape);
}

void WinDX11Mesh::ProcessNode(Renderer* renderer, aiNode* node, const aiScene* scene)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessPrimitive(renderer, mesh, scene);
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(renderer, node->mChildren[i], scene);
    }
}

}

