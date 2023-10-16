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
        std::vector<VertexMesh> verticesList;
        std::vector<unsigned int> indicesList;
        
        size_t indexOffset = 0;
        verticesList.reserve(shapes[s].mesh.indices.size());
        indicesList.reserve(shapes[s].mesh.indices.size());
        
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t numFaceVerts = shapes[s].mesh.num_face_vertices[f];

            for(size_t v = 0; v < numFaceVerts; v++)
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

        ShapeData data = {};
        data.VerticesSize = verticesList.size();
        data.IndexesSize = indicesList.size();
        GraphicsEngine::Get()->CreateBuffers(data, verticesList, indicesList);
        m_shapesData.emplace_back(data);
    }
}
    
}
