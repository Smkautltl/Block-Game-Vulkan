#version 450

layout(location = 0) out vec4 outColour;

layout(location = 0) in vec3 fragposition;
layout(location = 1) in vec3 fragcolour;
layout(location = 2) in vec3 fragNormal;

layout(push_constant) uniform Push 
{
	mat4 transform;
    vec3 position;
} push;

const vec3 lightPos = vec3(10.0, 150.0, -3.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 100.0;
const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 16.0;
const float screenGamma = 2.2;

void main()
{
  vec3 ambientcolour = fragcolour * 0.1; 
  vec3 normal = fragNormal;//normalize(fragNormal);
  vec3 lightDir = lightPos - fragposition;
  float distance = length(lightDir);
  distance = distance * distance;
  lightDir = normalize(lightDir);

  float lambertian = max(dot(lightDir, normal), 0.0);
  float specular = 0.0;

  if (lambertian > 0.0) {

    vec3 viewDir = normalize(-fragposition);

    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    specular = pow(specAngle, shininess);
       
    // this is phong (for comparison)
    //if (mode == 2) {
    //vec3 reflectDir = reflect(-lightDir, normal);
    //specAngle = max(dot(reflectDir, viewDir), 0.0);
    //  // note that the exponent is different here
    //specular = pow(specAngle, shininess/4.0);
    //}
  }
  vec3 colorLinear = ambientcolour + diffuseColor * lambertian * lightColor * lightPower / distance +
                     specColor * specular * lightColor * lightPower / distance;
  vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
  outColour = vec4(colorGammaCorrected, 1.0);
}