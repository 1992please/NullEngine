// basic texture shader
#type vertex
#version 460 core
layout (location = 0) in vec3 a_VertexPosition;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TextureCoords;
layout (location = 3) in float a_TexIndex;
layout (location = 4) in vec2 a_Tiling;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoords; 
out vec4 v_Color;
out float v_TexIndex;
out vec2 v_Tiling;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_VertexPosition,1.0);
    v_TexCoords = a_TextureCoords;
    v_Color = a_Color;
    v_TexIndex = a_TexIndex;
    v_Tiling = a_Tiling;
}

#type fragment
#version 460 core
layout( location = 0 ) out vec4 FragColor;
in vec2 v_TexCoords; 
flat in vec4 v_Color;
flat in float v_TexIndex;
flat in vec2 v_Tiling;

uniform sampler2D u_Textures[32];

void main() 
{
    // Tiling 
    FragColor = texture(u_Textures[uint(v_TexIndex)], v_TexCoords * v_Tiling) * v_Color;
}