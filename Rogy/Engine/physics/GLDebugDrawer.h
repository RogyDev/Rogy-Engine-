#ifndef GL_DEBUG_DRAWER_H
#define GL_DEBUG_DRAWER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <vector>

class CDebugDraw :
	public btIDebugDraw
{
private:
	int m_debugMode;
public:
	CDebugDraw(void);
	virtual ~CDebugDraw(void);

	struct _LINE {
		btVector3 from;
		btVector3 to;
	};

	std::vector<_LINE> LINES;

	struct _COLOR {
		btVector3 col;
	};

	std::vector<_COLOR> COLORS;

	GLuint vao, vbo[2];

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location, const char* textString);

	virtual void setDebugMode(int m_debugMode);

	virtual int getDebugMode() const;

	void doDrawing();
	void cleanDrawing();
};
#endif