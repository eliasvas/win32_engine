#include "Renderer.h"

void* get_array_buffer_ptr()
{
    return glMapBuffer(GL_ARRAY_BUFFER,GL_MAP_READ_BIT);
}

static void 
init_renderer(Renderer* rend)
{
    rend->tex_count = 0;
    rend->point_light_count = 0;
    rend-> dir_light_count = 0;
   
    init_shadowmap_fbo(&rend->shadowmap);
    {//initializing renderable VAO
        glGenVertexArrays(1, &rend->renderable_vao);    
        glBindVertexArray(rend->renderable_vao);
        glGenBuffers(1, &rend->renderable_instance_vbo);
        glGenBuffers(1,&rend->vertex_vbo);

        //binding per-vertex vbo
        glBindBuffer(GL_ARRAY_BUFFER, rend->vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        //vertex positions (per-vertex)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), (GLvoid*)0);
        //vertex texture-coordinates (per-vertex)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)(0));



        //binding per-instance vbo
        glBindBuffer(GL_ARRAY_BUFFER,rend->renderable_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rend->renderable_instance_data),  rend->renderable_instance_data,GL_DYNAMIC_DRAW);
        //position (per-instance)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Quad2D), (GLvoid*)0);
        glVertexAttribDivisor(2, 1);
        //scale (per-instance)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Quad2D), (GLvoid*)(2* sizeof(GLfloat)));
        glVertexAttribDivisor(3, 1);
        //texture unit (per-instance)
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Quad2D), (GLvoid*)(4 * sizeof(GLfloat)));
        glVertexAttribDivisor(4, 1);
        //opacity (per-instance)
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Quad2D), (GLvoid*)(5 * sizeof(GLfloat)));
        glVertexAttribDivisor(5, 1);
        //bottom_left coord
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Quad2D), (GLvoid*)(6 * sizeof(GLfloat)));
        glVertexAttribDivisor(6, 1);
        //tex_dim
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Quad2D), (GLvoid*)(8 * sizeof(GLfloat)));
        glVertexAttribDivisor(7, 1);
        //flip
        glEnableVertexAttribArray(8);
        glVertexAttribIPointer(8, 1, GL_UNSIGNED_INT, sizeof(Quad2D), (GLvoid*)(10 * sizeof(GLfloat)));
        glVertexAttribDivisor(8, 1);






        glBindVertexArray(0);
    }

    //this is for testing purposes.. make it normal..
    shader_load(&rend->shaders[0], "../assets/shaders/batch.vert", "../assets/shaders/batch.frag");
    shader_load(&rend->shaders[1],"../assets/shaders/batch_phong_tex_sm.vert", "../assets/shaders/batch_phong_tex_sm.frag");

    //TODO make texture loading dynamic!!!!!!!!!! <---
    load_texture(&rend->tex[1],"../assets/panda.png");
    rend->tex_count++;
    load_texture(&rend->tex[2],"../assets/runimation.png");
    rend->tex_count++;
    load_texture(&rend->tex[3],"../assets/braid.png");
    rend->tex_count++;
    load_texture(&rend->tex[4],"../assets/pandasheet.png");
    rend->tex_count++;
    load_texture(&rend->tex[5],"../assets/red.png");
    rend->tex_count++;
    load_texture(&rend->tex[6],"../assets/white.png");
    rend->tex_count++;

}

static void
renderer_render_scene(Renderer* rend,float* proj, Shader* shader_to_render_3d)
{
    vec2 texture_sizes[TEXTURE_MAX];

    glBindVertexArray(rend->renderable_vao);
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->renderable_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->renderable_instance_data), rend->renderable_instance_data, GL_STATIC_DRAW);


    for (i32 i = 0; i< rend->tex_count;++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, rend->tex[i].id);
        texture_sizes[i] = {(GLfloat)rend->tex[i].width, (GLfloat)rend->tex[i].height};
    }

    use_shader(&rend->shaders[0]);
    setMat4fv(&rend->shaders[0],"projection_matrix",proj);

    //passing the available tex_units as uniform
    GLuint loc = glGetUniformLocation(rend->shaders[0].ID, "slots");
    GLint arr[10] = {0,1,2,3,4,5,6,7,8,9};
    glUniform1iv(loc, 10,arr);

    //passing the dimensions of the available tex_units as uniform
    loc = glGetUniformLocation(rend->shaders[0].ID, "tex_sizes");
    glUniform2fv(loc, TEXTURE_MAX, (GLfloat*)texture_sizes); 

    loc = glGetUniformLocation(rend->shaders[0].ID, "iTime");
    glUniform1f(loc, global_platform.current_time);


    //glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->renderable_alloc_pos); // 10 diamonds, 4 vertices per instance
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->renderable_alloc_pos); // 10 diamonds, 4 vertices per instance
    glBindVertexArray(0);

    


    //NOTE(ilias):drawing models here!!!!
    use_shader(shader_to_render_3d);
    setMat4fv(&rend->shaders[1],"proj", (f32*)rend->perspective_projection.elements);
    setMat4fv(&rend->shaders[1],"view", (f32*)rend->view_matrix.elements);
    //setting all the directional lights
    //MEMORY lEAK --TODO fix
    char dir_attr[4][64] = {
        "dir_lights[x].ambient",
        "dir_lights[x].diffuse",
        "dir_lights[x].specular",
        "dir_lights[x].direction",
    };
    char point_attr[7][64] = {
        "point_lights[x].ambient",
        "point_lights[x].diffuse",
        "point_lights[x].specular",
        "point_lights[x].position",

        "point_lights[x].constant",
        "point_lights[x].linear",
        "point_lights[x].quadratic",
    };
    for(int i = 0; i < rend->dir_light_count; ++i)
    {
        //NOTE(ilias): this shouls also be an array in the glsl shader
        dir_attr[0][11] = '0'+i;
        dir_attr[1][11] = '0'+i;
        dir_attr[2][11] = '0'+i;
        dir_attr[3][11] = '0'+i;
        
        setVec3(&rend->shaders[1],dir_attr[0], rend->dir_lights[i].ambient);
        setVec3(&rend->shaders[1],dir_attr[1], rend->dir_lights[i].diffuse);
        setVec3(&rend->shaders[1],dir_attr[2], rend->dir_lights[i].specular);
        setVec3(&rend->shaders[1],dir_attr[3], rend->dir_lights[i].direction);
    }
    for(int i = 0; i < rend->point_light_count; ++i)
    {
        point_attr[0][13] = '0' + i;
        point_attr[1][13] = '0' + i;
        point_attr[2][13] = '0' + i;
        point_attr[3][13] = '0' + i;
        point_attr[4][13] = '0' + i;
        point_attr[5][13] = '0' + i;
        point_attr[6][13] = '0' + i;


        setVec3(&rend->shaders[1],point_attr[0], rend->point_lights[i].ambient);
        setVec3(&rend->shaders[1],point_attr[1], rend->point_lights[i].diffuse);
        setVec3(&rend->shaders[1],point_attr[2], rend->point_lights[i].specular);
        setVec3(&rend->shaders[1],point_attr[3], rend->point_lights[i].position);
        setFloat(&rend->shaders[1],dir_attr[4], rend->point_lights[i].constant);
        setFloat(&rend->shaders[1],dir_attr[5], rend->point_lights[i].linear);
        setFloat(&rend->shaders[1],dir_attr[6], rend->point_lights[i].quadratic);

    }

    for (u32 i = 0; i < rend->mesh_count;++i)
    {
        setMat4fv(&rend->shaders[1], "model", (f32*)rend->meshes[i].model_matrix.elements);
        setMat4fv(shader_to_render_3d, "model", (f32*)rend->meshes[i].model_matrix.elements);

        {
       
            //diffuse and spec should be samplers to textures
            setVec3(&rend->shaders[1],"m.ambient", {0.2f, 0.2f, 0.2f});
            setFloat(&rend->shaders[1], "m.shininess", 16.f);
            glActiveTexture(GL_TEXTURE0);
            setInt(&rend->shaders[1],"m.diffuse", 0);
            glBindTexture(GL_TEXTURE_2D, rend->tex[5].id);
            setInt(&rend->shaders[1],"m.specular", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, rend->tex[6].id);

            //TextureManager^^^
        }
        setInt(&rend->shaders[1], "point_light_count", rend->point_light_count); 
        setInt(&rend->shaders[1], "dir_light_count", rend->dir_light_count); 
        setMat4fv(&rend->shaders[1], "lightSpaceMatrix", (f32*)rend->shadowmap.lightSpaceMatrix.elements);
        setMat4fv(shader_to_render_3d, "lightSpaceMatrix", (f32*)rend->shadowmap.lightSpaceMatrix.elements);

        setVec3(&rend->shaders[1],"view_pos", rend->camera_pos);
        

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, rend->shadowmap.depth_attachment);
        setInt(&rend->shaders[1], "shadowMap", 2);

        glBindVertexArray(rend->meshes[i].vao);
        if (rend->meshes[i].indexed)
            glDrawElements(GL_TRIANGLES, rend->meshes[i].count, GL_UNSIGNED_INT, 0);
        else    
            glDrawArrays(GL_TRIANGLES,0, rend->meshes[i].count);

    }

    glBindVertexArray(0);

}
static void
renderer_render(Renderer* rend,float* proj)
{
    GLint prev_fbo; //its always a 0 -but who am I to judge?
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    setup_shadowmap(&rend->shadowmap, rend->view_matrix);
    glBindFramebuffer(GL_FRAMEBUFFER, rend->shadowmap.fbo);
    renderer_render_scene(rend, proj, &rend->shadowmap.s);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0, 0, global_platform.window_width,global_platform.window_height); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setup_debug_quad(&rend->debug_quad, &rend->shadowmap);
    renderer_render_scene(rend, proj, &rend->shaders[1]);
}


static void
renderer_begin(Renderer* rend, i32 w, i32 h)
{
    rend->render_width = w;
    rend->render_height = h;
    rend->rect_alloc_pos = 0;
    rend->renderable_alloc_pos = 0;
    rend->dir_light_count = 0;
    rend->point_light_count = 0;
    rend->mesh_count = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    //glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height)); //for some reason this is the only viewport called
    //glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height*(w/(float)h))); //this is the holy saviour???
}

static void
renderer_push(Renderer* rend, vec2 offset,vec2 scale, GLuint unit, GLuint flip = 0)
{

    Quad2D to_add = {offset,scale, unit,1.0f,{0.0f,0.0f},{1.0f,1.0f}, flip};
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}


static void
renderer_push(Renderer* rend, vec2 offset,vec2 scale, GLuint unit, vec2 bl, vec2 dim, GLuint flip = 0)
{
    Quad2D to_add = {offset,scale, unit,1.0f,bl,dim, flip};
    //rend->renderable_instance_data[rend->renderable_alloc_pos/sizeof(Quad2D)] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}

static void
renderer_push(Renderer* rend, vec2 offset,vec2 scale, GLuint unit, vec2 bl, vec2 dim, GLuint flip, f32 opacity)
{
    Quad2D to_add = {offset,scale, unit,opacity,bl,dim, flip};
    //rend->renderable_instance_data[rend->renderable_alloc_pos/sizeof(Quad2D)] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}

static void
renderer_push_dir_light(Renderer* rend,DirLight* light)
{
    rend->dir_lights[rend->dir_light_count++] = *light;
}

static void
renderer_push_dir_light_info(Renderer* rend,vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular)
{
    DirLight light;
    light.direction = direction;
    light.ambient = ambient;
    light.diffuse = diffuse;
    light.specular = specular;

    rend->dir_lights[rend->dir_light_count++] = light;
}



static void
renderer_push_point_light(Renderer* rend,PointLight* light)
{
    rend->point_lights[rend->point_light_count++] = *light;
}

static void
renderer_push_point_light_info(Renderer* rend,vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
{
    PointLight light;
    light.ambient = ambient;
    light.diffuse = diffuse;
    light.specular = specular;
    light.position = position;
    light.constant = 1.f;
    light.linear = 0.022f;
    light.quadratic = 0.0019f;

    rend->point_lights[rend->point_light_count++] = light;
}


static void 
renderer_push_mesh(Renderer* rend,Model* model, i32 triangle_count, b32 indexed = 0)
{
    ModelInfo info;
    info.vao = model->vao;
    mat4 model_mat = translate_mat4(model->position);
    model_mat.elements[0][0] = 0.05f;//model->scale.x; 
    model_mat.elements[1][1] = 0.05f;//model->scale.y; 
    model_mat.elements[2][2] = 0.05f;//model->scale.z; 
    //TODO(ilias): also add rotation info..
    info.model_matrix = model_mat;
    info.count = triangle_count;
    info.indexed = indexed;

    rend->meshes[rend->mesh_count++] = info;
}

static void 
renderer_push_mesh_vao(Renderer* rend,GLuint vao, mat4 model, i32 triangle_count, b32 indexed = 0)
{
   ModelInfo info = {vao, model, triangle_count, indexed};     

   rend->meshes[rend->mesh_count++] = info;
}

static void 
renderer_set_projection_matrix(Renderer* rend, mat4 projection)
{
    rend->perspective_projection = projection;
   
}

static void renderer_set_view_matrix(Renderer* rend, mat4 view)
{
    rend->view_matrix = view;
}

static void 
renderer_set_ortho_matrix(Renderer* rend, mat4 ortho)
{
    rend->orthographic_projection = ortho;    
}

static void
renderer_set_camera_pos(Renderer* rend, vec3 pos)
{
    rend->camera_pos = pos;
}

