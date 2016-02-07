#version 150
uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;

in vec4 ciPosition;
in vec4 ciNormal;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	Normal = (ciModelMatrix*ciNormal).xyz;
	FragPos  = (ciModelMatrix*ciPosition).xyz;

	gl_Position = ciModelViewProjection*ciPosition;
}