#version 150
uniform vec3 u_LightPos;
uniform vec3 u_EyePos;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	vec4 baseBlue = vec4(0,0,1,1);
	vec4 hiBlue = vec4(0.509, 0.093, 0.074, 1);
	vec4 rimBlue = vec4(0, 1.0, 0.187, 1);

	vec3 lightDir = normalize(u_LightPos - FragPos);
	vec3 eyeDir = normalize(u_EyePos - FragPos);
	vec3 normal = normalize(Normal);
	
	float diffuse = max(dot(normal,lightDir), 0);
	float specular = pow(max(0, dot( reflect(lightDir,normal), -eyeDir)), 2.0f);
	float fresnel = 1.0 - clamp( pow(max(dot(normal, eyeDir), 0), 4.0f), 0, 1);

	FragColor = baseBlue*diffuse+hiBlue*specular+rimBlue*fresnel;
}