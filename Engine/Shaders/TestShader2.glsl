#type vertex
#version 460 core
layout (location = 0) in vec3 a_VertexPosition;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform vec4 u_Color;

out vec4 v_Color;
void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_VertexPosition,1.0);
    
    v_Color = u_Color;
}
#type fragment
#version 460 core
layout( location = 0 ) out vec4 FragColor;
in vec4 v_Color;

void main() {
	FragColor = v_Color;
}