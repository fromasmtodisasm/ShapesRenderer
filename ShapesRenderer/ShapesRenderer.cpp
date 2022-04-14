// ShapesRenderer.cpp : Defines the entry point for the application.
//

#include "ShapesRenderer.h"
#include <vector>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_opengl2.h>
#include <imgui_impl_sdl.h>

#include "Camera.h"

namespace Test
{

	struct SDrawParams
	{
		STransform Transform;
		Vec3       Color;
	};

	struct IShape
	{
		~IShape() {}
		virtual void Draw(const SDrawParams& DrawParams) = 0;
	};

	struct SShape
	{
		IShape* Shape;
		Vec3       Color;
		STransform Transform;
		void       Draw() { Shape->Draw({ Transform, Color }); }
	};

	class CBaseShape : IShape
	{
		// Inherited via IShape
		virtual void Draw(const SDrawParams& DrawParams) override
		{
		}
	};

	int CreateShaderFromDisk(std::string_view Path)
	{
		return 0;
	}

	float GetTime()
	{
		return 0;
	}

	class CShapeRenderer
	{
	public:
		CShapeRenderer(int NumShapes)
		{
			m_ObjectShader = CreateShaderFromDisk("ObjectShader.glsl");
		}
		void DrawShapes()
		{
			for (const auto& shape : m_Shapes)
			{
				shape->Draw();
			}
		}
		void Update()
		{
			auto t = GetTime();
			for (auto shape : m_Shapes)
			{
				shape->Color = glm::sin(Vec3(t, t, t));
			}
		}

	private:
		std::vector<SShape*> m_Shapes;
		int                  m_ObjectShader;
	};

	class Circle : public IShape
	{
		// Inherited via IShape
		virtual void Draw(const SDrawParams& DrawParams) override
		{
		}
	};

	class CInput
	{
	};

	int main()
	{
		bool           Quit = false;
		CShapeRenderer Renderer(10);

		while (!Quit)
		{
			Renderer.Update();

			Renderer.DrawShapes();
		}

		return 0;
	}
} // namespace Test

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

typedef int32_t i32;
typedef uint32_t u32;
typedef int32_t b32;

#define WinWidth 1000
#define WinHeight 1000

int main(int ArgCount, char** Args)
{

	u32 WindowFlags = SDL_WINDOW_OPENGL;
	SDL_Window* Window = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WinWidth, WinHeight, WindowFlags);
	assert(Window);
	SDL_GLContext Context = SDL_GL_CreateContext(Window);

	b32 Running = 1;
	b32 FullScreen = 0;

	CCamera Camera({ 0,0,0 }, { 0,1,0 }, 0, 0);
	Camera.SetViewport(Vec4(0, 0, WinWidth, WinHeight));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	auto ctx = ImGui::CreateContext();

	auto quadric = gluNewQuadric();

	bool mouseDown[2] = { false };

	bool Keys[512] = {};
	auto isDown = [&Keys](int K) {
		return Keys[K];
	};

	auto isLeft = [&isDown]() {return isDown(SDLK_a); };
	auto isRight = [&isDown]() {return isDown(SDLK_d); };
	auto isFront = [&isDown]() {return isDown(SDLK_w); };
	auto isBack = [&isDown]() {return isDown(SDLK_s); };

	float mouseSensitivity = 0.2;
	//******************************************* // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(Window, Context);
	ImGui_ImplOpenGL2_Init();

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	while (Running)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf(&Camera.GetProjectionMatrix()[0][0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixf(&Camera.GetViewMatrix()[0][0]);


		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			ImGui_ImplSDL2_ProcessEvent(&Event);
			if (Event.type == SDL_KEYDOWN)
			{
				Keys[Event.key.keysym.sym] = true;
			}
			if (Event.type == SDL_KEYUP)
			{
				Keys[Event.key.keysym.sym] = false;
			}
			if (Event.type == SDL_KEYDOWN)
			{
				switch (Event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					Running = 0;
					break;
				case 'f':
					FullScreen = !FullScreen;
					if (FullScreen)
					{
						SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					else
					{
						SDL_SetWindowFullscreen(Window, WindowFlags);
					}
					break;
				default:
					break;
				}
			}
			if (!ImGui::GetIO().WantCaptureMouse)
			{
				if (Event.type == SDL_MOUSEBUTTONDOWN)
				{
					mouseDown[Event.button.button] = true;
				}
				else if (Event.type == SDL_MOUSEBUTTONUP)
				{
					mouseDown[Event.button.button] = false;
				}
				if (Event.type == SDL_MOUSEMOTION && mouseDown[1])
				{
					auto x = mouseSensitivity * Event.motion.xrel;
					auto y = mouseSensitivity * Event.motion.yrel;
					Camera.ProcessMouseMovement(x, y);
				}
			}
			if (isLeft()) { Camera.ProcessKeyboard(Movement::LEFT, 0.016, 1); }
			if (isRight()) { Camera.ProcessKeyboard(Movement::RIGHT, 0.016, 1); }
			if (isFront()) { Camera.ProcessKeyboard(Movement::FORWARD, 0.016, 1); }
			if (isBack()) { Camera.ProcessKeyboard(Movement::BACKWARD, 0.016, 1); }

			else if (Event.type == SDL_QUIT)
			{
				Running = 0;
			}
		}
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);
		{
			ImGui::DragFloat3("CameraPosition", &Camera.transform.Position[0]);
			ImGui::SliderAngle("CameraRotation", &Camera.transform.Rotation[0]);

		}

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glPushMatrix();
		//glScalef(0.5f, 0.5f, 0.5f);
		glTranslatef(0, 0, 0);

		glColor4f(1, 1, 1, 1);
		glBegin(GL_QUADS);                      // Draw A Quad
		glVertex3f(-1.0f, 1.0f, 0.0f);              // Top Left
		glVertex3f(1.0f, 1.0f, 0.0f);              // Top Right
		glVertex3f(1.0f, -1.0f, 0.0f);              // Bottom Right
		glVertex3f(-1.0f, -1.0f, 0.0f);              // Bottom Left
		glEnd();
		glPopMatrix();


		gluQuadricDrawStyle(quadric, GLU_FILL); // устанавливаем
						// стиль: сплошной
		gluSphere(quadric, 0, 10, 10);

		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(Window);
	}
	gluDeleteQuadric(quadric);
	return 0;
}
