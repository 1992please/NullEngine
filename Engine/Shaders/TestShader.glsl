$vertex_shader
#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexTexCoord;

out vec3 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition,1.0);
}


// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

in vec3 TexCoord;
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform BlobSettings { 
  vec4 InnerColor; 
  vec4 OuterColor; 
  float RadiusInner; 
  float RadiusOuter; 
};

void main() {
  float dx = TexCoord.x - 0.5; 
  float dy = TexCoord.y - 0.5; 
  float dist = sqrt(dx * dx + dy * dy); 
  FragColor =
    mix( InnerColor, OuterColor,
       smoothstep( RadiusInner, RadiusOuter, dist )); 
}
