#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include "tools.h"
#include "texture.h"
#include "shader.h"

#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT  1024

typedef struct ShadowMapFBO
{
    GLuint fbo;
    GLuint color_attachments[4];
    GLuint depth_attachment;
    u32 w,h;
    Shader s;

}ShadowMapFBO;

static void
init_shadowmap_fbo(ShadowMapFBO* shadowmap)
{
    //we generate a new framebuffer and make a texture that will represent depth 
    glGenFramebuffers(1, &shadowmap->fbo);
    glGenTextures(1, &shadowmap->depth_attachment);
    glBindTexture(GL_TEXTURE_2D, shadowmap->depth_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

    
    //we attach the depth texture as as a depth attachment for our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowmap->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap->depth_attachment, 0);
    glDrawBuffer(GL_NONE);//we explicitly state that we are not going to read or draw, so we
    glReadBuffer(GL_NONE);//don't need to bind any color attachment to our FBO
    shader_load (&shadowmap->s, "../assets/shaders/shadowmap.vert","../assets/shaders/shadowmap.frag");


    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}


static void
setup_shadowmap(ShadowMapFBO* shadowmap)
{
    //render to depth map
    glViewport(0,0,SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowmap->fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    f32 near_plane = 1.f;
    f32 far_plane = 9.f;
    mat4 light_projection = orthographic_proj(-10.f,10.f,-10.f,10.f, near_plane, far_plane);
    //mat4 light_view = look_at(vec3 eye, vec3 center, vec3 fake_up)
    mat4 light_view = look_at(v3(-2.f, 4.f, -1.f), v3(0.f,0.f,0.f), v3(0.f,1.f,0.f));
    mat4 lightSpaceMatrix = mul_mat4(light_projection, light_view); 
    //glBindFramebuffer(GL_FRAMEBUFFER,0);

    //render the scene as normal
    glViewport(0,0,global_platform.window_width, global_platform.window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ConfigureShaderAndMatrices
    use_shader(&shadowmap->s);
    setMat4fv(&shadowmap->s, "lightSpaceMatrix", (f32*)lightSpaceMatrix.elements);

    glBindTexture(GL_TEXTURE_2D, shadowmap->depth_attachment);
    //RenderScene()
}


#endif
