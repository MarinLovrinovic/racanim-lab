#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 vColor;

uniform mat4 model;
uniform mat4 viewProjection;

void main() {
    vColor = vec3(0, 1, 1);
    gl_Position = viewProjection * model * vec4(aPos, 1.0);
}