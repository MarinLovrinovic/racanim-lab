#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vColor[];
out vec3 gColor;

void main() {
    vec3 a = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 b = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 c = gl_in[2].gl_Position.xyy / gl_in[2].gl_Position.w;

    vec3 normal = cross(b - a, c - a);

    if (normal.z > 0) {
        for (int i = 0; i < 3; ++i) {
            gl_Position = gl_in[i].gl_Position;
            gColor = vColor[i]; // Pass color to fragment shader
            EmitVertex();
        }
        EndPrimitive();
    }
}
