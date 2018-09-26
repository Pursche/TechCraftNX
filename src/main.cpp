#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "renderer/renderer.h"

#include "nxlink.h"

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

	TRACE("Test from main!");
	// Main graphics loop
	while (appletMainLoop())
	{
		// Get and process input
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_PLUS)
			break;

		// Update our scene
		//sceneUpdate();

		// Render stuff!
		renderer->Render();
		renderer->Present();
	}

	// Deinitialize our scene
	//sceneExit();

	// Deinitialize Renderer
	renderer->Deinit();

	deinitNxLink();

	return EXIT_SUCCESS;
}