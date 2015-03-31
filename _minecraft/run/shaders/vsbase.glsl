varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;
varying vec2 vTexCoord;


uniform float elapsed;
uniform float iGlobalTime;
uniform mat4 invertView;



void main()
{

	//gl_Vertex = invertView* gl_ModelViewMatrix * gl_Vertex;
	vTexCoord = gl_MultiTexCoord0;
	// Transforming The Vertex
	if(gl_Vertex.z <= 10.1 && gl_Normal.z >= 0.5)
	{
		gl_Vertex.z += sin(iGlobalTime+mod(gl_Vertex.y,3.141592))*5;
	}
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//Recalcul du gl_Position : 
	//gl_Position = gl_ProjectionMatrix *  gl_Vertex;
	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal; 

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);
	//Couleur
	color = gl_Color;
}
