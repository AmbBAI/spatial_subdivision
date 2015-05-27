$input v_pos, v_normal

#include <bgfx_shader.sh>

void main()
{
	vec3 lightDir = normalize(vec3(1.,1.,-1.));
	float l = max(dot(lightDir, v_normal), 0.) * .8 + .2;

	gl_FragColor = vec4(l,l,l,1.);
}