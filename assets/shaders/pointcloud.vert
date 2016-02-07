#version 150
uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;

in vec4		ciPosition;
in vec4		ciNormal;
in vec3		iPosition;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	Normal = (ciModelMatrix*ciNormal).xyz;
	FragPos = (ciModelMatrix * (ciPosition+vec4(iPosition,1.0))).xyz;
	gl_Position = ciModelViewProjection * (ciPosition+vec4(iPosition,1.0));
}