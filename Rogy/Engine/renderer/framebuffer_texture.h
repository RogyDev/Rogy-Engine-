#ifndef R_FRAMEBUFFER_H
#define R_FRAMEBUFFER_H

#include <iostream>
#include <GL\glew.h>

class FrameBufferTex {

public:
	FrameBufferTex(){}
	~FrameBufferTex(){}

	void Generate(int SCR_W, int SCR_H, bool For3dRendering = true)
	{
		if (scr_w == SCR_W && scr_h == SCR_H) return;

		scr_w = SCR_W;
		scr_h = SCR_H;

		for3dRendering = For3dRendering;

		Clear();

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)scr_w, (int)scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		if (for3dRendering) {
			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)	
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)scr_w, (int)scr_h);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		init = true;
	}

	unsigned int GetBuffer()
	{
		return framebuffer;
	}
	void Bind()
	{
		if (!init) return;
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}
	void Unbind(int to = 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, to);
	}

	void UseTexture()
	{
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	}

	void Clear()
	{
		if (!init) return;
		//std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is CLEAR" << std::endl;
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &textureColorbuffer);
		if (!for3dRendering)
			glDeleteRenderbuffers(1, &rbo);
		init = false;
	}
	int scr_w, scr_h;
	unsigned int rbo, framebuffer, textureColorbuffer;
private:
	bool init = false;
	
	bool for3dRendering = false;
};

#endif