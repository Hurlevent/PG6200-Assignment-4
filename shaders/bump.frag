#version 330

uniform sampler2D bump_color_texture;
uniform sampler2D bump_normal_map;
uniform mat4 bump_model;
uniform mat4 bump_transpose_inverse_modelview;

in vec2 texture_coord;

in vec3 f_v;
in vec3 f_l;

out vec4 res_Color;



void main() {
	
	vec4 color = texture2D(bump_color_texture, texture_coord);
	vec4 normal = texture2D(bump_normal_map, texture_coord);

		
	vec4 transformed_normal = bump_transpose_inverse_modelview * (normal * 2 - 1);

	vec3 h = normalize(f_v + f_l);
	vec3 n = normalize(transformed_normal.xyz);

	float diff = max(0.1f, dot(n, f_l));

	float spec = pow(max(0.0f, dot(n, h)), 128.0f);

	res_Color = diff * color + vec4(spec);
	
}
