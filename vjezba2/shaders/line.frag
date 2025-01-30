#version 330 core

in vec3 vColor;

out vec4 FragColor;

uniform vec4 materialDiffuse;

void main() {
    FragColor = materialDiffuse;
}
