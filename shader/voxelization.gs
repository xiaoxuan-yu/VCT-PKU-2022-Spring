#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 ProjectionX;
uniform mat4 ProjectionY;
uniform mat4 ProjectionZ;

in VertexOut{
    vec2 texCoord;
    vec4 depthCoord;    //depth texture coordinates for the vertex
} vertices[];

out GeometryOut{
	vec2 TexCoord;
	flat int projectIndex;
	vec4 DepthCoord; //do not change in gs
};

int selectViewProject()
{
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 faceNormal = cross(p1, p2);
	float nDX = abs(faceNormal.x);
	float nDY = abs(faceNormal.y);
	float nDZ = abs(faceNormal.z);

	if (nDX >= nDY && nDX >= nDZ)
	{
		return 1;
	}
	else if (nDY >= nDX && nDY >= nDZ)
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

void main() {
	projectIndex = selectViewProject();
	mat4 projectionMatrix = projectIndex == 1 ? ProjectionX : projectIndex == 2 ? ProjectionY : ProjectionZ;
	for (int i = 0; i < gl_in.length(); i++) {
		TexCoord = vertices[i].texCoord;
		DepthCoord = vertices[i].depthCoord;
		gl_Position = projectionMatrix * gl_in[i].gl_Position;  //Projection
		EmitVertex();
	}
	EndPrimitive();
}