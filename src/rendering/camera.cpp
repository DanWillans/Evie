#include "evie/camera.h"
#include "evie/types.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/geometric.hpp>

namespace evie {
void Camera::Rotate(const float& yaw_offset, const float& pitch_offset)
{
  yaw_ = std::fmodf(yaw_ += yaw_offset, 360.0);
  pitch_ = std::fmodf(pitch_ += pitch_offset, 360.0);
  pitch_ = std::min(pitch_, 89.0f);
  pitch_ = std::max(pitch_, -89.0f);
  direction_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  direction_.y = sin(glm::radians(pitch_));
  direction_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
}

void Camera::Rotate(const MousePosition& mouse_pos)
{
  float x_offset = static_cast<float>(mouse_pos.x) - last_mouse_x_;
  float y_offset = last_mouse_y_ - static_cast<float>(mouse_pos.y);
  last_mouse_x_ = static_cast<float>(mouse_pos.x);
  last_mouse_y_ = static_cast<float>(mouse_pos.y);
  x_offset *= sensitivity;
  y_offset *= sensitivity;
  this->Rotate(x_offset, y_offset);
}

// If W is pressed then update the camera position
// The camera position is updated by multiplying a camera_speed with the camera direction vector.
// The direction vector is the target/direction vector of where we want to look or what we want to look at.
// To simulate the camera moving forwards by moving the world along the positive Z axis.
// We do this by adding onto the cameras view position.
void Camera::MoveForwards(const float& delta_time) { position_ += camera_speed * delta_time * direction_; }

// We move the camera position along the positive Z axis.
void Camera::MoveBackwards(const float& delta_time) { position_ -= camera_speed * delta_time * direction_; };

// To strafe left we do a cross product. The cross product creates a perpendicular vector to the plane of the
// two vectors. If you cross product the Front and Up vectors you'll get a right vector. If we want the camera
// to go left we move the world right. So we negate the right vector from the camera position. This move the
// camera along the negative x axis.
void Camera::MoveLeft(const float& delta_time)
{
  position_ -= glm::normalize(glm::cross(direction_, up_)) * camera_speed * delta_time;
}

// We move the camera position to the right.
void Camera::MoveRight(const float& delta_time)
{
  position_ += glm::normalize(glm::cross(direction_, up_)) * camera_speed * delta_time;
}

mat4 Camera::GetViewMatrix()
{
  // Create mat4
  // auto right = glm::normalize(glm::cross(glm::normalize(position_ + direction_), glm::normalize(up_)));
  // mat4 rot = mat4{ 1.0f };
  // rot[0][0] = right.x;
  // rot[1][0] = right.y;
  // rot[2][0] = right.z;
  // rot[0][1] = up_.x;
  // rot[1][1] = up_.y;
  // rot[2][1] = up_.z;
  // rot[0][2] = direction_.x;
  // rot[1][2] = direction_.y;
  // rot[2][2] = direction_.z;
  // mat4 pos = mat4{ 1.0f };
  // pos[3][0] = -position_.x;
  // pos[3][1] = -position_.y;
  // pos[3][2] = -position_.z;
  // return rot * pos;
  // 1. Position = known
  // 2. Calculate cameraDirection
  glm::vec3 zaxis = glm::normalize(-direction_);
  // 3. Get positive right axis vector
  glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up_), zaxis));
  // 4. Calculate camera up vector
  glm::vec3 yaxis = glm::cross(zaxis, xaxis);

  // Create translation and rotation matrix
  // In glm we access elements as mat[col][row] due to column-major layout
  glm::mat4 translation = glm::mat4(1.0f);// Identity matrix by default
  translation[3][0] = -position_.x;// Third column, first row
  translation[3][1] = -position_.y;
  translation[3][2] = -position_.z;
  glm::mat4 rotation = glm::mat4(1.0f);
  rotation[0][0] = xaxis.x;// First column, first row
  rotation[1][0] = xaxis.y;
  rotation[2][0] = xaxis.z;
  rotation[0][1] = yaxis.x;// First column, second row
  rotation[1][1] = yaxis.y;
  rotation[2][1] = yaxis.z;
  rotation[0][2] = zaxis.x;// First column, third row
  rotation[1][2] = zaxis.y;
  rotation[2][2] = zaxis.z;

  // Return lookAt matrix as combination of translation and rotation matrix
  return rotation * translation;// Remember to read from right to left (first translation then rotation)
  // return glm::lookAt(position_, position_ + direction_, up_);
}

}// namespace evie