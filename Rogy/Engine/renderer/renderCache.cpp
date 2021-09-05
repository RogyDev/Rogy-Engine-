#include "renderCache.h"

RCache::RCache()
{
}

RCache::~RCache()
{
}

void RCache::SetCamera(Camera& cam)
{
	pbrShader->use();
	if (last_proj != cam.projectionMatrix)
	{
		last_proj = cam.GetProjectionMatrix();
		pbrShader->SetMat4("projection", last_proj);
	}
	if (last_view != cam.viewMatrix)
	{
		last_view = cam.GetViewMatrix();
		pbrShader->SetMat4("view", last_view);
	}
	if (last_CamPos != cam.transform.Position)
	{
		last_CamPos = cam.transform.Position;
		pbrShader->SetVec3("CamPos", last_CamPos);
	}
}	

void RCache::BindTexture(ShaderTextureType type, GLuint tex_id)
{
	if (type == TEX_IRRADIANCE_MAP)
	{
	//	if (irra != tex_id)
		{
			irra = tex_id;
			glActiveTexture(GL_TEXTURE0 + TEX_IRRADIANCE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
		}
	}
	else if (type == TEX_PREFILTER_MAP)
	{
		//if (pref != tex_id)
		{
			pref = tex_id;
			glActiveTexture(GL_TEXTURE0 + TEX_PREFILTER_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
		}
	}
}

void RCache::BindTexAlbedo(GLuint tex_id)
{
	if (m_albedo != tex_id)
	{
		m_albedo = tex_id;
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
}

void RCache::BindTexMetal(GLuint tex_id)
{
	if (m_metal != tex_id)
	{
		m_metal = tex_id;
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
}

void RCache::BindTexRough(GLuint tex_id)
{
	if (m_rough != tex_id)
	{
		m_rough = tex_id;
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
}

void RCache::BindTexEmmi(GLuint tex_id)
{
	//if (m_emm != tex_id)
	{
		m_emm = tex_id;
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
}

void RCache::BindTexNormal(GLuint tex_id)
{
	if (m_norm != tex_id)
	{
		m_norm = tex_id;
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
}

