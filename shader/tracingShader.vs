#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec2 UV;
out vec3 WorldPosition;
out vec3 WorldNormal;
out vec3 WorldTangent;
out vec3 WorldBitangent;
out vec3 WorldLookAtDir;

uniform vec3 CameraPosition;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition,1);

	WorldPosition = (ModelMatrix * vec4(vertexPosition,1)).xyz;

	WorldNormal = normalize((ModelMatrix * vec4(vertexNormal,0)).xyz);
	WorldTangent = normalize((ModelMatrix * vec4(vertexTangent,0)).xyz);
	WorldBitangent = normalize((ModelMatrix * vec4(vertexBitangent,0)).xyz);
	WorldLookAtDir = CameraPosition - WorldPosition; 

	UV = vertexUV;
}


