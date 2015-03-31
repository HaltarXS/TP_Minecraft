uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;
uniform float iGlobalTime;

float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}


void main (void)
{
	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	gl_FragColor = color;
}