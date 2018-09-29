#define GLM_ENABLE_EXPERIMENTAL

#include "renderer/renderer.h"
#include "chunk.h"
#include "nxlink.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "tracy/Tracy.hpp"

int main(int argc, char* argv[])
{
	initNxLink();

	sleep(1);

	Result rc = romfsInit();
	if (R_FAILED(rc))
		printf("romfsInit: %08X\n", rc);
	else
		printf("romfs Init Successful!\n");

	CRenderer* renderer = new CRenderer();
	
	if (!renderer->Init())
	{
		TRACE("Could not init renderer, exiting");
		return EXIT_FAILURE;
	}

	GLuint shaderProgram = loadShaderProgram("romfs:/shaders/chunk.vs", "romfs:/shaders/chunk.fs");


	std::vector<Chunk> chunks;
	std::vector<VisualChunk> visualChunks;

	for (int32_t x = -2; x < 2; ++x)
	{
		for (int32_t z = -2; z < 2; ++z)
		{
			Chunk chunk;
			initChunk(chunk, x, 0, z);

			VisualChunk visualChunk;
			initVisualChunk(visualChunk, chunk);

			chunks.push_back(chunk);
			visualChunks.push_back(visualChunk);
		}
	}

	float t = 0.0f;

	// Main graphics loop
	while (appletMainLoop())
	{
		t += 0.001f;

		// Get and process input
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_PLUS)
		{
			printf("Shutting down...\n");
			break;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		
		glm::mat4 matWorld = glm::eulerAngleY(t) * glm::translate(glm::mat4(1.0f), glm::vec3(-7, -7, -7));
		glm::mat4 matView = glm::lookAt(glm::vec3(-15, -13, -12), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 matProj = glm::perspectiveFov(30.0f, 1280.0f, 720.0f, 0.1f, 1000.0f);

		glm::mat4 matWorldViewProj = matProj * matView * matWorld;

		glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&matWorldViewProj);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Draw opaque stuff
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		for (const VisualChunk& visualChunk : visualChunks)
		{
			drawChunkOpaque(visualChunk);
		}

		// Draw transparency
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
		//glDepthMask(GL_FALSE);

		//drawChunkTransparent(visualChunk);

		// Render stuff!
		renderer->Render();
		renderer->Present();
		FrameMark;
	}

	renderer->Deinit();
	deinitNxLink();
	romfsExit();
	return 0;
}