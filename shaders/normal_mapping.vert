#version 330

uniform mat4 nm_projection;
uniform mat4 nm_model;
uniform mat4 nm_view;
uniform vec3 nm_light_pos;

in  vec2 in_Position;

out vec2 texture_coord;

out vec3 f_v;
out vec3 f_l;

void main() {
	vec4 pos = nm_view * nm_model * vec4(in_Position.x, 0.0, in_Position.y, 1.0); // WTF?? HAN BYTTER Y OG Z
	

	gl_Position = nm_projection * pos;
	

	f_v = normalize(-pos.xyz);
	f_l = normalize(nm_light_pos - pos.xyz);

	texture_coord = in_Position.xy;
}
