
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;


void main()
{
    WorldPos = aPos;
	
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(aPos, 1.0);

	gl_Position = clipPos.xyww;
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;

in vec3 WorldPos;

uniform samplerCube CloudsTex;
uniform bool UseClouds;
uniform vec3 sun_direction;
uniform vec3 skyColor;


uniform vec3 A, B, C, D, E, F, G, H, I, Z;


vec3 hosek_wilkie(float cos_theta, float gamma, float cos_gamma)
{
	vec3 chi = (1 + cos_gamma * cos_gamma) / pow(1 + H * H - 2 * cos_gamma * H, vec3(1.5));
    return (1 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}
vec3 hosek_wilkie_sky_rgb(vec3 v, vec3 sun_dir)
{
    float cos_theta = clamp(v.y, 0, 1);
	float cos_gamma = clamp(dot(v, sun_dir), 0, 1);
	float gamma_ = acos(cos_gamma);

	vec3 R = Z * hosek_wilkie(cos_theta, gamma_, cos_gamma);
    return R;
}

//---------NOISE GENERATION------------
//Noise generation based on a simple hash, to ensure that if a given point on the dome
//(after taking into account the rotation of the sky) is a star, it remains a star all night long
float Hash( float n ){
        return fract( (1.0 + sin(n)) * 415.92653);
}
float Noise3d( vec3 x ){
    float xhash = Hash(round(400*x.x) * 37.0);
    float yhash = Hash(round(400*x.y) * 57.0);
    float zhash = Hash(round(400*x.z) * 67.0);
    return fract(xhash + yhash + zhash);
}

void main()
{
    vec3 color = hosek_wilkie_sky_rgb(normalize(WorldPos), sun_direction);
    //vec3 colorNight = vec3(0.02, 0.03, 0.09);
    float dist = dot(sun_direction,WorldPos);

   // color *= (sun_direction.y + 0.4) + 0.1;
    //color *= vec3(0.035, 0.035, 0.12) *(-sun_direction.y+0.1);
    
    //vec3 color = mix(colorNight,colorLight,dist);

    // Stars
    if(sun_direction.y < 0.01)
    {
        //Night or dawn
        float noise = Noise3d(normalize(WorldPos));
        float threshold = 0.998;
        //We generate a random value between 0 and 1
        float star_intensity = noise;
        //And we apply a threshold to keep only the brightest areas
        if (star_intensity >= threshold){
            //We compute the star intensity
            star_intensity = pow((star_intensity - threshold)/(1.0 - threshold), 4.0) *(-sun_direction.y+0.1);
            color += vec3(star_intensity);
        }
    }

    //Sun
    float radius = length(normalize(WorldPos) - sun_direction);
    if(radius < 0.05){//We are in the area of the sky which is covered by the sun
        float time1 = clamp(sun_direction.y,0.01,1);
        radius = radius/0.018;
        if(radius < 1.0-0.001){//< we need a small bias to avoid flickering on the border of the texture
            //We read the alpha value from a texture where x = radius and y=height in the sky (~time)
            vec4 sun_color = vec4(1,1,1,1) * 0.2;
            color += mix(color,sun_color.rgb,sun_color.a);
        }
    }

    if(UseClouds)
	{
		vec3 cloud = color * texture(CloudsTex, WorldPos).rgb * 2;
		color += cloud;
	}
    // Apply exposure.
    //color = 1.0 - exp(-1.0 * color);
    //if(mPow)
    {
        color.r = pow(color.r, 1.2);
        color.g = pow(color.g, 1.2);
        color.b = pow(color.b, 1.2);
    }
    FragColor = vec4(color, 1.0);
    gPosition = vec3(9999999.0, 9999999.0, 9999999.0);
}

#endif