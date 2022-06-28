#version 450 core

layout(location = 0) in vec3 vertexPosition_model;

uniform mat4 ModelMatrix;

void main() {
	gl_Position = ModelMatrix * vec4(vertexPosition_model, 1);	
}