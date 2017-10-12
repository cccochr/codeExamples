// walls.frag
varying vec3 ec_vnormal, ec_vposition;

void main()
{
	vec3 P, N, L, V, H;
	vec4 diffuse_color = gl_FrontMaterial.diffuse; 
	vec4 specular_color = gl_FrontMaterial.specular; 
	float shininess = gl_FrontMaterial.shininess, blinn_phong;
	float pi = 3.14159265;
	blinn_phong = (shininess+2.0)/(8.0*pi);

	P = ec_vposition;
	N = normalize(ec_vnormal);
	vec4 final_color = {0,0,0,0};

	L = normalize(gl_LightSource[0].position - P);
	V = normalize(-P);
	H = normalize(L+V);

	diffuse_color*=gl_LightSource[0].diffuse;	
	final_color += ((diffuse_color*(max(dot(N,L),0.0))) + (specular_color*blinn_phong*pow(max(dot(H,N),0.0),shininess)));

	L = normalize(gl_LightSource[1].position - P);
	H = normalize(L+V);
	final_color += (0.2*(diffuse_color*(max(dot(N,L),0.0))) + (specular_color*blinn_phong*pow(max(dot(H,N),0.0),shininess)));

	gl_FragColor = final_color;
}
