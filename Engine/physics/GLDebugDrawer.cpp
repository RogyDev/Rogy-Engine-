#include "GLDebugDrawer.h"
//#include "..\gizmos\debug_gizmos.h"
CDebugDraw::CDebugDraw(void)
{

}
CDebugDraw::~CDebugDraw(void)
{

}

void CDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	//std::cout << "draw line\n";
	_LINE a;
	a.from = from;
	a.to = to;
	LINES.push_back(a);
	/*
	_COLOR b;
	b.col = color;

	
	COLORS.push_back(b);*/
	//const ddVec3 mfrom = { from.getX(), from.getY(), from.getZ() };
	//const ddVec3 mto = { to.getX(), to.getY(), to.getZ() };
	//dd::line(mfrom, mto, dd::colors::Green);
}

void CDebugDraw::doDrawing()
{
	/*glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, LINES.size() * sizeof(_LINE), &LINES[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, COLORS.size() * sizeof(_COLOR), &COLORS[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

}

void CDebugDraw::cleanDrawing()
{
	// delete buffers
	/*glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &vao);*/
}

void    CDebugDraw::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void    CDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
	//glRasterPos3f(location.x(),  location.y(),  location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void    CDebugDraw::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}

void    CDebugDraw::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	{
		//btVector3 to=pointOnB+normalOnB*distance;
		//const btVector3&from = pointOnB;
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

		//GLDebugDrawer::drawLine(from, to, color);

		//glRasterPos3f(from.x(),  from.y(),  from.z());
		//char buf[12];
		//sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
	}
}

int CDebugDraw::getDebugMode() const
{
	return m_debugMode;
}