#type vertex
#version 460 core
layout (location = 0) in vec3 a_VertexPosition;
layout (location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec4 v_Color;
void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_VertexPosition,1.0);
    v_Color = a_Color;
}
#type fragment
#version 460 core
layout( location = 0 ) out vec4 FragColor;
in vec4 v_Color;

void main() {
	FragColor = v_Color;
}