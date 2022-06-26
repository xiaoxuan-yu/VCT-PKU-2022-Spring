#version 450 core

uniform mat4 V;

layout(location = 0) in vec3 vposition;
out vec2 textureCoordinateFrag;

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec2 transform(vec2 p) { return 0.5f * p + vec2(0.5f); }

void main() {
	textureCoordinateFrag = transform(vposition.xy);
	gl_Position = vec4(vposition, 1);
}