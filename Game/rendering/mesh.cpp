#include "mesh.h"

using namespace renderer;

Mesh::Mesh(VertexList vertices, VertexFormat vertexFormat, IndexList indices) :
    handle {uploadMesh(vertices, vertexFormat, indices) }
{
}

void Mesh::free()
{
    deleteMesh(handle);
}

void Mesh::draw() const
{
    drawMesh(handle);
}

Mesh Mesh::triangle()
{
    VertexList vertices
    {
        9,
        new f32[]
        {
            -0.25, 0, 0,
             0, 0.5, 0,
             0.25, 0, 0,
        }
    };

    IndexList indices
    {
        4,
        new u32[] {0, 1, 2, 0},
    };

    VertexFormat format
    {
        1,
        new s32[]{3},
    };

    return Mesh{vertices, format, indices};

}
