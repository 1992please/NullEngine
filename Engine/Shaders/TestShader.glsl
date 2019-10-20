$vertex_shader
#version 450 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
uniform mat4 MVP;

void main()
{
   gl_Position = vec4(VertexPosition,1.0) * MVP;
}
// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

layout( location = 0 ) out vec4 FragColor;

uniform vec3 UnlitColor;
void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}