// teapot.frag
varying vec3 ec_vnormal, ec_vposition;
uniform sampler2D mytexture;

void main()
{
	vec4 final_color = {1,1,1,1};
	gl_FragColor = final_color;
}


