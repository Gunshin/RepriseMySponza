#version 430 core

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float half_cone_angle_degrees;
};

struct Material
{
    vec3 colour;
    float shininess;
};

vec3 calculateColour(Light light_, Material mat_, vec3 V_);

uniform vec3 camPosition;

layout(std430, binding = 1) buffer BufferMaterials
{
    Material materials[];
};

layout(std430, binding = 2) buffer BufferLights
{
    Light lights[];
};

in vec3 vs_pos;
in vec3 vs_normal;
flat in int vs_matIndex;

out vec4 fragment_colour;

void main(void)
{

	vec3 V = normalize(camPosition - vs_pos);

	vec3 col = vec3(0,0,0);
	for(int i = 0; i < lights.length(); ++i)
	{
		col += calculateColour(lights[i], materials[vs_matIndex], V);
	}
	
    fragment_colour = vec4(col, 1.0);
}

vec3 calculateColour(Light light_, Material mat_, vec3 V_)
{
	
	vec3 L = normalize(light_.position - vs_pos);

	float LDdotML = dot(normalize(light_.direction), normalize(-L));

	if(LDdotML > cos(light_.half_cone_angle_degrees))
	{
		vec3 R = normalize(reflect(-L, vs_normal));

		float distance = distance(vs_pos, light_.position);

        vec3 attenuatedDistance = vec3(1.0, 1.0, 1.0) * smoothstep(light_.range, 1, distance);
        vec3 attenuatedCone = vec3(1.0, 1.0, 1.0) * smoothstep(cos(light_.half_cone_angle_degrees), 1, LDdotML);

        vec3 attenuatedLight = attenuatedDistance * attenuatedCone;

		vec3 Id = mat_.colour * max(dot(L, vs_normal), 0) * attenuatedLight;

		vec3 Is = vec3(0, 0, 0);
		if(dot(L, vs_normal) > 0 && mat_.shininess > 0)
		{
			Is = vec3(1, 1, 1) * pow(max(0, dot(R, V_)), mat_.shininess) * attenuatedLight;
		}

		return Id + Is;
	}

	return vec3(0,0,0);
}