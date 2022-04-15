// ShapesRenderer.cpp : Defines the entry point for the application.
//

#include "ShapesRenderer.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_opengl2.h>
#include <imgui_impl_sdl.h>

#include "Camera.h"


#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace Test
{

	struct SDrawParams
	{
		STransform Transform;
		Vec3       Color;
	};

	struct IShape
	{
		enum Type
		{
			Quad,
			Triangle,
			Circle,
		};
		~IShape() {}
		virtual void Draw(const SDrawParams& DrawParams) = 0;
	};

	struct SSicrleDesc
	{
		size_t NumSegments;
		float Size;
	};
	struct SQuadDesc {
		float Size;
	};

	struct SShapeDescription
	{
		IShape::Type Type;
		union
		{
			SSicrleDesc Circle;
			SQuadDesc Quad;
		};
	};
	class CCircle : public IShape
	{
	public:
		CCircle(float Size, size_t NumSegments) : m_Size(Size), m_Segments(NumSegments) {}
		// Inherited via IShape
		virtual void Draw(const SDrawParams& DrawParams) override
		{
			const auto& p = DrawParams;
			auto& S = p.Transform.Scale;
			auto& R = p.Transform.Rotation;
			auto& T = p.Transform.Position;
			auto& C = p.Color;
			auto Radius = 2;

			glLineWidth(1.f);
			glPushMatrix();
			glScalef(S.x, S.y, S.z);
			glRotatef(R.x, 1, 0, 0);
			glRotatef(R.y, 0, 1, 0);
			glRotatef(R.z, 0, 0, 1);
			glTranslatef(T.x, T.y, T.z);
			glColor4f(C.r, C.g, C.b, 1);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < m_Segments; i++)
			{
				auto x = float(Radius * glm::cos(i * 2 * glm::pi<float>() / m_Segments));
				auto y = float(Radius * glm::sin(i * 2 * glm::pi<float>() / m_Segments));
				glVertex3f(x, 0.0f, y);

			}
			glEnd();
			glPopMatrix();
		}
		float m_Size;
		size_t m_Segments;
	};

	class CQuad : public IShape
	{
	public:
		CQuad(float Size) : m_Size(Size) {}
		virtual void Draw(const SDrawParams& DrawParams) override
		{
			const auto& p = DrawParams;
			auto& S = p.Transform.Scale;
			auto& R = p.Transform.Rotation;
			auto& T = p.Transform.Position;
			auto& C = p.Color;

			glPushMatrix();
			glScalef(S.x, S.y, S.z);
			glRotatef(R.x, 1, 0, 0);
			glRotatef(R.y, 0, 1, 0);
			glRotatef(R.z, 0, 0, 1);
			glTranslatef(T.x, T.y, T.z);
			glColor4f(C.r, C.g, C.b, 1);

			glBegin(GL_QUADS);
			glVertex3f(-m_Size, m_Size, 0.0f);
			glVertex3f(m_Size, m_Size, 0.0f);
			glVertex3f(m_Size, -m_Size, 0.0f);
			glVertex3f(-m_Size, -m_Size, 0.0f);
			glEnd();

			glPopMatrix();
		}
	private:
		float m_Size;
	};

	std::shared_ptr<IShape> CreateShape(const SShapeDescription& Desc)
	{
		switch (Desc.Type)
		{
		case Test::IShape::Quad:
			return std::make_shared<CQuad>(Desc.Quad.Size);
		case Test::IShape::Circle:
			return std::make_shared<CCircle>(Desc.Circle.Size, Desc.Circle.NumSegments);
			break;
		default:
			break;
		}
		return nullptr;
	}


	class CEntity
	{
	public:
		CEntity(const STransform& transform, const Vec3& color)
			: m_Transform(transform), m_Color(color)
		{

		}
		void Move(const Vec3& v)
		{
			m_Transform.Position += v;
		}
		void Rotate(const Vec3& v)
		{
			m_Transform.Rotation += v;
		}
		void Scale(const Vec3& v)
		{
			m_Transform.Rotation += v;
		}
		void SetColor(const Vec3& c)
		{
			m_Color = c;
		}
		Vec3 GetColor(const Vec3& c)
		{
			return m_Color;
		}

		void SetShape(std::shared_ptr<IShape> pShape)
		{
			m_pShape = pShape;
		}
		void Draw()
		{
			m_pShape->Draw({ m_Transform, m_Color });
		}

	private:
		std::shared_ptr<IShape> m_pShape{};
		STransform m_Transform;
		Vec3 m_Color;
	};


	class Application
	{

		using i32 = int32_t;
		using u32 = uint32_t;
		using b32 = int32_t;

		static constexpr int WinWidth = 1000;
		static constexpr int WinHeight = 1000;
	public:
		Application()
		{
			// Setup window
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
			SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

			m_Window = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WinWidth, WinHeight, WindowFlags);
			assert(m_Window);
			m_Context = SDL_GL_CreateContext(m_Window);
			SDL_GL_SetSwapInterval(1); // Enable vsync

			m_Camera.SetViewport(Vec4(0, 0, WinWidth, WinHeight));
			m_Camera.SetPos({ 1.4, 9.4, -4.7 });
			m_Camera.SetAngles(Vec3{ 50,267,0 });
			m_Camera.UpdateCameraVectors();

			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			//******************************************* // Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			// Setup Platform/Renderer backends
			ImGui_ImplSDL2_InitForOpenGL(m_Window, m_Context);
			ImGui_ImplOpenGL2_Init();


			for (size_t i = 0; i < 10; i++)
			{
				SShapeDescription desc;
				desc.Type = IShape::Circle;
				desc.Circle.Size = 2.f;
				desc.Circle.NumSegments = 30;

				CEntity entity(STransform{ Vec3{-5.f + 2 * desc.Circle.Size * i, 0, 0}, {90,0,0},{1,1,1} }, { 1,float(i) / 10,float(i) / 10 });
				auto shape = CreateShape(desc);
				entity.SetShape(shape);
				m_Entities.push_back(entity);
			}
			for (size_t i = 0; i < 10; i++)
			{
				SShapeDescription desc;
				desc.Type = IShape::Quad;
				desc.Quad.Size = 2;

				CEntity entity(STransform{ Vec3{-5.f + 2 * desc.Quad.Size * i, 3, 0}, {90,0,0},{1,1,1} }, { 1,float(i) / 10,float(i) / 10 });
				auto shape = CreateShape(desc);
				entity.SetShape(shape);
				m_Entities.push_back(entity);
			}


		}
		~Application()
		{
			// Cleanup
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();

			SDL_GL_DeleteContext(m_Context);
			SDL_DestroyWindow(m_Window);
			SDL_Quit();

		}

		void HandleInput()
		{

			auto isDown = [this](int K) {
				return m_Keys[K];
			};

			auto isLeft = [&isDown]() {return isDown(SDL_SCANCODE_A); };
			auto isRight = [&isDown]() {return isDown(SDL_SCANCODE_D); };
			auto isFront = [&isDown]() {return isDown(SDL_SCANCODE_W); };
			auto isBack = [&isDown]() {return isDown(SDL_SCANCODE_S); };
			SDL_Event Event;


			while (SDL_PollEvent(&Event))
			{
				ImGui_ImplSDL2_ProcessEvent(&Event);
				if (Event.type == SDL_KEYDOWN)
				{
					m_Keys[Event.key.keysym.scancode] = true;
				}
				if (Event.type == SDL_KEYUP)
				{
					m_Keys[Event.key.keysym.scancode] = false;
				}
				if (Event.type == SDL_WINDOWEVENT)
				{
					switch (Event.window.event)
					{
					case SDL_WINDOWEVENT_RESIZED:

						m_Camera.SetViewport({ 0, 0, Event.window.data1, Event.window.data2 });
						break;

					}
				}
				if (Event.type == SDL_KEYDOWN)
				{
					switch (Event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						m_Running = 0;
						break;
					default:
						break;
					}
				}
				if (!ImGui::GetIO().WantCaptureMouse)
				{
					if (Event.type == SDL_MOUSEBUTTONDOWN)
					{
						m_MouseDown[Event.button.button] = true;
					}
					else if (Event.type == SDL_MOUSEBUTTONUP)
					{
						m_MouseDown[Event.button.button] = false;
					}
					if (Event.type == SDL_MOUSEMOTION && m_MouseDown[1])
					{
						auto x = m_MouseSensitivity * Event.motion.xrel;
						auto y = m_MouseSensitivity * Event.motion.yrel;
						m_Camera.ProcessMouseMovement(x, y);
					}
				}
				if (isLeft()) { m_Camera.ProcessKeyboard(Movement::LEFT, (float)m_DeltaTime, 1); }
				if (isRight()) { m_Camera.ProcessKeyboard(Movement::RIGHT, (float)m_DeltaTime, 1); }
				if (isFront()) { m_Camera.ProcessKeyboard(Movement::FORWARD, (float)m_DeltaTime, 1); }
				if (isBack()) { m_Camera.ProcessKeyboard(Movement::BACKWARD, (float)m_DeltaTime, 1); }

				else if (Event.type == SDL_QUIT)
				{
					m_Running = 0;
				}
			}

		}

		void Render()
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMultMatrixf(&m_Camera.GetProjectionMatrix()[0][0]);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMultMatrixf(&m_Camera.GetViewMatrix()[0][0]);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


			// Start the Dear ImGui frame
			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			{
				ImGui::DragFloat3("CameraPosition", &m_Camera.transform.Position[0]);
				ImGui::DragFloat3("CameraRotation", &m_Camera.transform.Rotation[0]);

			}

			// Rendering
			ImGui::Render();
			glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			int i = 0;
			for (auto& entity : m_Entities)
			{
				entity.Rotate(Vec3(0, float(0.1f * m_DeltaTime), 0));
				entity.Draw();
				i++;
			}

			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

			SDL_GL_SwapWindow(m_Window);

		}

		void Start()
		{
			// Our state
			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			Uint64 NOW = SDL_GetPerformanceCounter();
			Uint64 LAST = 0;

			m_DeltaTime = 0;
			while (m_Running)
			{
				LAST = NOW;
				NOW = SDL_GetPerformanceCounter();
				m_DeltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

				HandleInput();

				Render();

			}

		}

	public:
		SDL_Window* m_Window{};
		SDL_GLContext m_Context{};

		bool m_Running = true;
		double m_DeltaTime;
		CCamera m_Camera;
		std::vector<CEntity> m_Entities;
		bool m_MouseDown[10] = { false };

		bool m_Keys[SDL_NUM_SCANCODES] = {};
		float m_MouseSensitivity = 0.2f;

	};

} // namespace Test

int main(int ArgCount, char** Args)
{

	Test::Application app;

	app.Start();
	return 0;
}
