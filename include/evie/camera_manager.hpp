#ifndef INCLUDE_EVIE_CAMERA_MANAGER_HPP_
#define INCLUDE_EVIE_CAMERA_MANAGER_HPP_

#include "evie/core.h"
#include "evie/camera.h"
#include "evie/result.h"


namespace evie {

class EVIE_API CameraManager
{
public:
  static constexpr uint8_t MAX_CAMERAS = 10;
  CameraManager();
  Result<Camera*> CreateCamera();
  Result<Camera*> CreateFPSCamera();
  void SetActiveCamera(const Camera& camera);
  Camera* GetActiveCamera();

private:
  std::vector<Camera> cameras_;
  Camera* active_camera_{ nullptr };
  uint8_t camera_count_{ 0 };
};

}// namespace evie

#endif// !INCLUDE_EVIE_CAMERA_MANAGER_HPP_