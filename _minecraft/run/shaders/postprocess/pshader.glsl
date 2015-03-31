uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;
uniform float iGlobalTime;
uniform bool transition;
uniform float startTime;


float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

bool isContour(vec2 texCoord,float xstep, float ystep,float baseDepth)
{
	if(baseDepth > 0.05)
		return false;
	float dif = 0.001;
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x-xstep,texCoord.y-ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x-xstep,texCoord.y) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x-xstep,texCoord.y+ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x,texCoord.y-ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x,texCoord.y+ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x+xstep,texCoord.y-ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x+xstep,texCoord.y) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	{
		float depth = LinearizeDepth(texture2D( Texture1 , vec2(texCoord.x+xstep,texCoord.y+ystep) ).r);
		if(depth > baseDepth+dif || depth < baseDepth - dif)
			return true;
	}
	return false;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 oldTv(vec3 col)
{
	vec2 iResolution = vec2(screen_width,screen_height);
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float screenRatio = iResolution.x / iResolution.y;
	
	vec3 texture = col.rgb;
	
	float barHeight = 6.;
	float barSpeed = 2.0;
	float barOverflow = 1.2;
	float blurBar = clamp(sin(uv.y * barHeight + iGlobalTime * barSpeed) + 1.25, 0., 1.);
	float bar = clamp(floor(sin(uv.y * barHeight + iGlobalTime * barSpeed) + 1.95), 0., barOverflow);
	
	float noiseIntensity = .60;
	float pixelDensity = 250.;
	vec3 color = vec3(clamp(rand(
		vec2(floor(uv.x * pixelDensity * screenRatio), floor(uv.y * pixelDensity)) *
		iGlobalTime / 1000.
	) + 1. - noiseIntensity, 0., 1.));
	
	color = mix(color - noiseIntensity * vec3(.25), color, blurBar);
	color = mix(color - noiseIntensity * vec3(.08), color, bar);
	color = mix(vec3(0.), texture, color);
	color.b += .042;
	
	color *= vec3(1.0 - pow(distance(uv, vec2(0.5, 0.5)), 2.1) * 2.8);
	
	return color;
}

void main (void)
{

	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;	
	depth = LinearizeDepth(depth);

	/*const float LOG2 = 1.442695;
	float z = depth*25;
	float fogFactor = exp2( -gl_Fog.density * 
					   gl_Fog.density * 
					   z * 
					   z * 
					   LOG2 );
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	//gl_FragColor = vec4(z, z, z, 1.0);
	color = mix(gl_Fog.color, color, fogFactor );


	if(transition)
	{
		float d = min(0.05*(iGlobalTime-startTime),0.501);
		if(0.05*(iGlobalTime-startTime) < 1)
		{
			vec2 iResolution = vec2(screen_width,screen_height);
			vec2 uv = gl_FragCoord.xy / iResolution.xy;
			vec2 mvt = vec2(0,0.05)*(iGlobalTime-startTime);
			mvt.x = sin((iGlobalTime-startTime))*25.0/iResolution.x;
			if(uv.x > 0.5-d && uv.x < 0.5+d && uv.y >0.5-d && uv.y < 0.5+d)
			{
				color.r = 1.0;
			}
			if(uv.x > 0.5-d+mvt.x && uv.x < 0.5+d+mvt.x  && uv.y >0.5-d+mvt.y && uv.y < 0.5+d+mvt.y)
			{
				uv-= mvt;
				color = texture2D(Texture0, uv);
			}
		}
		else
		{
			color.r = 1.0;
		}
	}
	
	

	color = vec4(oldTv(color),1.0);
	if(isContour(vec2( gl_TexCoord[0] ), xstep,ystep, depth))
		color = vec4(0,0,0,1);
	*/
	gl_FragColor = color;
}