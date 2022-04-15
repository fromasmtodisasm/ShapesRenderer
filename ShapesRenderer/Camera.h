#pragma once

#include <glm/gtc/matrix_transform.hpp>
const float PITCH = 0.0f;
const float SPEED = 0.1f;
const float SENSITIVTY = 0.25f;
const float ZOOM = 45.0f;

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;

struct STransform
{
	Vec3 Position;
	Vec3 Rotation;
	Vec3 Scale;
};

enum class Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class CCamera
{
public:

	enum class Mode
	{
		FPS,
		FLY
	} mode = CCamera::Mode::FLY;
	enum class Type
	{
		Perspective,
		Ortho
	} type = Type::Perspective;

public:
	// Camera Attributes
	STransform    transform;
	Vec3 Front;
	Vec3 Up;
	Vec3 Right;
	Vec3 WorldUp;
	Vec3        m_MoveDirection;
	// Camera options
	float        MovementSpeed;
	float        Zoom;
	float        m_fov = 45.0f;
	float        Ratio = 16.0f / 9;
	float        zNear = 0.1f;
	float        zFar = 10000.f;

	Vec4 ViewPort;

	// Constructor with vectors
	CCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.f, float pitch = PITCH, Vec3 Front = Vec3{ 0,0,-1 })
		: MovementSpeed(10.f)
		, Zoom(ZOOM)
	{
		this->transform.Position = position;
		this->WorldUp = up;
		this->transform.Rotation.y = yaw;
		this->transform.Rotation.x = pitch;
		this->UpdateCameraVectors();
	}
	// Constructor with scalar values
	CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: Front(glm::vec3(0.0f, 0.0f, 0.0f))
		, MovementSpeed(10.f)
		, Zoom(ZOOM)
	{
		this->transform.Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->transform.Rotation.y = yaw;
		this->transform.Rotation.x = pitch;
		this->UpdateCameraVectors();
	}

	void Update()
	{
		UpdateCameraVectors();
	}

	void Init(int width, int height, float fov = 45.f)
	{
	}
	void SetAngle(Vec3 ang)
	{
	}
	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	Mat4 GetViewMatrix() const
	{
		//return glm::lookAt(this->transform.position, this->transform.position + this->Front, this->Up);
		return glm::lookAtLH(this->transform.Position, this->transform.Position + this->Front, this->Up);
	}
	void SetViewport(const Vec4& viewPort) { ViewPort = viewPort; }
	Mat4 GetProjectionMatrix()
	{
		Vec4& v = ViewPort;
		return glm::perspective(glm::radians(m_fov), (float)(v.z) / (float)(v.w), zNear, zFar);
	}

	Vec3 GetPos() const
	{
		return transform.Position;
	}
	Vec3 GetAngles() const
	{
		return transform.Rotation;
	}

	void SetPos(glm::vec3 pos)
	{
		transform.Position = pos;
	}

	void SetAngles(glm::vec3 ang)
	{
		transform.Rotation = ang;
	}

	inline void  SetFov(float fov) { m_fov = fov; }
	inline float GetFov() const { return (m_fov); }

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void         ProcessMouseScroll(float yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		transform.Rotation.y -= xoffset;
		transform.Rotation.x += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			auto& x = transform.Rotation.x;
			x = glm::clamp(x, -89.f, 89.f);
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		UpdateCameraVectors();
	}
	void ProcessKeyboard(Movement direction, float deltaTime, float value)
	{
		float velocity = 0.01f * deltaTime * value;
		mode = CCamera::Mode::FLY;

		auto moveForward = glm::vec3(
			Front.x,
			mode == CCamera::Mode::FPS ? 0 : Front.y,
			Front.z);

		if (direction == Movement::FORWARD)
		{
			m_MoveDirection = moveForward;
		}
		if (direction == Movement::BACKWARD)
		{
			m_MoveDirection = -moveForward;
		}
		if (direction == Movement::LEFT)
		{
			m_MoveDirection = Right;
		}
		if (direction == Movement::RIGHT)
		{
			m_MoveDirection = -Right;
		}
		transform.Position += velocity * m_MoveDirection;
	}


	// Calculates the front vector from the Camera's (updated) Eular Angles
	void UpdateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->transform.Rotation.y)) * cos(glm::radians(this->transform.Rotation.x));
		front.y = sin(glm::radians(this->transform.Rotation.x));
		front.z = sin(glm::radians(this->transform.Rotation.y)) * cos(glm::radians(this->transform.Rotation.x));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};
