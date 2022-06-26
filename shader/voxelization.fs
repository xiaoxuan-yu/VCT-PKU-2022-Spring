#version 450
uniform sampler2D DiffuseTexture;

uniform sampler2D ShadowMap;
uniform int shadowMapRes;

uniform int VoxelDimensions;
uniform layout(RGBA8) image3D VoxelTexture;

in GeometryOut{
	vec2 TexCoord;
	flat int projectIndex;
	vec4 DepthCoord; //do not change in gs
} frag;


float ShadowMapping(float bias)
{

	float current_depth = frag.DepthCoord.z / frag.DepthCoord.w;

	float shadow = 0.0f;
	int radius = 2;

	//PCF Algorithm, 3x3 sampling space
	for (int x = -radius; x <= radius; ++x)
	{
		for (int y = -radius; y <= radius; ++y)
		{
			vec2 offset = vec2(1.0f / shadowMapRes * x, 1.0f / shadowMapRes * y);
			float closest_depth = texture(ShadowMap, vec2(frag.DepthCoord.xy + offset)).r;

			//use bias for antialiasing
			if (current_depth - bias <= closest_depth)
				shadow += 1.0f;
		}
	}

	shadow /= (2 * radius + 1) * (2 * radius + 1);
	return shadow;
}

void main() {
	vec4 color = texture(DiffuseTexture, frag.TexCoord);
	ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, VoxelDimensions * gl_FragCoord.z);
	ivec3 voxelPos;
	if (frag.projectIndex == 1)
	{
		voxelPos.x = VoxelDimensions - 1 - camPos.z;
		voxelPos.z = VoxelDimensions - 1 - camPos.x;
		voxelPos.y = camPos.y;
	}
	else if (frag.projectIndex == 2)
	{
		voxelPos.z = VoxelDimensions - 1 - camPos.y;
		voxelPos.y = VoxelDimensions - 1 - camPos.z;
		voxelPos.x = camPos.x;
	}
	else
	{
		voxelPos = camPos;
		voxelPos.z = VoxelDimensions - 1 - camPos.z;
	}
	imageStore(VoxelTexture, voxelPos, vec4(color.rgb * ShadowMapping(0.002), 1.0));
}