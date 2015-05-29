$input a_position, a_normal
$output v_pos, v_normal

#include <bgfx_shader.sh>

void main()
{
	vec3 normal = a_normal.xyz*2.0 - 1.0;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

	v_pos = gl_Position.xyz;
	v_normal = mul(u_modelView, vec4(normal, 0.0) ).xyz;
}
