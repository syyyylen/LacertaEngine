#include "WinDX11Mesh.h"
#include "../Drawcall.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../Libs/TinyObjLoader/include/tiny_obj_loader.h"
#include <locale>

#include "WinDX11Renderer.h"

namespace LacertaEngine
{
    
WinDX11Mesh::WinDX11Mesh()
{
}

WinDX11Mesh::~WinDX11Mesh()
{
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
        std::vector<VertexMesh> verticesList;
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

                VertexMesh vertex;
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

        WinDX11ShapeData data = {};
        data.VerticesSize = verticesList.size();
        data.IndexesSize = indicesList.size();
        data.Vbo = driver->CreateVBO(verticesList);
        data.Ibo = driver->CreateIBO(indicesList);
        
        m_shapesData.emplace_back(data);
    }
}

const std::vector<ShapeData> WinDX11Mesh::GetShapesData() const
{
    std::vector<ShapeData> ShapesDataTemp;

    for(auto Shape : m_shapesData)
    {
        ShapeData S;
        S.Vbo = Shape.Vbo;
        S.Ibo = Shape.Ibo;
        S.VerticesSize = Shape.VerticesSize;
        S.IndexesSize = Shape.IndexesSize;

        ShapesDataTemp.emplace_back(S);
    }

    return ShapesDataTemp;
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
    
}

