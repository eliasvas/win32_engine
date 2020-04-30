#pragma once
#include "tb.h"

//TODO(ilias): make the API clean!!
static u32 fbo;
static u32 quad_vao;
static u32 color_buffer;
static Shader postproc_shader;



f32 quad_verts[] = { //this boi fills the entire screen 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
static void 
init_fake_framebuffer()
{
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &color_buffer);
        glBindTexture(GL_TEXTURE_2D, color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_platform.window_width, global_platform.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //GLuint depthrenderbuffer;
        //glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, global_platform.window_width, global_platform.window_height);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,1);
        glViewport(0, 0, global_platform.window_width, global_platform.window_height); //for some reason this is the only viewport called
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
        //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, DrawBuffers);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            char* error_msg = "Framebuffer is not complete!";
	        memcpy(infoLog, error_msg,strlen(error_msg));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  //we are binding the DEFAULT framebuffer 

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

