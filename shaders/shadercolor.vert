#version 330
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 inColor;

uniform mat4 model;
uniform mat4 projection;

out vec3 vColor;

void main() {
    vColor = inColor;
    gl_Position = projection * model * vec4(pos, 1.0);
}
