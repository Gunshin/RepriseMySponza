#version 330

uniform mat4 projectionViewMat;
uniform mat4 viewMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
//layout (location = 2) in vec2 texCoord;
layout (location = 2) in mat4x3 instanceMat;
layout (location = 6) in int matIndex;

out vec3 vs_pos;
out vec3 vs_normal;
out vec3 vs_colour;

void main(void)
{
	vec4 pos = vec4(instanceMat * vec4(position, 1), 1);
	vs_pos = vec3(pos);
	vs_normal = normalize(mat3(instanceMat) * normal);

	vs_colour = vec3(1.0, 0.0, 0.0); //vec3(1 - texCoord.x, texCoord.x, texCoord.y);

	gl_Position = projectionViewMat * pos;
}
