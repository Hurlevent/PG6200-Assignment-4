#version 130

uniform sampler2D color_texture;

in vec3 ex_Color;
in vec2 texture_coord;

out vec4 res_Color;

void main() {
	vec4 color = texture2D(color_texture, texture_coord);

    //res_Color = vec4(ex_Color, 1.0);

	res_Color = color;
}
