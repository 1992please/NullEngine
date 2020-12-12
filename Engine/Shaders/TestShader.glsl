$vertex_shader
#version 450 core

subroutine vec3 shadeModelType( vec3 position, vec3 normal);
subroutine uniform shadeModelType shadeModel;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform struct LightInfo {
  vec3 Position; // Light position in eye coords.
  vec3 La;       // Ambient light intensity
  vec3 Ld;       // Diffuse light intensity
  vec3 Ls;       // Specular light intensity
} Light;

uniform struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
} Material;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity

uniform mat4 ModelView;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 MVP;

void getCamSpace( out vec3 norm, out vec3 position ) {
    norm = normalize(vec3(vec4(VertexNormal, 0.0f) * ModelView));
    position = vec3(vec4(VertexPosition,1.0) * ModelView);
}


subroutine( shadeModelType )
vec3 phongModel( vec3 position, vec3 n ) {  
  vec3 ambient = Light.La * Material.Ka;
  vec3 s = normalize( Light.Position - position );
  float sDotN = max( dot(s,n), 0.0 );
  vec3 diffuse = Light.Ld * Material.Kd * sDotN;
  vec3 spec = vec3(0.0);
  if( sDotN > 0.0 ) {
    vec3 v = normalize(-position);
    vec3 r = reflect( -s, n );
    spec = Light.Ls * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );
  }

  return ambient + diffuse + spec;
}

subroutine( shadeModelType )
vec3 diffuseOnly( vec3 position, vec3 n )
{
    vec3 s = normalize( Light.Position - position );
    return Light.Ld * Material.Kd * max( dot(s, n), 0.0 );
}

void main()
{
    // Get the position and normal in camera space
    vec3 camNorm, camPosition;
    getCamSpace(camNorm, camPosition);

    // Evaluate the reflection model
    LightIntensity = shadeModel( camPosition, camNorm );

    gl_Position = vec4(VertexPosition,1.0) * MVP;
}

// //------------------------------------------------------------------------------------------------------------------
$fragment_shader
#version 450 core

in vec3 LightIntensity;

layout( location = 0 ) out vec4 FragColor;

void main() {
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}