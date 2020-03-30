#pragma once
#include <vector> //what a disgrace!! NOTE(ilias): replace with fixed sized array
#include "ext/HandmadeMath.h"
#include "shader.h"
#include "texture.h"


struct bitmap_font
{
    GLuint VAO, vertex_buffer, uv_buffer;
    texture tex;
    shader s;
};

static void 
init_text(bitmap_font* f, const char * texture_path)
{
    glGenVertexArrays(1, &f->VAO);
    glGenBuffers(1, &f->vertex_buffer);
    glGenBuffers(1, &f->uv_buffer);

    shader_load(&f->s, "../assets/shaders/text.vert", "../assets/shaders/text.frag");
    load_texture(&f->tex,texture_path);
}

static void 
print_text(bitmap_font *f,const char*text, i32 x,i32 y, i32 size)
{

    u32 length = strlen(text);

	std::vector<hmm_vec2> vertices;
	std::vector<hmm_vec2> UVs;
    for (u32 i = 0; i < length; ++i)
    {
        /*
        For every character we need to render we have to find its screen coordinate
        positions, meaning its corners (since we are rendering a quad, and then pass it
        to our vertex buffer as 6 points, meaning 2 triangles so we can render
        (this is basically where each character goes in screen(?) space)
        */
        hmm_vec2 v_up_left = {(f32)x + i*size, (f32)y+size};
        hmm_vec2 v_up_right = {(f32)x + i*size + size, (f32)y+size};
        hmm_vec2 v_down_left = {(f32)x + i*size, (f32)y};
        hmm_vec2 v_down_right = {(f32)x + i*size + size, (f32)y};

        vertices.push_back(v_up_left);
        vertices.push_back(v_down_left);
        vertices.push_back(v_up_right);


        vertices.push_back(v_down_right);
        vertices.push_back(v_up_right);
        vertices.push_back(v_down_left);
        /*
         every UV is basically in range [0 1] and it denotes the coordinates to sample
         inside the texture, every (ASCII) character has uv_x = (c%16) /16.f and uv_y = (c/16)/16.f
         now, we must have the UV-coordinates of every letter in our texture atlas, and every letter
         in our atlas is composed of 4 points (it's a QUAD) so we need to find them:
        */
        char letter = text[i]; //NOTE(ilias): getting the last byte, which is its ASCII encoding value
        f32 uv_x = (letter % 16) / 16.f;
        f32 uv_y = (letter / 16) / 16.f;
        hmm_vec2 uv_down_left = {uv_x, uv_y};
        hmm_vec2 uv_down_right = {uv_x+1.0f/16.0f, uv_y};
        hmm_vec2 uv_up_right = {uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f)};
        hmm_vec2 uv_up_left = {uv_x, (uv_y + 1.0f/16.0f)};

        UVs.push_back(uv_up_left);
        UVs.push_back(uv_down_left);
        UVs.push_back(uv_up_right);


        UVs.push_back(uv_down_right);
        UVs.push_back(uv_up_right);
        UVs.push_back(uv_down_left);
    }
    glBindBuffer(GL_ARRAY_BUFFER, f->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(hmm_vec2), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, f->uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(hmm_vec2), &UVs[0], GL_STATIC_DRAW);


   
    use_shader(&f->s);

    setFloat(&f->s, "time", ((float)rand()) / (float)FLT_MAX);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f->tex.id); 

    glBindVertexArray(f->VAO);

    glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, f->vertex_buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, f->uv_buffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

static void 
cleanup_text(bitmap_font *f)
{

}
