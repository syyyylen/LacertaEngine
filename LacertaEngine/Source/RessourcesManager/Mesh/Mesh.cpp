#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../Libs/TinyObjLoader/include/tiny_obj_loader.h"
#include <locale>
#include <codecvt>

#include "../../Logger/Logger.h"
#include "../../Rendering/GraphicsEngine.h"

namespace LacertaEngine
{
    
Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateResource(const wchar_t* filePath)
{
    SetFilePath(filePath);

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string error;
    
    int utf8StrLen = WideCharToMultiByte(CP_UTF8, 0, filePath, -1, nullptr, 0, nullptr, nullptr);
    std::string inputfile;
    WideCharToMultiByte(CP_UTF8, 0, filePath, -1, &inputfile[0], utf8StrLen, nullptr, nullptr);

    bool res = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &error, inputfile.c_str());

    if(!error.empty() || !res)
    {
        LOG(Error, "Mesh failed to load : ");
        throw std::exception("Mesh not loaded successfully");
    }

    if(shapes.size() > 1)
    {
        LOG(Error, "Mesh has multiple shapes!");
        throw std::exception("Mesh has multiple shapes!");
    }

    std::vector<VertexMesh> verticesList;
    std::vector<unsigned int> indicesList;

    for(size_t s = 0; s < shapes.size(); s++)
    {
        size_t indexOffset = 0;
        verticesList.reserve(shapes[s].mesh.indices.size());
        indicesList.reserve(shapes[s].mesh.indices.size());
        
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            unsigned char numFaceVerts = shapes[s].mesh.num_face_vertices[f];

            for(unsigned char v = 0; v < numFaceVerts; v++)
            {
                tinyobj::index_t index = shapes[s].mesh.indices[indexOffset + v];
                
                tinyobj::real_t vx = attributes.vertices[index.vertex_index * 3 + 0];
                tinyobj::real_t vy = attributes.vertices[index.vertex_index * 3 + 1];
                tinyobj::real_t vz = attributes.vertices[index.vertex_index * 3 + 2];
                
                tinyobj::real_t tx = attributes.texcoords[index.texcoord_index * 2 + 0];
                tinyobj::real_t ty = attributes.texcoords[index.texcoord_index * 2 + 1];

                tinyobj::real_t nx = attributes.normals[index.normal_index * 3 + 0];
                tinyobj::real_t ny = attributes.normals[index.normal_index * 3 + 1];
                tinyobj::real_t nz = attributes.normals[index.normal_index * 3 + 2];

                VertexMesh vertex;
                vertex.Position = Vector3(vx, vy, vz);
                vertex.Texcoord = Vector2(tx, ty);
                vertex.Normal = Vector3(nx, ny, nz);
                verticesList.push_back(vertex);

                indicesList.push_back((unsigned int)indexOffset + v);
            }

            indexOffset += numFaceVerts;
        }
    }

    m_verticesSize = verticesList.size();
    m_indexesSize = indicesList.size();
    GraphicsEngine::Get()->CreateBuffers(this, verticesList, indicesList);
}
    
}
