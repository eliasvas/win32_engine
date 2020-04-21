#pragma once
#include "ext/tiny_obj_loader.h"
#include "ext/HandmadeMath.h"
#include "vector"
//TODO(ilias): make my own obj loader... 

struct vertex
{
   hmm_vec3 position; 
   hmm_vec3 normal;
   hmm_vec2 tex_coord;
};

struct model{
    GLuint vao;
    shader s;
    std::vector<vertex> vertices;
    hmm_vec4 position; //??
};

static void
load_model_data(std::vector<vertex>& vec, const char * obj_path, const char * mtl_path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;
    tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, obj_path, mtl_path);

    for (u32 i = 0; i < shapes.size(); ++i)
    {
        tinyobj::shape_t &shape = shapes[i];
        tinyobj::mesh_t &mesh = shape.mesh;
        for (u32 j = 0; j < mesh.indices.size();++j)
        {
            tinyobj::index_t i = mesh.indices[j];

            hmm_vec3 position = {attributes.vertices[i.vertex_index * 3], attributes.vertices[i.vertex_index * 3 + 1], attributes.vertices[i.vertex_index * 3 + 2]};
            hmm_vec3 normal = {attributes.vertices[i.normal_index * 3], attributes.vertices[i.normal_index * 3 + 1], attributes.vertices[i.normal_index * 3 + 2]};
            hmm_vec2 tex_coord = {attributes.vertices[i.texcoord_index * 2], attributes.vertices[i.texcoord_index * 2 + 1]};

            vertex v = {position, normal, tex_coord};
            vec.push_back(v);
        }
    }
}

static void 
init_model (model* m, std::vector<vertex>& vertices)
{
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
    glBindVertexArray(0);

    shader_load(&m->s,"../assets/shaders/mesh.vert","../assets/shaders/mesh.frag");
}

static void 
render_model(model* m, hmm_mat4 mvp)
{
    use_shader(&m->s);
    setMat4fv(&m->s, "MVP", (GLfloat*)mvp.Elements);
    glBindVertexArray(m->vao);
    glDrawArrays(GL_TRIANGLES, 0, m->vertices.size());
    glBindVertexArray(0);
}




