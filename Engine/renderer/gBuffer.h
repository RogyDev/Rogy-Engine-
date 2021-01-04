#ifndef R_GEOMETRY_BUFFER

#include "GL\glew.h"
#include <iostream>

class GeoBuffer
{
public:
	GeoBuffer();
	~GeoBuffer();

	// configure g-buffer framebuffer
	// ------------------------------
	unsigned int gBuffer;
	unsigned int gPosition, gNormal;
	unsigned int rboDepth;

	int SCR_H, SCR_W;

	void Create(int w, int h);
	void Clear();
	void Bind();
	void UnBind();

private:
	bool initilazed = false;
};

#endif // !R_GEOMETRY_BUFFER
