#version 450 core
#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)
#define STEP_LENGTH 0.001f

//uniform sampler2D textureFront; // Unit cube front FBO.
//uniform sampler2D textureBack; // Unit cube back FBO.
uniform sampler3D texture3D; // Texture in which voxelization is stored.
uniform vec3 cameraPosition; // World camera position.
uniform int state = 0; // Decides mipmap sample level.

//in vec2 textureCoordinateFrag;
in vec3 worldPosition;
out vec4 color;

vec3 transform(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	const float mipmapLevel = state;

	// Initialize ray.
	vec3 origin;
	origin.x = cameraPosition.x / 240;
	origin.y = cameraPosition.y / 240;
	origin.z = cameraPosition.z / 240;
	vec3 direction = worldPosition-origin;
	const uint numberOfSteps = uint(INV_STEP_LENGTH * length(direction));
	direction = normalize(direction);

	for (uint step = 0; step < numberOfSteps && color.a < 0.99f; ++step) {
		const vec3 currentPoint = origin + STEP_LENGTH * step * direction;
		vec3 coordinate = transform(currentPoint);
		//float mipmapLevel = log2(step);
		vec4 currentSample = textureLod(texture3D, transform(currentPoint), mipmapLevel);
		color += (1.0f - color.a) * currentSample;
	}
}