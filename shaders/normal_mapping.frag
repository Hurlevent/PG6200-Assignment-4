#version 330

uniform sampler2D nm_color_texture;
uniform sampler2D nm_normal_map;

in vec2 texture_coord;

in vec3 f_v;
in vec3 f_l;

out vec4 res_Color;



void main() {
	
	vec4 color = texture2D(nm_color_texture, texture_coord);
	vec4 normal = texture2D(nm_normal_map, texture_coord);
	

	vec3 h = normalize(normalize(f_v) + normalize(f_l));
	vec3 n = normalize(normal.xyz);

	float diff = max(0.1f, dot(n, normalize(f_l)));

	float spec = pow(max(0.0f, dot(n, h)), 128.0f);

	res_Color = diff * color + vec4(spec);
	
}
