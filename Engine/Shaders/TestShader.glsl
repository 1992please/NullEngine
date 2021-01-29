#type vertex
#version 450 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec4 InColor;
out vec4 Color;
void main()
{
    gl_Position = vec4(VertexPosition,1.0);
    Color = InColor;
}
#type fragment
#version 450 core
layout( location = 0 ) out vec4 FragColor;
in vec4 Color;

void main() {
	FragColor = Color;
}