#version 330

uniform mat4 reg_projection;
uniform mat4 reg_model;
uniform mat4 reg_view;
uniform vec3 reg_light_pos;
uniform mat4 reg_transpose_inverse_modelview;

in vec2 in_Position;
in vec2 in_Normal;

out vec2 texture_coord;

out vec3 f_v;
out vec3 f_l;
out vec3 f_n;

void main() {
	vec4 pos = reg_view * reg_model * vec4(in_Position.x, in_Position.y, 0.0, 1.0); 
	

	gl_Position = reg_projection * pos;

	f_v = normalize(-pos.xyz);
	f_l = normalize(reg_light_pos - pos.xyz);
	f_n = normalize((reg_transpose_inverse_modelview * vec4(vec3(in_Normal.x, 0.0, in_Normal.y), 1)).xyz); 

	texture_coord = in_Position.xy;
}
