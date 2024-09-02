#pragma once

extern "C"
{
#include "glad/glad.h"
}

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#define CAMERA_MOVE_FORWARD (0x01 << 0)
#define CAMERA_MOVE_BACKWARD (0x01 << 1)
#define CAMERA_MOVE_LEFT (0x01 << 2)
#define CAMERA_MOVE_RIGHT (0x01 << 3)

class Camera
{
public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f,
        float movement_speed = 2.5f,
        float mouse_sensitivity = 0.1f,
        float zoom = 45.0f);
    ~Camera();
    glm::mat4 GetViewMatrix();
    float GetFOV();
    glm::vec3 GetPosition();
    void ProcessKeyboard(unsigned int direction, float time_delta);
    void ProcessMouseMovement(float xoff, float yoff, bool constrain_pitch);
    void ProcessMouseScroll(float yoff);
    void ShowInfo();

private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;

    float yaw_;
    float pitch_;

    float movement_speed_;
    float mouse_sensitivity_;
    float zoom_;

    void UpdateCameraVecs();
};