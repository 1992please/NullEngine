// basic texture shader
#type vertex
#version 460 core
layout (location = 0) in vec3 a_VertexPosition;
layout (location = 1) in vec2 a_TextureCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoords; 

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_VertexPosition,1.0);
    v_TexCoords = a_TextureCoords;
}

#type fragment
#version 460 core
layout( location = 0 ) out vec4 FragColor;
in vec2 v_TexCoords; 

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec2 u_Tiling;

void main() {
	FragColor = texture(u_Texture, v_TexCoords * u_Tiling) * u_Color;
}