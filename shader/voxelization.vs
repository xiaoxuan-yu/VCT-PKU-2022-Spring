#version 450 core
layout(location = 0) in vec3 worldPosition;  //world coordinates of model
layout(location = 2) in vec2 textureCoord;           //texture coordinates

uniform mat4 DepthModelViewProjectionMatrix;
uniform mat4 ModelMatrix;

out VertexOut {
    vec2 texCoord;
    vec4 depthCoord;    //depth texture coordinates for the vertex
} ;

void main() {
    gl_Position = ModelMatrix * vec4(worldPosition,1);
    texCoord = textureCoord;
    depthCoord = DepthModelViewProjectionMatrix * vec4(worldPosition, 1);   
	depthCoord.xyz = depthCoord.xyz * 0.5f + vec3(0.5f);    //transform x,y,z into [0,1]
}