#define GLM_ENABLE_EXPERIMENTAL

#include "renderer/renderer.h"
#include "chunk.h"
#include "nxlink.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

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

	VisualChunk::init();

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

	float cameraYaw = 0.0f;
	float cameraPitch = 0.0f;
	glm::vec3 cameraPos(0.0f);

	float movementSpeed = 0.5f;
	float lookSpeed = 0.07f;

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

		if (kDown & KEY_A)
		{
			VisualChunk::s_triangleFilteringEnabled = !VisualChunk::s_triangleFilteringEnabled;
		}

		if (kDown & KEY_B)
		{
			VisualChunk::s_freezeCulling = !VisualChunk::s_freezeCulling;
		}

		// Read joysticks
		JoystickPosition joyLeft, joyRight;
        hidJoystickRead(&joyLeft, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
        hidJoystickRead(&joyRight, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);

		const float moveRight = static_cast<float>(joyLeft.dx) / static_cast<float>(JOYSTICK_MAX);
		const float moveForward = static_cast<float>(joyLeft.dy) / static_cast<float>(JOYSTICK_MAX);
		const float lookRight = static_cast<float>(joyRight.dx) / static_cast<float>(JOYSTICK_MAX);
		const float lookUp = static_cast<float>(joyRight.dy) / static_cast<float>(JOYSTICK_MAX);

		cameraYaw += lookRight * lookSpeed;
		cameraPitch -= lookUp * lookSpeed;
		cameraPitch = std::min(cameraPitch, glm::half_pi<float>());
		cameraPitch = std::max(cameraPitch, -glm::half_pi<float>());

		cameraPos.x += (moveForward * sin(cameraYaw) * cos(cameraPitch) + moveRight * cos(cameraYaw)) * movementSpeed;
		cameraPos.z += (moveForward * cos(cameraYaw) * cos(cameraPitch) + moveRight * -sin(cameraYaw)) * movementSpeed;
		cameraPos.y -= moveForward * sin(cameraPitch) * movementSpeed;

		glm::mat4 cameraMatrix = glm::translate(glm::mat4(1.0f), cameraPos) * glm::eulerAngleYX(cameraYaw, cameraPitch);

		glm::mat4 matView = glm::inverse(cameraMatrix);
		glm::mat4 matProj = glm::perspectiveFovLH(glm::radians(80.0f), 1280.0f, 720.0f, 0.1f, 1000.0f);

		glm::mat4 matViewProj = matProj * matView;

		CullChunkParams cullParams;
		cullParams.matViewProj = matViewProj;

		// Cull chunks
		for (const VisualChunk& visualChunk : visualChunks)
		{
			cullChunk(visualChunk, cullParams);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(matViewProj));

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
	}

	glDeleteProgram(shaderProgram);

	VisualChunk::deinit();

	renderer->Deinit();
	deinitNxLink();
	romfsExit();
	return 0;
}