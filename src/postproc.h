#pragma once
#include "tb.h"

//TODO(ilias): make the API clean!!
static u32 fbo;
static u32 quad_vao;
static u32 color_buffer;
static u32 rbo;
static Shader postproc_shader;



f32 quad_verts[] = { //this boi fills the entire screen 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
#if 0
static void 
init_fake_framebuffer()
{
    //NOTE(ilias): maybe add all 4 color attachments?? 
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_platform.window_width, global_platform.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, global_platform.window_width, global_platform.window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0); //we dont need it to be bound just the ID
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        char* error_msg = "Framebuffer is not complete!";
        memcpy(infoLog, error_msg,strlen(error_msg));
    }

    shader_load (&postproc_shader,"../assets/shaders/postproc.vert","../assets/shaders/postproc.frag");
    u32 quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), &quad_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}
static void change_to_fake_framebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); //we draw the whole screen in the second framebuffer
    glViewport(0,0,global_platform.window_width, global_platform.window_height);
    glBindTexture(GL_TEXTURE_2D, color_buffer); //TODO(ilias): check performance of reallocating texture data!!!!!!!!
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_platform.window_width, global_platform.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glEnable(GL_DEPTH_TEST);
}

static void 
render_to_framebuffer0(f32 inverted = 0.f)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0,0,global_platform.window_width, global_platform.window_height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.f,1.f,1.f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    use_shader(&postproc_shader);
    glBindVertexArray(quad_vao);
    setInt(&postproc_shader,"screenTexture",0);
    setFloat(&postproc_shader, "flag", inverted);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffer); //we use the color attachment texture as texture for our quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

#endif
















static void 
init_fake_framebuffer()
{
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);
        glGenTextures(1, &color_buffer);
        glBindTexture(GL_TEXTURE_2D, color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_platform.window_width, global_platform.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glViewport(0, 0, global_platform.window_width, global_platform.window_height); //for some reason this is the only viewport called
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, global_platform.window_width, global_platform.window_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);




        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            char* error_msg = "Framebuffer is not complete!";
	        memcpy(infoLog, error_msg,strlen(error_msg));
        }

        shader_load (&postproc_shader,"../assets/shaders/postproc.vert","../assets/shaders/postproc.frag");
        u32 quad_vbo;
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), &quad_verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
}
static void change_to_fake_framebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); //we draw the whole screen in the second framebuffer
    glBindTexture(GL_TEXTURE_2D, color_buffer); //TODO(ilias): check performance of reallocating texture data!!!!!!!!
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_platform.window_width, global_platform.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, global_platform.window_width, global_platform.window_height);

    glViewport(0, 0,global_platform.window_width,global_platform.window_height); //for some reason this is the only viewport called
    glEnable(GL_DEPTH_TEST);
}

static void 
render_to_framebuffer0(f32 inverted = 0.f)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, global_platform.window_width, global_platform.window_height); //for some reason this is the only viewport called
    //glBindTexture(GL_TEXTURE_2D, color_buffer); //we use the color attachment texture as texture for our quad
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.1f,0.1f,0.1f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    use_shader(&postproc_shader);
    glBindVertexArray(quad_vao);
    setInt(&postproc_shader,"screenTexture",0);
    setFloat(&postproc_shader, "flag", inverted);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffer); //we use the color attachment texture as texture for our quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}
