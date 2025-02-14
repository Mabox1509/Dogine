//[INCLUDES]
#include "../dogine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//[NAMESPACE]
namespace Dogine 
{
	//[IMPLEMENTATIONS]
	Camera::Camera(glm::vec3 _position, float _fov = 45.0f, float _near = .1f, float _far = 100.0f)
	{
		position = _position;

		fov = _fov;
		near_plane = _near;
		far_plane = _far;

		orthographic_size = 5;
		is_orthographic = false;
	}


	void Camera::Rotate(glm::vec3 _angle)
	{
		// Convertir los ángulos de grados a radianes
		glm::vec3 radian_angles = glm::radians(_angle);

		// Crear una matriz de rotación a partir de los ángulos en radianes
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), radian_angles.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotar sobre X
		rotation = glm::rotate(rotation, radian_angles.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotar sobre Y
		rotation = glm::rotate(rotation, radian_angles.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotar sobre Z

		// Aplicar la rotación a la orientación y al vector "up"
		orientation = glm::mat3(rotation) * orientation;
		up = glm::mat3(rotation) * up;
	}
	glm::mat4 Camera::Matrix(float _aspect)
	{
		glm::mat4 _view = glm::mat4(1.0f);
		glm::mat4 _proj = glm::mat4(1.0f);

		_view = glm::lookAt(position, position + orientation, up);
		if (is_orthographic)
			_proj = glm::ortho(-_aspect * orthographic_size, _aspect * orthographic_size, -orthographic_size, orthographic_size, near_plane, far_plane);
		else
			_proj = glm::perspective(glm::radians(fov), _aspect, near_plane, far_plane);


		//OUT VALUE
		return _proj * _view;
	}
}