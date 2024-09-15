#include "evie/result.h"
#include "evie/types.h"
#include <doctest/doctest.h>


#include <evie/camera_manager.hpp>

TEST_CASE("Test CreateCamera")
{
  evie::CameraManager controller;
  evie::Result<evie::Camera*> camera = controller.CreateCamera();
  REQUIRE(camera.Good());
  auto cam = *camera;
  cam->camera_speed = 100.0F;
  // We want to check that we create a normal camera not an FPS camera.
  // Rotate camera
  evie::MousePosition mouse_pos{ 100.F, 100.0F };
  cam->Rotate(mouse_pos);
  mouse_pos.x = 200.0F;
  mouse_pos.y = 200.0F;
  cam->Rotate(mouse_pos);
  // Move camera
  cam->MoveForwards(1.0F);
  // Check camera isn't locked
  auto pos = cam->GetPosition();
  REQUIRE(pos.y != 0.0F);
}

TEST_CASE("Test CreateFPSCamera")
{
  evie::CameraManager controller;
  evie::Result<evie::Camera*> camera = controller.CreateFPSCamera();
  REQUIRE(camera.Good());
  auto cam = *camera;
  cam->camera_speed = 100.0F;
  // We want to check that we create a normal camera not an FPS camera.
  // Rotate camera
  evie::MousePosition mouse_pos{ 100.F, 100.0F };
  cam->Rotate(mouse_pos);
  mouse_pos.x = 200.0F;
  mouse_pos.y = 200.0F;
  cam->Rotate(mouse_pos);
  // Move camera
  cam->MoveForwards(1.0F);
  // Check camera is locked
  auto pos = cam->GetPosition();
  REQUIRE(pos.y == 0.0F);
}

TEST_CASE("Test camera swapping")
{
  evie::CameraManager controller;
  // Create cameras
  auto camera_res = controller.CreateCamera();
  REQUIRE(camera_res.Good());
  auto fps_camera_res = controller.CreateFPSCamera();
  REQUIRE(fps_camera_res.Good());
  auto camera = *camera_res;
  auto fps_camera = *fps_camera_res;

  // Check current active camera
  REQUIRE(camera->GetID() != controller.GetActiveCamera()->GetID());
  REQUIRE(fps_camera->GetID() == controller.GetActiveCamera()->GetID());

  // Set active camera
  controller.SetActiveCamera(*camera);
  REQUIRE(camera->GetID() == controller.GetActiveCamera()->GetID());
  REQUIRE(fps_camera->GetID() != controller.GetActiveCamera()->GetID());
}

TEST_CASE("Check manager max camera count")
{
  evie::CameraManager controller;
  for (int i = 0; i < 10; ++i) {
    controller.CreateCamera();
  }
  auto res = controller.CreateCamera();
  REQUIRE(res.Bad());
}

TEST_CASE("Check no active camera"){
  evie::CameraManager manager;
  REQUIRE(manager.GetActiveCamera() == nullptr);
}