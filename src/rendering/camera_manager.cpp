#include "evie/camera_manager.hpp"
#include <evie/camera.h>

namespace evie {
CameraManager::CameraManager() { cameras_.reserve(MAX_CAMERAS); }
Result<Camera*> CameraManager::CreateCamera()
{
  if (camera_count_ < MAX_CAMERAS) {
    cameras_[camera_count_] = Camera{ CameraID{ camera_count_ } };
    SetActiveCamera(cameras_[camera_count_]);
    camera_count_++;
    return &cameras_[camera_count_ - 1];
  } else {
    return Error{ "Too many cameras" };
  }
}

Result<Camera*> CameraManager::CreateFPSCamera()
{
  if (camera_count_ < MAX_CAMERAS) {
    cameras_[camera_count_] = Camera{ CameraID{ camera_count_ }, true };
    SetActiveCamera(cameras_[camera_count_]);
    camera_count_++;
    return &cameras_[camera_count_ - 1];
  } else {
    return Error{ "Too many cameras" };
  }
}

void CameraManager::SetActiveCamera(const Camera& camera)
{
  auto id = camera.GetID().Get();
  active_camera_ = &cameras_[id];
}

Camera* CameraManager::GetActiveCamera() { return active_camera_; }

}// namespace evie