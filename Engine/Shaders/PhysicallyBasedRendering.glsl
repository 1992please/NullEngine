$vertex_shader
#version 450 core

layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 ModelView;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 MVP;

// Get the position and normal in camera space
void getCamSpace( out vec3 norm, out vec3 position ) {
    norm = normalize(vec3(vec4(VertexNormal, 0.0f) * ModelView));
    position = vec3(vec4(VertexPosition,1.0) * ModelView);
}

void main()
{
    // Get the position and normal in camera space
    getCamSpace(Normal, Position);

    gl_Position = vec4(VertexPosition,1.0) * MVP;
}

// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

const float PI = 3.14159265358979323846;

in vec3 Position;
in vec3 Normal;

layout( location = 0 ) out vec4 FragColor;

uniform struct LightInfo {
  vec4 Position; // Light position in cam. coords.
  vec3 L;        // Intensity
} Light[3];

uniform struct MaterialInfo {
  float Rough;     // Roughness
  bool Metal;      // Metallic (true) or dielectric (false)
  vec3 Color;      // Diffuse color for dielectrics, f0 for metallic
} Material;

uniform struct FogInfo {
  float MaxDist;
  float MinDist;
  vec3 Color;
} Fog;

float ggxDistribution( float nDotH ) {
  float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
  float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
  return alpha2 / (PI * d * d);
}

float geomSmith( float dotProd ) {
  float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
  float denom = dotProd * (1 - k) + k;
  // we didn't add dotProd to the nominator because it will get canceled in the end in the main BRDF spec equation
  return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH){
  vec3 f0 = vec3(0.04);
  if(Material.Metal){
    f0 = Material.Color;
  }
  return f0 + (1 - f0) * pow(1 - lDotH, 5);
}

vec3 microfacetModel( int lightIdx, vec3 position, vec3 n ) {
  vec3 diffuseBrdf = vec3(0.0);
  if(!Material.Metal){
    diffuseBrdf = Material.Color;
  }

  vec3 l = vec3(0.0);
  vec3 lightI = Light[lightIdx].L;
  if( Light[lightIdx].Position.w == 0.0 ){
    l = normalize(Light[lightIdx].Position.xyz);
  }
  else{
    l = Light[lightIdx].Position.xyz - position;
    float dist = length(l);
    l = normalize(l);
    lightI /= (dist * dist);
  }

  vec3 v = normalize( -position );
  vec3 h = normalize( v + l );
  float nDotH = dot( n, h );
  float lDotH = dot( l, h );
  float nDotL = max( dot( n, l ), 0.0 );
  float nDotV = dot( n, v );

  vec3 specBrdf = 0.25 * schlickFresnel(lDotH) * ggxDistribution(nDotH) * geomSmith(nDotL) * geomSmith(nDotV);

  return lightI * (diffuseBrdf + PI * specBrdf)  * nDotL;
}


void main() {
  // float dist = abs( Position.z );
  // float fogFactor = (Fog.MaxDist - dist) /
  //                   (Fog.MaxDist - Fog.MinDist);
  // fogFactor = clamp( fogFactor, 0.0, 1.0 );
  // Evaluate the lighting equation, for each light
  vec3 shadeColor = vec3(0.0);
  for( int i = 0; i < 3; i++ ){
    shadeColor += microfacetModel( i, Position, Normal );
  }
  // vec3 Color = mix( Fog.Color, shadeColor, fogFactor );
  FragColor = vec4(shadeColor, 1.0);
}