#include <GL/glew.h>
#include <SDL.h>
#include "windows.h"

#include <iostream>

#include "OpenGL/Renderer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"
#include "Maths/Matrix2D.h"

#include <cstdio>
#include "Managers/InputManager.h"
#include "Managers/Frame Rate Controller.h"
#include "Managers/ResourceManager.h"
#include "Managers/GameObject.h"
#include "Managers/GameObjectManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/CollisionManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/Components/Component.h"

#include "Defines.h"

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

#pragma comment(lib, "legacy_stdio_definitions.lib")


Input_Manager *gpInputManager;
FrameRateController *gpFrameRateController;
ResourceManager *gpResourceManager;
GameObjectManager *gpGameObjectManager;
ObjectFactory *gpObjectFactory;
CollisionManager *gpCollisionManager;
PhysicsManager *gpPhysicsManager;

Shader *gpShader;
Matrix3D* gpProj;


int main(int argc, char* args[])
{
	//-----SDL

	if (AllocConsole())
	{
		FILE* file;

		freopen_s(&file, "CONOUT$", "wt", stdout);
		freopen_s(&file, "CONOUT$", "wt", stderr);
		freopen_s(&file, "CONOUT$", "wt", stdin);

		SetConsoleTitle("SDL 2.0 Test");
	}

	SDL_Window *pWindow;
	int error = 0;
	int posX = 0, posY = 0;
	bool appIsRunning = true;
	
	// Initialize SDL
	if ((error = SDL_Init(SDL_INIT_VIDEO)) < 0)
	{
		printf("Couldn't initialize SDL, error %i\n", error);
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	pWindow = SDL_CreateWindow("Gunter",			// window title
		SDL_WINDOWPOS_CENTERED,						// initial x position
		SDL_WINDOWPOS_CENTERED,						// initial y position
		SCREEN_WIDTH,								// width, in pixels
		SCREEN_HEIGHT,								// height, in pixels
		SDL_WINDOW_OPENGL);

	// Check that the window was successfully made
	if (NULL == pWindow)
	{
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_GLContext gContext = SDL_GL_CreateContext(pWindow); 
	if (gContext == NULL) 
	{ 
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError()); 
	}
	else { 
		//Initialize GLEW
		glewExperimental = true;
		if(glewInit() != GLEW_OK )
		{
			printf( "Error initializing GLEW!\n"); 
		}
	}
	//------ SDL End
	
	gpInputManager = new Input_Manager();
	gpFrameRateController = new FrameRateController(60);
	gpGameObjectManager = new GameObjectManager();
	gpResourceManager = new ResourceManager();
	gpObjectFactory = new ObjectFactory();
	gpCollisionManager = new CollisionManager();
	gpPhysicsManager = new PhysicsManager();

	//-----
	printf("%s\n",glGetString(GL_VERSION));
	{
		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;

		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		va.AddBuffer(vb);
		va.Push(2, GL_FLOAT, sizeof(float));
		va.Push(2, GL_FLOAT, sizeof(float));
		va.AddLayout();

		IndexBuffer ib(indices, 6);

		gpProj = new Matrix3D();
		Matrix3DOrtho(gpProj, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0f, 1.0f);
	//-----
		
		
		gpShader = new Shader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
		gpShader->Bind();

		gpObjectFactory->LoadLevel("level.json");

	//---- Background -----
		GameObject background(NO_OBJECT);
		Component *pNewComponent = nullptr;

		pNewComponent = background.AddComponent(TRANSFORM);
		pNewComponent = background.AddComponent(SPRITE);

		background.SetTransform(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH - 150.0f, SCREEN_HEIGHT - 150.0f, 0.0f);
		background.SetSprite("res/textures/background1.png");
	//-----

	//----- Wall ------
		GameObject wall(WALL);
		
		pNewComponent = wall.AddComponent(TRANSFORM);
		pNewComponent = wall.AddComponent(SPRITE);

		wall.SetTransform(95.0f, SCREEN_HEIGHT / 2, 40.0f, SCREEN_HEIGHT - 150.0f, 0.0f);
		wall.SetSprite("res/textures/wall.png");

	//-------------
	//----- Health Bar ------
/*
		GameObject healthbar(NO_OBJECT);

		pNewComponent = healthbar.AddComponent(TRANSFORM);
		pNewComponent = healthbar.AddComponent(SPRITE);

		healthbar.SetTransform( 170.0f, SCREEN_HEIGHT - 50.0f, 300.0f, 60.0f, 0.0f);
		healthbar.SetSprite("res/textures/hpbar.png");
		*/

	//-----

	//------
		Renderer renderer;
	
		/* Loop until the user closes the window */
		while (true == appIsRunning)
		{
			gpFrameRateController->FrameStart();
			//printf("Frame time = %f\n", gpFrameRateController->GetFrameTime());
			
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					appIsRunning = false;
				}
			}
			
			gpInputManager->Update();


			if (gpInputManager->IsPressed(SDL_SCANCODE_ESCAPE))
			{
				appIsRunning = false;
			}

			gpShader->Bind();

			/* Render here */
			renderer.Clear();

			background.Update();
			renderer.Draw(va, ib, *gpShader);
			
			wall.Update();
			renderer.Draw(va, ib, *gpShader);

		//	healthbar.Update();
		//	renderer.Draw(va, ib, *gpShader);

			for(unsigned int i = 0; i < gpGameObjectManager->mGameObjects.size(); ++i)
			{
				gpGameObjectManager->mGameObjects[i]->Update();

				/* Draw call*/
				renderer.Draw(va, ib, *gpShader);
			}

			gpPhysicsManager->Update(gpFrameRateController->GetFrameTime());

			SDL_GL_SwapWindow(pWindow);

			gpFrameRateController->FrameEnd();
		}
	}


	free(gpInputManager);
	free(gpFrameRateController);
	free(gpResourceManager);
	free(gpGameObjectManager);
	free(gpCollisionManager);
	free(gpObjectFactory);
	free(gpPhysicsManager);

	// Close and destroy the window
	SDL_DestroyWindow(pWindow);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}