#version 450

layout (location = 0) in vec3 fragColour;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUV;
layout (location = 4) in flat uint fragTextureIndex;

layout (location = 0) out vec4 outColour;

struct PointLight 
{
  vec4 position; // ignore w
  vec4 Colour; // w is intensity
};

layout(binding = 0) uniform GlobalUbo 
{
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColour; // w is intensity
  PointLight pointLights[10];
  int numLights;
} ubo;

layout(binding = 1) uniform sampler2DArray shadowMap;
layout(binding = 2) uniform sampler texSampler;
layout(binding = 3) uniform texture2D textures[1024];

layout(push_constant) uniform Push 
{
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main() 
{
    if(fragTextureIndex > 0 && fragTextureIndex < 7)
    {
        outColour = texture(sampler2D(textures[fragTextureIndex], texSampler), vec2(fragUV.y, fragUV.x));
    }
    else
    {
        vec3 diffuseLight = ubo.ambientLightColour.xyz * ubo.ambientLightColour.w;
        vec3 specularLight = vec3(0.0);
        vec3 surfaceNormal = normalize(fragNormalWorld);

        vec3 cameraPosWorld = ubo.inverseView[3].xyz;
        vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

        for (int i = 0; i < ubo.numLights; i++) 
        {
          PointLight light = ubo.pointLights[i];
          vec3 directionToLight = light.position.xyz - fragPosWorld;
          float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
          directionToLight = normalize(directionToLight);

          //DIFFUSE LIGHTING
          float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
          vec3 intensity = light.Colour.xyz * light.Colour.w * attenuation;

          diffuseLight += intensity * cosAngIncidence;

          //SPECULAR LIGHTING
          vec3 halfAngle = normalize(directionToLight + viewDirection);
          float blinnTerm = dot(surfaceNormal, halfAngle);
          blinnTerm = clamp(blinnTerm, 0, 1);
          blinnTerm = pow(blinnTerm, 64.0); //Higher value = Sharper Highlights
          specularLight += intensity * blinnTerm;


        }

        outColour = vec4(diffuseLight * fragColour + specularLight * fragColour, 1.0) * texture(sampler2D(textures[fragTextureIndex], texSampler), vec2(fragUV.y, fragUV.x));
    }
  
}