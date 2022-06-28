#version 450 core
layout(location = 0) in vec3 worldPosition;  //world coordinates of model
layout(location = 2) in vec2 textureCoord;           //texture coordinates

uniform mat4 DepthModelViewProjectionMatrix;
uniform mat4 ModelMatrix;

out VertexOut {
    vec2 texCoord;
    vec3 fragPos;
} ;

void main() {
    gl_Position = ModelMatrix * vec4(worldPosition,1);
    texCoord = textureCoord;
    fragPos = (ModelMatrix * vec4(worldPosition,1)).xyz;
}