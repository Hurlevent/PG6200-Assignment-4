#version 330

uniform mat4 reg_projection;
uniform mat4 reg_model;
uniform mat4 reg_view;
uniform vec3 reg_light_pos;
uniform vec3 reg_normal;

in  vec2 in_Position;

//out vec3 ex_Color;
out vec2 texture_coord;

out vec3 f_v;
out vec3 f_l;
out vec3 f_n;

void main() {
	vec4 pos = reg_view * reg_model * vec4(in_Position.x, 0.0, in_Position.y, 1.0); // WTF?? HAN BYTTER Y OG Z
	

	gl_Position = reg_projection * pos;
	
	//ex_Color = vec3(0.5f, 0.5f, 1.0f);

	f_v = normalize(-pos.xyz);
	f_l = normalize(reg_light_pos - pos.xyz);
	f_n = normalize(reg_normal);

	texture_coord = in_Position.xy;
}
