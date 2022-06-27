#version 450 core

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location = 0) in vec3 vposition;
//out vec2 textureCoordinateFrag;
out vec3 worldPosition;

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec2 transform(vec2 p) { return 0.5f * p + vec2(0.5f); }

void main() {
	//textureCoordinateFrag = transform((P*M*vec4(vposition,1.0f)).xy);
	worldPosition = vposition * 0.5 + 0.5;
	gl_Position = P*V*M*vec4(vposition, 1);
}