#version 150
uniform mat4 ciModelViewProjection;
in vec4		ciPosition;
in vec3		iPosition;

void main()
{
	gl_Position = ciModelViewProjection * (ciPosition+vec4(iPosition,1.0));
}