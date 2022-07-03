#version 450 core

in vec3 WorldPosition;

out vec4 FragColor;

uniform vec3 lightPos;
uniform float far_plane;
uniform samplerCube ShadowMap;

void main()
{
    // Get vector between fragment position and light position
    vec3 fragToLight = WorldPosition - lightPos;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(ShadowMap, fragToLight).r;
    
    FragColor = vec4(vec3(closestDepth), 1.0);
}