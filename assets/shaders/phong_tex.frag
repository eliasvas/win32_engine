#version 330 core
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct Light {
	vec3 direction;
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 FragColor;
  
in vec3 f_normal;
in vec3 w_frag_pos;
in vec2 f_texcoords;

uniform vec3 view_pos; //camera position
uniform Material m;
uniform Light light;


void main()
{
	vec3 ambient = light.diffuse * vec3(texture(m.diffuse, f_texcoords));
	
	vec3 norm = normalize(f_normal);
	vec3 light_dir = normalize(light.position - w_frag_pos);
	//vec3 light_dir = normalize(-light.direction); //for directional light
	
	float diff = max(dot(norm,light_dir),0.0);
	vec3 diffuse =light.diffuse * diff * vec3(texture(m.diffuse, f_texcoords));
	
	vec3 view_dir = normalize(view_pos - w_frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), m.shininess);
	
	vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
	
    FragColor = vec4((diffuse + ambient + specular), 1.0);
} 

