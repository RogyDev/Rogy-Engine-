#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL\glew.h>


class Texture
{
    public:
        GLuint texID, texWidth, texHeight, texComponents;
        GLfloat anisoFilterLevel;
        GLenum texType, texInternalFormat, texFormat;
        std::string texName;
		std::string TPath;

		unsigned char* tData;
		int GetRed(int x, int y);
		int GetGreen(int x, int y);
		int GetBlue(int x, int y);
		int GetAlpha(int x, int y);

        Texture();
        ~Texture();
		bool setTexture(const char* texPath, std::string tex_Name, bool texFlip = false, bool keepData = false);
        void setTextureHDR(const char* texPath, std::string texName, bool texFlip);
		void setTextureHDR(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter);
        void setTextureCube(std::vector<const char*>& faces, bool texFlip);
        void setTextureCube(GLuint width, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter);
        void computeTexMipmap();
        GLuint getTexID();
        GLuint getTexWidth();
        GLuint getTexHeight();
        std::string getTexName();
		std::string getTexPath();
        void useTexture();
};


#endif
