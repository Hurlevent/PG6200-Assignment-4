#version 130

uniform sampler2D color_texture;
//uniform vec3 normal;

//in vec3 ex_Color;
in vec2 texture_coord;

in vec3 f_v;
in vec3 f_l;
in vec3 f_n;

out vec4 res_Color;



void main() {
	vec4 color = texture2D(color_texture, texture_coord);

	vec3 h = normalize(f_v + f_l);
	vec3 n = normalize(f_n);

	float diff = max(0.1f, dot(n, f_l));

	float spec = pow(max(0.0f, dot(n, h)), 128.0f);

	res_Color = diff * color + vec4(spec);
	//res_Color = vec4(ex_Color, 1.0);
}
