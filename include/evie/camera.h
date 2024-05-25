#ifndef EVIE_INCLUDE_RENDERING_CAMERA_H_
#define EVIE_INCLUDE_RENDERING_CAMERA_H_

#include "evie/core.h"
#include "evie/types.h"

#include "glm/glm.hpp"
#include <evie/camera.h>

namespace evie {
class EVIE_API Camera
{
public:
  /**
   * @brief Move the camera position backwards. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveBackwards(const float& delta_time);

  /**
   * @brief Move the camera position forwards. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveForwards(const float& delta_time);

  /**
   * @brief Move the camera position left. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveLeft(const float& delta_time);

  /**
   * @brief Move the camera position right. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveRight(const float& delta_time);

  /**
   * @brief Get the View Matrix object.
   *
   * @return mat4 A 4x4 matrix that represents the translation view matrix.
   */
  mat4 GetViewMatrix();

  /**
   * @brief Rotate the camera. This is a helper that will use the mouse position to rotate the camera.
   *
   * @param mouse_pos Current mouse position
   */
  void Rotate(const MousePosition& mouse_pos);

  /**
   * @brief Rotate the cameras direction by a raw yaw and pitch. This should be an offset not an absolute value.
   *
   * @param yaw Yaw offset to rotate the camera by.
   * @param pitch
   */
  void Rotate(const float& yaw_offset, const float& pitch_offset);

  void ResetCameraPosition(const vec3& position = { 0.0f, 0.0f, 0.0f });

  float camera_speed = 1.0f;
  float field_of_view = 45.0f;
  float sensitivity = 0.05f;

  const vec3& GetPosition() const { return position_; }
  const vec3& GetDirection() const { return direction_; }

private:
  // Fix this. This last mouse position should be the centre of the
  // viewport size. Construct based on the viewport size or set the
  // viewport.
  float last_mouse_x_ = 630;
  float last_mouse_y_ = 360;
  float pitch_ = 0.0f;
  // Offset yaw by -90.0f. A yaw of 0 degrees points the camera at
  // the positive x axis. Counter clockwise 90 degrees to point at z axis.
  float yaw_ = -90.0f;
  vec3 direction_{ 0.0f, 0.0f, -1.0f };
  vec3 up_{ 0.0f, 1.0f, 0.0f };
  vec3 position_{ 0.0f, 0.0f, 3.0f };
  bool first_mouse_{true};
};

class EVIE_API FPSCamera{
  public:
  /**
   * @brief Move the camera position backwards. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveBackwards(const float& delta_time);

  /**
   * @brief Move the camera position forwards. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveForwards(const float& delta_time);

  /**
   * @brief Move the camera position left. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveLeft(const float& delta_time);

  /**
   * @brief Move the camera position right. This position moves in proportion to
   * the camera_speed. To change the movement speed change the public variable
   * camera_speed.
   *
   * @param delta_time The time difference between the last frame and this frame.
   *                   Required so that the distance remainds constant. speed * time = distance.
   */
  void MoveRight(const float& delta_time);

  /**
   * @brief Get the View Matrix object.
   *
   * @return mat4 A 4x4 matrix that represents the translation view matrix.
   */
  mat4 GetViewMatrix();

  /**
   * @brief Rotate the camera. This is a helper that will use the mouse position to rotate the camera.
   *
   * @param mouse_pos Current mouse position.
   * @param enable_cursor If enabled then the cursor will rotate the camera. Use this flag to keep the camera up to date with the mouse position so that jump don't occur.
   */
  void Rotate(const MousePosition& mouse_pos, bool enable_cursor = true);

  /**
   * @brief Rotate the cameras direction by a raw yaw and pitch. This should be an offset not an absolute value.
   *
   * @param yaw Yaw offset to rotate the camera by.
   * @param pitch
   */
  void Rotate(const float& yaw_offset, const float& pitch_offset);

  void ResetCameraPosition(const vec3& position = { 0.0f, 0.0f, 0.0f });

  float camera_speed = 1.0f;
  float field_of_view = 45.0f;
  float sensitivity = 0.05f;

  const vec3& GetPosition() const { return position_; }
  const vec3& GetDirection() const { return direction_; }

private:
  // Fix this. This last mouse position should be the centre of the
  // viewport size. Construct based on the viewport size or set the
  // viewport.
  float last_mouse_x_ = 630;
  float last_mouse_y_ = 360;
  float pitch_ = 0.0f;
  // Offset yaw by -90.0f. A yaw of 0 degrees points the camera at
  // the positive x axis. Counter clockwise 90 degrees to point at z axis.
  float yaw_ = -90.0f;
  vec3 direction_{ 0.0f, 0.0f, -1.0f };
  vec3 up_{ 0.0f, 1.0f, 0.0f };
  vec3 position_{ 0.0f, 0.0f, 3.0f };
  bool first_mouse_{true};
};

}// namespace evie

#endif// !EVIE_INCLUDE_RENDERING_CAMERA_H_