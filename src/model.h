#pragma once
#include "ext/tiny_obj_loader.h"
#include "ext/HandmadeMath.h"
#include "vector"
#include "objloader.h"

struct vertex
{
   vec3 position; 
   vec3 normal;
   vec2 tex_coord;
};
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    f32 shininess;
};
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct PointLight {    
    vec3 position;
    
    f32 constant;
    f32 linear;
    f32 quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct Model{
    GLuint vao;
    Shader s;
    std::vector<vertex> vertices;
    vec3 position;
    vec3 scale;
    Material m;
    Texture diff;
    Texture spec;
    //quaternion rotation
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

            vec3 position = {attributes.vertices[i.vertex_index * 3], attributes.vertices[i.vertex_index * 3 + 1], attributes.vertices[i.vertex_index * 3 + 2]};
            vec3 normal = {attributes.vertices[i.normal_index * 3], attributes.vertices[i.normal_index * 3 + 1], attributes.vertices[i.normal_index * 3 + 2]};
            vec2 tex_coord = {attributes.vertices[i.texcoord_index * 2], attributes.vertices[i.texcoord_index * 2 + 1]};

            vertex v = {position, normal, tex_coord};
            vec.push_back(v);
        }
    }
}
static void
load_model_dataOG(std::vector<vertex>& vec, const char * obj_path, const char *mtl_path)
{
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> uvs;
    load_obj(vertices, normals, uvs, obj_path);
    for (int i = 0; i < vertices.size(); ++i)
    {
        vec.push_back({vertices[i], normals[i], uvs[i]});
    }
}

static void 
init_model_textured(Model* m, std::vector<vertex>& vertices)
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

    shader_load(&m->s,"../assets/shaders/phong_tex.vert","../assets/shaders/phong_tex.frag");
    load_texture(&(m->diff),"../assets/corona.png");
    load_texture(&(m->spec),"../assets/noise.png");
      
}

static void 
render_model_textured(Model* m, mat4* projection, mat4* view, vec3 light_pos, vec3 camera_pos)
{
    use_shader(&m->s);
    
    setInt(&m->s, "m.diffuse", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m->diff.id);
    setInt(&m->s, "m.specular", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m->spec.id);

    Light light = {light_pos,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};
    mat4 model = translate_mat4(m->position);
    model.elements[0][0] =0.1f;
    model.elements[1][1] =0.1f;
    model.elements[2][2] =0.1f;
    setMat4fv(&m->s, "proj", (GLfloat*)projection->elements);
    setMat4fv(&m->s, "view", (GLfloat*)view->elements);
    setMat4fv(&m->s, "model", (GLfloat*)model.elements);
    {
        setVec3(&m->s,"m.ambient", {0.2f, 0.2f, 0.2f});
        setVec3(&m->s,"m.diffuse", {0.7f, 0.3f, 0.2f});
        setVec3(&m->s,"m.specular", {0.1f, 0.1f, 0.1f});
        setFloat(&m->s, "m.shininess", 3.f);
    }
    {
        setVec3(&m->s,"light.ambient", light.ambient);
        setVec3(&m->s,"light.direction", {0,1,0});
        setVec3(&m->s,"light.diffuse", light.diffuse);
        setVec3(&m->s,"light.specular", light.specular);
        glUniform3f(glGetUniformLocation(m->s.ID, "light.position"), light_pos.x, light_pos.y, light_pos.z); 
    }

    glUniform3f(glGetUniformLocation(m->s.ID, "view_pos"), camera_pos.x, camera_pos.y, camera_pos.z); 

    glBindVertexArray(m->vao);
    //int mode = (int)(global_platform.current_time) % 3;
    //if (mode == 2)mode = GL_TRIANGLES;
    //glDrawArrays( mode,0, m->vertices.size());
    glDrawArrays(GL_TRIANGLES,0, m->vertices.size());
    glBindVertexArray(0);
}











//DEPRECATED, ONLY USE FOR TESTING!!
static void 
init_model (Model* m, std::vector<vertex>& vertices)
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

    shader_load(&m->s,"../assets/shaders/phong.vert","../assets/shaders/phong.frag");
}

static void 
render_model(Model* m, mat4* projection, mat4* view, vec3 light_pos, vec3 camera_pos)
{
    use_shader(&m->s);
    Light light = {light_pos,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};
    mat4 model = translate_mat4(m->position);
    model.elements[0][0] =0.1f;
    model.elements[1][1] =0.1f;
    model.elements[2][2] =0.1f;
    setMat4fv(&m->s, "proj", (GLfloat*)projection->elements);
    setMat4fv(&m->s, "view", (GLfloat*)view->elements);
    setMat4fv(&m->s, "model", (GLfloat*)model.elements);
    {
        setVec3(&m->s,"m.ambient", {0.2f, 0.2f, 0.2f});
        setVec3(&m->s,"m.diffuse", {0.7f, 0.3f, 0.2f});
        setVec3(&m->s,"m.specular", {0.1f, 0.1f, 0.1f});
        setFloat(&m->s, "m.shininess", 32.f);
    }
    {
        setVec3(&m->s,"light.ambient", light.ambient);
        setVec3(&m->s,"light.diffuse", light.diffuse);
        setVec3(&m->s,"light.direction", {-0.2,-1.0,-0.3});
        setVec3(&m->s,"light.specular", light.specular);
        glUniform3f(glGetUniformLocation(m->s.ID, "light.position"), light_pos.x, light_pos.y, light_pos.z); 
    }

    glUniform3f(glGetUniformLocation(m->s.ID, "view_pos"), camera_pos.x, camera_pos.y, camera_pos.z); 

    glBindVertexArray(m->vao);
    //int mode = (int)(global_platform.current_time) % 3;
    //if (mode == 2)mode = GL_TRIANGLES;
    //glDrawArrays( mode,0, m->vertices.size());
    glDrawArrays(GL_TRIANGLES,0, m->vertices.size());
    glBindVertexArray(0);
}



