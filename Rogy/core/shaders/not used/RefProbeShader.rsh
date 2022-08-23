

// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

out float FragColor;

in vec2 TexCoords;

struct RefProbe 
{
	vec3 mBoxMin;
	vec3 mBoxMax;
};

uniform vec3 pMins;
uniform vec3 pMaxs;

uniform RefProbe env_probes[5];
uniform int RefProbeCount;
uniform sampler2D gPosition;
uniform float minX;
uniform float maxX;
uniform float m_vals[6 * 5];

float insideBox3d(vec3 vpos, vec3 bmin, vec3 bmax)
{
	vec3 s = step(bmin, vpos) - step(bmax, vpos);
	return s.x * s.y * s.z;
}


void main()
{
	vec3 position = texture(gPosition, TexCoords).xyz;

	float RefProbeIndex = 0.0;

	for(int i = 0; i < 5; i++)
	{
		int ii = i * 6;
		RefProbeIndex += i * insideBox3d(position, vec3(m_vals[ii +  0], m_vals[ii +  1], m_vals[ii +  2]), vec3(m_vals[ii +  3], m_vals[ii +  4], m_vals[ii +  5]));
	}
	
	FragColor = RefProbeIndex / 4.0;

	//int i = 0;
	//float minX_ = m_vals[0];
	//float maxX_ = m_vals[3];
	
	//if((position.x > minX_ && position.x < maxX_))
	//if(insideBox3d(position, env_probes[i].mBoxMin, env_probes[i].mBoxMax) == 1)
	//int ii = 1 * 6;
	/*if(insideBox3d(position, vec3(m_vals[ii +  0], m_vals[ii +  1], m_vals[ii +  2]), vec3(m_vals[ii +  3], m_vals[ii +  4], m_vals[ii +  5])) == 1)
	{
		//RefProbeIndex = 4.0;
		FragColor = 1.0;
	}
	else
	{
		FragColor = 0.3;
	}*/

	//FragColor = 0.3 + 0.7 * insideBox3d(position, vec3(m_vals[ii +  0], m_vals[ii +  1], m_vals[ii +  2]), vec3(m_vals[ii +  3], m_vals[ii +  4], m_vals[ii +  5]));

    
}

#endif