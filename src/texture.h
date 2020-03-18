#inlude "types.h"

typedef struct texture 
{
    GLuint id;
    i32 width;
    i32 height;
};

static texture load_texture(const char *filename)
{
    texture tex = {0};
    void* data = 0;
    u63 data_len = 0;
    //NOTE(ilias): we need a read_entire_file() method before proceeding
    if (load_entire_file(filename,&data,data_len))
    {
        tex = tex_init(data, data_len);    
        //free void* data
    }
    return tex;
}
