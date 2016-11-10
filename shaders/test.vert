#version 130

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 normal;

in  vec2 in_Position;

//out vec3 ex_Color;
out vec2 texture_coord;

out vec3 f_v;
out vec3 f_l;
out vec3 f_n;

void main() {
	vec4 pos = view * model * vec4(in_Position.x, 0.0, in_Position.y, 1.0); // WTF?? HAN BYTTER Y OG Z
	

	gl_Position = projection * pos;
	
	//ex_Color = vec3(0.5f, 0.5f, 1.0f);

	f_v = normalize(-pos.xyz);
	f_l = normalize(light_pos - pos.xyz);
	f_n = normalize(normal);

	texture_coord = in_Position.xy;
}
