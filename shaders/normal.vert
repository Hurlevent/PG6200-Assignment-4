#version 130

uniform mat4 some_transform_matrix;

in vec3 in_Position;

void main(){
	vec4 n_pos = some_transform_matrix * vec4(in_Position, 1.0f);
	gl_Position = mat4(vec4(2.0, 0.0, 0.0, 0.0), vec4(0.0, 2.0, 0.0, 0.0), vec4(0.0, 0.0, 2.0, 0.0), vec4(-1.0, -1.0, 0.0, 1.0)) * n_pos;
}