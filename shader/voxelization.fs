#version 450
uniform sampler2D DiffuseTexture;

uniform samplerCube ShadowMap;
uniform vec3 lightPos;
uniform float far_plane;

uniform int VoxelDimensions;
uniform layout(RGBA8) image3D VoxelTexture;

in GeometryOut{
	vec2 TexCoord;
	flat int projectIndex;
	vec3 FragPos;
} frag;


vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowMapping(float bias)
{

	vec3 fragToLight = frag.FragPos - lightPos;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(ShadowMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= far_plane;   // Undo mapping [0,1]
                if(currentDepth - bias > closestDepth)
                    shadow += 0.0;
                else
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

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