#include "mesh.h"

Mesh::Mesh(renderer::VertexList vertices, renderer::VertexFormat vertexFormat, renderer::IndexList indices) :
    handle {renderer::uploadMesh(vertices, vertexFormat, indices) }
{
}

Mesh::~Mesh()
{
    renderer::deleteMesh(handle);
}

void Mesh::draw() const
{
    renderer::drawMesh(handle);
}
