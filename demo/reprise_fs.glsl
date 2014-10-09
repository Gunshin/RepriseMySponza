#version 330 core

struct Light
{
    vec3 position;
    vec3 direction;
    float half_cone_angle_degrees;
    float range;
};

vec3 calculateColour(Light light_);

uniform vec3 ambientIntensity;

uniform Light lights[5];

in vec3 vs_pos;
in vec3 vs_normal;
in vec3 vs_colour;

out vec4 fragment_colour;

void main(void)
{

	vec3 col = vec3(0,0,0);
	for(int i = 0; i < 5; ++i)
	{
		col += calculateColour(lights[i]);
	}
	vec3 Ia = ambientIntensity;

    fragment_colour = vec4(col + Ia, 1.0);
}

vec3 calculateColour(Light light_)
{
	
	vec3 L = normalize(light_.position - vs_pos);

	float LDdotML = dot(normalize(light_.direction), normalize(-L));

	if(LDdotML > cos(light_.half_cone_angle_degrees))
	{

		/*vec3 colour = vec3(0);
		colour.r = vs_pos.x;*/

		float distance = distance(vs_pos, light_.position);
		float attenuation = 1.0 / (2
							   + 0.1 * distance
							   + 0.0001 * distance * distance);
	
		vec3 Id = vs_colour * max(dot(L, vs_normal), 0) * attenuation;

		return Id;
	}

	return vec3(0,0,0);
}