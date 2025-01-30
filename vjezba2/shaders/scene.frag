#version 330 core

in vec3 normal;
in vec3 position;
in vec3 uv;

uniform mat4 model;
uniform mat4 viewProjection;

uniform vec3 eye;

uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform vec3 materialAmbient;
uniform vec4 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

out vec4 FragColor;

void main() {
    vec3 normalizedNormal = normalize(normal);

    vec3 toEye = normalize(eye - position);
    vec3 toLight = lightPosition - position;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight;

    vec3 ambient = materialAmbient * lightAmbient;
    vec3 diffuse = vec3(materialDiffuse) * clamp(dot(normalizedNormal, toLight), 0, 1) * lightDiffuse;

    float specularMultiplier = pow(max(0, dot(toEye, reflect(-toLight, normalizedNormal))), materialShininess);
    vec3 specular = materialSpecular * specularMultiplier * lightSpecular;

    vec3 light = ambient + (diffuse + specular) / (distanceToLight + 0.00001);

//    FragColor = vec4(light, 1.0);
    float xFromCenter = uv.x - 0.5;
    float yFromCenter = uv.y - 0.5;
    float fromCenter = sqrt(xFromCenter * xFromCenter + yFromCenter * yFromCenter);
    FragColor = vec4(materialDiffuse.rgb, materialDiffuse.a * (1 - 2 * fromCenter));
//    FragColor = vec4(1, 1, 1, 1 - 2 * fromCenter);

}
