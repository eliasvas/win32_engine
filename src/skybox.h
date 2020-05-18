#pragma once
#include "tb.h"
#include <vector>
#include <string>
#include <iostream>
#include <vector>


static u32 
load_cubemap(std::vector<std::string> faces)
{
    u32 textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //stbi_set_flip_vertically_on_load(true);
    i32 width, height, nrChannels;
    for (u32 i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


struct skybox {
	
	u32 vao;
	u32 vbo; //remove this boi
    u32 tex_id;
    std::vector<std::string> faces; //names??
    Shader shader;
};

static void 
load_skybox(skybox *skybox,const std::vector<std::string>& faces) {
    skybox->faces = faces;
    skybox->tex_id = load_cubemap(faces);
    //make a cubemap_texture 
    //attach the images
    // render
}

static void
init_skybox(skybox* skybox, std::vector<std::string>& faces) {
    shader_load(&skybox->shader,"../assets/shaders/skybox_rendering.vert", "../assets/shaders/skybox_rendering.frag");
    load_skybox(skybox,faces);
    f32 skybox_vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &skybox->vao);
    glGenBuffers(1, &skybox->vbo);
    glBindVertexArray(skybox->vao);
    glBindBuffer(GL_ARRAY_BUFFER, skybox->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);	
    glBindVertexArray(0);
}

static void 
render_skybox(skybox* skybox,mat4 projection, mat4 view) {
    use_shader(&skybox->shader);
    //this render must be done at the beginning of the scene rendering process
    //because we disabled depth testing and basically only the color buffer gets updated
    glDepthMask(GL_FALSE);
    glBindVertexArray(skybox->vao);
    setMat4fv(&skybox->shader, "uniform_projection_matrix", (float*)projection.elements);
    setMat4fv(&skybox->shader, "uniform_view_matrix", (float*)view.elements);
    //setMat4fv(&skybox->shader, "MVP", (float*)(mul_mat4(projection,view).elements));

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);
    glDrawArrays(GL_TRIANGLES, 0,36);
    glDepthMask(GL_TRUE);
}

