$vertex_shader
#version 450 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec3 LightPosition; // Light position in eye coords.
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity

uniform mat4 Model;
uniform mat4 MVP;

void main()
{
    vec3 tnorm = normalize(vec3(vec4(VertexNormal, 0.0f) * Model));
    vec3 eyeCoords = vec3(vec4(VertexPosition,1.0) * Model);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));

    LightIntensity = Ld * Kd * max( dot( s, tnorm ), 0.0 );

    gl_Position = vec4(VertexPosition,1.0) * MVP;
}

// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

in vec3 LightIntensity;

layout( location = 0 ) out vec4 FragColor;

void main() {
    FragColor = vec4(LightIntensity, 1.0);
}