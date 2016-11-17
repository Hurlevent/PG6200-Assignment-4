#version 330

uniform mat4 bump_projection;
uniform mat4 bump_model;
uniform mat4 bump_view;
uniform vec3 bump_light_pos;

in  vec2 in_Position;

out vec2 texture_coord;

out vec3 f_v;
out vec3 f_l;

void main() {
	vec4 pos = bump_view * bump_model * vec4(in_Position.x, in_Position.y, 0.0, 1.0); // WTF?? HAN BYTTER Y OG Z
	

	gl_Position = bump_projection * pos;
	

	f_v = normalize(-pos.xyz);
	f_l = normalize(bump_light_pos - pos.xyz);

	texture_coord = in_Position.xy;
}