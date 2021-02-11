// basic texture shader
#type vertex
#version 460 core
layout (location = 0) in vec3 a_VertexPosition;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TextureCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoords; 
out vec4 v_Color;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_VertexPosition,1.0);
    v_TexCoords = a_TextureCoords;
    v_Color = a_Color;
}

#type fragment
#version 460 core
layout( location = 0 ) out vec4 FragColor;
in vec2 v_TexCoords; 
in vec4 v_Color;
uniform sampler2D u_Texture;
uniform vec2 u_Tiling;

void main() {
	FragColor = v_Color;
}