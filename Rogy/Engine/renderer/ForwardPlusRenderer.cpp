#include "ForwardPlusRenderer.h"

ForwardPlusRenderer::ForwardPlusRenderer()
{
}

ForwardPlusRenderer::~ForwardPlusRenderer()
{
}

bool ForwardPlusRenderer::Initialize(unsigned int H, unsigned int W)
{
	lightCullingShader.loadComputeShader("core/shaders/ForwardPlus/light_culling.rcomp");
	depthShader.loadShader("core/shaders/ForwardPlus/depth.rsh");
	light_accumulation.loadShader("core/shaders/ForwardPlus/light_accumulation.rsh");
	debugShader.loadShader("core/shaders/ForwardPlus/debug.rsh");
	depthdebugShader.loadShader("core/shaders/ForwardPlus/depthdebug.rsh");

	light_accumulation.use();

	lightCullingShader.use();
	glUniform1i(glGetUniformLocation(lightCullingShader.Program, "depthMap"), 4);

	return SetResolution(H, W);
}

glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937 gen) 
{
	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);

	glm::vec3 position = glm::vec3(0.0);
	for (int i = 0; i < 3; i++) {
		float min = LIGHT_MIN_BOUNDS[i];
		float max = LIGHT_MAX_BOUNDS[i];
		position[i] = (GLfloat)dis(gen) * (max - min) + min;
	}

	return position;
}
bool ForwardPlusRenderer::SetResolution(unsigned int H, unsigned int W)
{
	//return false;
	if(W == SCREEN_SIZE.x && H == SCREEN_SIZE.y)
		return true;
	SCREEN_SIZE.x = W;
	SCREEN_SIZE.y = H;

	//if (!first)
	//	Cleanup();

	// Set the default values for the light buffer
	//SetupLights();
	/*std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	plight *pointLights = (plight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		plight &light = pointLights[i];
		light.position = glm::vec4(RandomPosition(dis, gen), 1.0f);
		light.color = glm::vec4(1.0f + dis(gen), 1.0f + dis(gen), 1.0f + dis(gen), 1.0f);
		light.paddingAndRadius = glm::vec4(glm::vec3(0.0f), 20);
	}
	*/
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// So we need to create a depth map FBO
	// This will be used in the depth pass
	// Create a depth map frame buffer object and texture
	//glGenFramebuffers(1, &depthMapFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	/*glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_SIZE.x, SCREEN_SIZE.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);
	GLuint a;
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)	
	glGenRenderbuffers(1, &a);
	glBindRenderbuffer(GL_RENDERBUFFER, a);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_SIZE.x, SCREEN_SIZE.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, a);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:a: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_SIZE.x, SCREEN_SIZE.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:aaa: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)
	workGroupsX = (SCREEN_SIZE.x + (SCREEN_SIZE.x % 16)) / 16;
	workGroupsY = (SCREEN_SIZE.y + (SCREEN_SIZE.y % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;

	// Generate our shader storage buffers
	glGenBuffers(1, &lightBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

	// Bind visible light indices buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * MAX_LIGHTS_PER_TILE, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	std::cout << "ForwardPlusRenderer::SetResolution " << SCREEN_SIZE.x << ":" << SCREEN_SIZE.y << std::endl;
	std::cout << "ForwardPlusRenderer::workGroups " << workGroupsX << ":" << workGroupsY << std::endl;
	std::cout << "ForwardPlusRenderer::numberOfTiles " << numberOfTiles << std::endl;

	first = false;
	return true;
}

void ForwardPlusRenderer::UpdateLights(std::vector<PointLight*>& m_PointLights)
{
	//return;
	//if (m_PointLights.empty()) return;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	plight *pointLights = (plight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (size_t i = 0; /*i < NUM_LIGHTS &&*/  i < m_PointLights.size(); i++) {
		plight &light = pointLights[i];
		PointLight* p = m_PointLights[i];
		light.position = glm::vec4(p->Position, 0.0f);
		light.color = glm::vec4(p->Color, 1.0f);
		light.paddingAndRadius = glm::vec4(glm::vec3(0.0f), p->Raduis);
	}

	lightCount = m_PointLights.size();

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ForwardPlusRenderer::Cleanup()
{
	glDeleteBuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
	glDeleteBuffers(1, &lightBuffer);
	glDeleteBuffers(1, &visibleLightIndicesBuffer);
}

void ForwardPlusRenderer::DepthPass(Camera & cam)
{
	// Step 1: Render the depth of the scene to a depth map depthShader
	depthShader.use();
	depthShader.SetMat4("projection", cam.GetProjectionMatrix());
	depthShader.SetMat4("view", cam.GetViewMatrix());
	depthShader.SetMat4("invP", glm::inverse(cam.GetProjectionMatrix()));
	// Bind the depth map's frame buffer and draw the depth map to it
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardPlusRenderer::SetModel(glm::mat4 model)
{
	depthShader.SetMat4("model", model);
}

void ForwardPlusRenderer::LightCullingPass(Camera& cam)
{
	// Unbind depthMapFBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Step 2: Perform light culling on point lights in the scene
	lightCullingShader.use();
	glUniform1i(glGetUniformLocation(lightCullingShader.Program, "lightCount"), lightCount);
	glUniform2iv(glGetUniformLocation(lightCullingShader.Program, "screenSize"), 1, &SCREEN_SIZE[0]);

	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(cam.GetProjectionMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glUniform1i(glGetUniformLocation(lightCullingShader.Program, "depthMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(workGroupsX, workGroupsY, 1);

	// Unbind the depth map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
