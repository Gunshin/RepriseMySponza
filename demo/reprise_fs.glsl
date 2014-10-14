#version 330 core

struct Light
{
    vec3 position;
    vec3 direction;
    float half_cone_angle_degrees;
    float range;
};

vec3 calculateColour(Light light_, vec3 V_);

uniform vec3 camPosition;

uniform vec3 materialColour[4];
uniform float shininess[4];

uniform Light lights[5];

in vec3 vs_pos;
in vec3 vs_normal;
flat in int vs_matIndex;

out vec4 fragment_colour;

void main(void)
{

	vec3 V = normalize(camPosition-vs_pos);

	vec3 col = vec3(0,0,0);
	for(int i = 0; i < 5; ++i)
	{
		col += calculateColour(lights[i], V);
	}
	
    fragment_colour = vec4(col, 1.0);
}

vec3 calculateColour(Light light_, vec3 V_)
{
	
	vec3 L = normalize(light_.position - vs_pos);

	float LDdotML = dot(normalize(light_.direction), normalize(-L));

	if(LDdotML > cos(light_.half_cone_angle_degrees))
	{
		vec3 R = reflect(-L, vs_normal);

		float distance = distance(vs_pos, light_.position);
		float attenuation = 1.0 / (2
							   + 0.01 * distance
							   + 0.0001 * distance * distance);
		vec3 Id = materialColour[vs_matIndex] * max(dot(L, vs_normal), 0) * attenuation;

		vec3 Is = vec3(0, 0, 0);
		if(dot(L, vs_normal) > 0 && shininess[vs_matIndex] > 0)
		{
			Is = vec3(1, 1, 1) * pow(max(0, dot(R, V_)), shininess[vs_matIndex]) * attenuation;
		}

		return Id + Is;
	}

	return vec3(0,0,0);
}