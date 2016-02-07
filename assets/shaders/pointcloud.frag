#version 150
uniform vec3 u_LightPos;
uniform vec3 u_EyePos;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	vec4 blue = vec4(0,0.15,0.75,1);
	vec3 lightDir = normalize(u_LightPos - FragPos);
	vec3 eyeDir = normalize(FragPos - u_EyePos);
	vec3 normal = normalize(Normal);
	
	float diffuse = max(dot(normal,lightDir), 0);
	float specular = pow(max(0, dot( reflect(lightDir,normal), eyeDir)), 16.0f);

	FragColor = blue*diffuse+blue*specular;
}