#version 430

//uniform sampler2D g_texture;
//uniform vec4 g_color = vec4(1, 1, 1, 1);

const vec3 g_sunDirection = vec3(0.3, -1, 0.7);

in vec2 texcoord;
in vec3 normal;

out vec4 color;

void main()
{
	float sunPower = dot(normal, normalize(-g_sunDirection)) * 0.5f;
	color = 0.3 + sunPower * vec4(1, 1, 1, 1);
}
