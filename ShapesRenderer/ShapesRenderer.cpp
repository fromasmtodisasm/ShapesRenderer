// ShapesRenderer.cpp : Defines the entry point for the application.
//

#include "ShapesRenderer.h"
#include <vector>
#include <glm/glm.hpp>
#include <gl/GL.h>

namespace Test
{
	using Vec3 = glm::vec3;

	struct STransform
	{
		Vec3 Position;
		Vec3 Rotation;
		Vec3 Scale;
	};

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
		IShape*    Shape;
		Vec3       Color;
		STransform Transform;
		void       Draw() { Shape->Draw({Transform, Color}); }
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
			for each (const auto& shape in m_Shapes)
			{
				shape->Draw();
			}
		}
		void Update()
		{
			auto t = GetTime();
			for each (auto shape in m_Shapes)
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

	}
} // namespace Test

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}
