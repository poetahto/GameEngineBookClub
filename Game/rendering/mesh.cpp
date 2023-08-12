#include "mesh.h"

using namespace Renderer;

Mesh::Mesh(VertexList vertices, VertexFormat vertexFormat, IndexList indices) :
    handle{uploadMesh(vertices, vertexFormat, indices)}
{
}

void Mesh::free()
{
    deleteMesh(handle);
}


void Mesh::draw(DrawMode mode) const
{
    drawMesh(handle, mode);
}

Mesh Mesh::triangle()
{
    VertexList vertices
    {
        15,
        new f32[]
        {
            -0.25, 0, 0, 0, 0,
            0, 0.5, 0, 1, 1,
            0.25, 0, 0, 0, 1,
        }
    };

    IndexList indices
    {
        4,
        new u32[]{0, 1, 2, 0},
    };

    VertexFormat format
    {
        2,
        new s32[]{3,2},
    };

    return Mesh{vertices, format, indices};
}

Mesh Mesh::quad()
{
    VertexList vertices
    {
        20,
        new f32[]
        {
            -0.25, 0, 0, 0, 0,
            -0.25, 0.5, 0, 0, 1,
            0.25, 0.5, 0, 1, 1,
            0.25, 0, 0, 1, 0,
        }
    };

    IndexList indices
    {
        6,
        new u32[]{0, 1, 2, 2, 3, 0},
    };

    VertexFormat format
    {
        2,
        new s32[]{3,2},
    };

    return Mesh{vertices, format, indices};
}
