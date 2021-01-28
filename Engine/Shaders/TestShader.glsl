#type vertex
#version 450 core
layout (location = 0) in vec3 VertexPosition;

out vec3 Color;
void main()
{
    gl_Position = vec4(VertexPosition,1.0);
    Color = vec3(1.0, 0.0, 0.0);
}

#type fragment
#version 450 core
layout( location = 0 ) out vec4 FragColor;
in vec3 Color;

void main() {
	FragColor = vec4(Color, 1.0);
}