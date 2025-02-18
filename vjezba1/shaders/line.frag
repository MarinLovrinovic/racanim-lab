#version 330 core

in vec3 vColor;

out vec4 FragColor;

uniform vec3 materialDiffuse;

void main() {
    FragColor = vec4(materialDiffuse, 1);
}
