#version 150
uniform vec3 u_LightPos;
uniform vec3 u_EyePos;
uniform vec3 u_RimColor;
uniform vec3 u_SpecColor;
uniform float u_SpecPower;
uniform float u_RimPower;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	vec3 lightDir = normalize(u_LightPos - FragPos);
	vec3 eyeDir = normalize(u_EyePos - FragPos);
	vec3 normal = normalize(Normal);
	
	float specular = pow(max(0, dot( reflect(lightDir,normal), -eyeDir)), u_SpecPower);
	float fresnel = 1.0 - clamp( pow(max(dot(normal, eyeDir), 0), u_RimPower), 0, 1);

	FragColor.rgb = u_SpecColor*specular+u_RimColor*fresnel;
	FragColor.a = 1;
	//FragColor = vec4(1);
}