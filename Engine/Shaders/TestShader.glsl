$vertex_shader
#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

//uniform struct {
//  mat4 RotationMatrix;
//  mat4 ViewMatrix; } MyMats;

uniform mat4 RotationMatrix;

//uniform mat4 Mats[2];

void main()
{
    Color = VertexColor;
    gl_Position = RotationMatrix * vec4(VertexPosition,1.0);
}


// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

in vec3 Color;
layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(Color, 1.0);
}
