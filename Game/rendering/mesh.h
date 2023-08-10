#ifndef MESH_H
#define MESH_H

#include "renderer.h"

struct Mesh
{
    Mesh(renderer::VertexList vertices, renderer::VertexFormat vertexFormat, renderer::IndexList indices);
    void free();
    void draw(renderer::DrawMode mode) const;
    renderer::MeshHandle handle;

    static Mesh triangle();
    static Mesh quad();
};

#endif // MESH_H
