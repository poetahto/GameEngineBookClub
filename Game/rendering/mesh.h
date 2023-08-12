#ifndef MESH_H
#define MESH_H

#include "renderer.h"

struct Mesh
{
    Mesh(Renderer::VertexList vertices, Renderer::VertexFormat vertexFormat, Renderer::IndexList indices);
    void free();
    void draw(Renderer::DrawMode mode) const;
    Renderer::MeshHandle handle;

    static Mesh triangle();
    static Mesh quad();
};

#endif // MESH_H
