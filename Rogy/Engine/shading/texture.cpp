#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL\glew.h>

#include "stb\stb_image.h"
#include "texture.h"


int Texture::GetRed(int x, int y)
{
	unsigned char* pixel = tData + y * texWidth * 4 + x * 4;
	return pixel[0] ;
}

int Texture::GetGreen(int x, int y)
{
	unsigned char* pixel = tData + y * texWidth * 4 + x * 4;
	return pixel[1];
}

int Texture::GetBlue(int x, int y)
{
	/*unsigned char* pixelOffset = tData + (x + 255 * z) * 3;
	unsigned char r = pixelOffset[0];
	unsigned char g = pixelOffset[1];
	unsigned char b = pixelOffset[2];
	return ((int)r + (int)b + (int)g) / 3;*/
	return tData[4 * (y * texWidth + x) + 0];
	//unsigned char* pixel = tData + y * texWidth * 4 + x * 4;
	//return ((int)pixel[2] + (int)pixel[1] + (int)pixel[0])/3 ;
}

int Texture::GetAlpha(int x, int y)
{
	unsigned char* pixel = tData + y * texWidth * 4 + x * 4;
	return pixel[3];
}

Texture::Texture()
{

}


Texture::~Texture()
{
    glDeleteTextures(1, &this->texID);
}

std::string Texture::getTexPath()
{
	return this->TPath;
}

bool Texture::setTexture(const char* texPath, std::string tex_Name, bool texFlip, bool keepData)
{
    this->texType = GL_TEXTURE_2D;

    std::string tempPath = std::string(texPath);
	TPath = tempPath;
	//std::cout << TPath.c_str() << std::endl;

    if(texFlip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &this->texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texID);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoFilterLevel);  // Request the maximum level of anisotropy the GPU used can support and use it
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisoFilterLevel);

    int width, height, numComponents;
    unsigned char* texData = stbi_load(tempPath.c_str(), &width, &height, &numComponents, 0);

    this->texWidth = width;
    this->texHeight = height;
    this->texComponents = numComponents;
    texName = tex_Name;

    if (texData)
    {
        if (numComponents == 1)
            this->texFormat = GL_RED;
        else if (numComponents == 3)
            this->texFormat = GL_RGB;
        else if (numComponents == 4)
            this->texFormat = GL_RGBA;
        this->texInternalFormat = this->texFormat;

        glTexImage2D(GL_TEXTURE_2D, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_UNSIGNED_BYTE, texData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // Need AF to get ride of the blur on textures
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else
    {
        std::cerr << "TEXTURE LOADING FAILED : " << texPath << std::endl;
		return false;
    }

	if (keepData)
		tData = texData;
	else
		stbi_image_free(texData);

    glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}


void Texture::setTextureHDR(const char* texPath, std::string texName, bool texFlip)
{
    this->texType = GL_TEXTURE_2D;

    std::string tempPath = std::string(texPath);

    if(texFlip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &this->texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texID);

    if(stbi_is_hdr(tempPath.c_str()))
    {
        int width, height, numComponents;
        float* texData = stbi_loadf(tempPath.c_str(), &width, &height, &numComponents, 0);

        this->texWidth = width;
        this->texHeight = height;
        this->texComponents = numComponents;
        this->texName = texName;

        if (texData)
        {
            // Need a higher precision format for HDR to not lose informations, thus 32bits floating point
            if (numComponents == 3)
            {
                this->texInternalFormat = GL_RGB32F;
                this->texFormat = GL_RGB;
            }
            else if (numComponents == 4)
            {
                this->texInternalFormat = GL_RGBA32F;
                this->texFormat = GL_RGBA;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_FLOAT, texData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenerateMipmap(GL_TEXTURE_2D);
        }

        else
        {
            std::cerr << "HDR TEXTURE - FAILED LOADING : " << texPath << std::endl;
        }

        stbi_image_free(texData);
    }

    else
    {
        std::cerr << "HDR TEXTURE - FILE IS NOT HDR : " << texPath << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::setTextureHDR(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter)
{
	this->texType = GL_TEXTURE_2D;

	glGenTextures(1, &this->texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texID);

	this->texWidth = width;
	this->texHeight = height;
	this->texFormat = format;
	this->texInternalFormat = internalFormat;

	if (format == GL_RED)
		this->texComponents = 1;
	else if (format == GL_RG)
		this->texComponents = 2;
	else if (format == GL_RGB)
		this->texComponents = 3;
	else if (format == GL_RGBA)
		this->texComponents = 4;

	glTexImage2D(GL_TEXTURE_2D, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::setTextureCube(std::vector<const char*>& faces, bool texFlip)
{
    this->texType = GL_TEXTURE_CUBE_MAP;

    std::vector<std::string> cubemapFaces;

    for (GLuint j = 0; j < faces.size(); j++)
    {
        std::string tempPath = std::string(faces[j]);
        cubemapFaces.push_back(tempPath);
    }

    if(texFlip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &this->texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(this->texType, this->texID);

    int width, height, numComponents;
    unsigned char* texData;

    for(GLuint i = 0; i < 6; i++)
    {
        texData = stbi_load(cubemapFaces[i].c_str(), &width, &height, &numComponents, 0);

        if(this->texWidth == NULL && this->texHeight == NULL && this->texComponents == NULL)
        {
            this->texWidth = width;
            this->texHeight = height;
            this->texComponents = numComponents;
        }

        if (texData)
        {
            if (numComponents == 1)
                this->texFormat = GL_RED;
            else if (numComponents == 3)
                this->texFormat = GL_RGB;
            else if (numComponents == 4)
                this->texFormat = GL_RGBA;
            this->texInternalFormat = this->texFormat;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_UNSIGNED_BYTE, texData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenerateMipmap(this->texType);
        }

        else
        {
            std::cerr << "CUBEMAP TEXTURE - FAILED LOADING : " << cubemapFaces[i] << std::endl;
        }

        stbi_image_free(texData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(this->texType, 0);
}


void Texture::setTextureCube(GLuint width, GLenum format, GLenum internalFormat, GLenum type, GLenum minFilter)
{
    this->texType = GL_TEXTURE_CUBE_MAP;

    glGenTextures(1, &this->texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(this->texType, this->texID);

    for(GLuint i = 0; i < 6; ++i)
    {
        if(this->texWidth == NULL && this->texHeight == NULL && this->texComponents == NULL)
        {
            this->texWidth = width;
            this->texHeight = width;
            this->texFormat = format;
            this->texInternalFormat = internalFormat;
        }

        if (format == GL_RED)
            this->texComponents = 1;
        else if (format == GL_RGB)
            this->texComponents = 3;
        else if (format == GL_RGBA)
            this->texComponents = 4;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, type, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(this->texType, 0);
}


void Texture::computeTexMipmap()
{
    glBindTexture(this->texType, this->texID);
    glGenerateMipmap(this->texType);
}


GLuint Texture::getTexID()
{
    return this->texID;
}


GLuint Texture::getTexWidth()
{
    return this->texWidth;
}


GLuint Texture::getTexHeight()
{
    return this->texHeight;
}


std::string Texture::getTexName()
{
    return this->texName;
}


void Texture::useTexture()
{
    glBindTexture(this->texType, this->texID);
}
