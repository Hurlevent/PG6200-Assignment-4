#version 330

uniform vec3 n_normal;

layout(location = 0) out vec4 out_color;


void main(){
	out_color = vec4(n_normal, 1.0f);
}