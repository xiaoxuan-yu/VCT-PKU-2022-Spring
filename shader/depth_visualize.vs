#version 450 core

layout(location = 0) in vec3 vertexPosition;

out vec3 WorldPosition;

uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
	gl_Position =  ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition,1);

	WorldPosition = (ModelMatrix * vec4(vertexPosition,1)).xyz;
}