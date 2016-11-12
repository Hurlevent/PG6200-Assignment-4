#version 330

uniform sampler2D reg_color_texture;

in vec2 texture_coord;

in vec3 f_v;
in vec3 f_l;
in vec3 f_n;

out vec4 res_Color;



void main() {
	//texture_coord.y *= -1.0f;
	vec4 color = texture2D(reg_color_texture, texture_coord);


	vec3 h = normalize(f_v + f_l);
	vec3 n = normalize(f_n);

	float diff = max(0.1f, dot(n, f_l));

	float spec = pow(max(0.0f, dot(n, h)), 128.0f);

	res_Color = diff * color + vec4(spec);

}
