#version 450 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // write this as modified depth
    gl_FragDepth = lightDistance / far_plane;
}