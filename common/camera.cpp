#include "camera.hpp"
#include "spdlog/spdlog.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float movement_speed, float mouse_sensitivity, float zoom)
    : position_(position), up_(up), yaw_(yaw), pitch_(pitch), movement_speed_(movement_speed), mouse_sensitivity_(mouse_sensitivity), zoom_(zoom)
{
    UpdateCameraVecs();
}

Camera::~Camera() {}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

float Camera::GetFOV()
{
    return zoom_;
}

glm::vec3 Camera::GetPosition()
{
    return position_;
}

void Camera::ProcessKeyboard(unsigned int direction, float time_delta)
{
    float velocity = movement_speed_ * time_delta;

    if (direction & CAMERA_MOVE_FORWARD)
        position_ += front_ * velocity;
    if (direction & CAMERA_MOVE_BACKWARD)
        position_ -= front_ * velocity;
    if (direction & CAMERA_MOVE_LEFT)
        position_ -= right_ * velocity;
    if (direction & CAMERA_MOVE_RIGHT)
        position_ += right_ * velocity;
}

void Camera::ProcessMouseMovement(float xoff, float yoff, bool constrain_pitch)
{
    yaw_ += xoff * mouse_sensitivity_;
    pitch_ += yoff * mouse_sensitivity_;

    if (constrain_pitch)
    {
        pitch_ = pitch_ > 89.0f ? 89.0f : pitch_;
        pitch_ = pitch_ < -89.0f ? -89.0f : pitch_;
    }

    UpdateCameraVecs();
}

void Camera::ProcessMouseScroll(float yoff)
{
    zoom_ -= yoff;
    zoom_ = zoom_ > 45.0f ? 45.0f : zoom_;
    zoom_ = zoom_ < 1.0f ? 1.0f : zoom_;
}

void Camera::ShowInfo()
{
    SPDLOG_INFO("Camera: \nposition: ({}, {}, {})\nfront: ({}, {}, {})\nup: ({}, {}, {})\nright: ({}, {}, {})\nyaw: {}\npitch: {}",
        position_.x, position_.y, position_.z,
        front_.x, front_.y, front_.z,
        up_.x, up_.y, up_.z,
        right_.x, right_.y, right_.z,
        yaw_,
        pitch_);
}

void Camera::UpdateCameraVecs()
{
    front_ = glm::normalize(glm::vec3(
        cos(glm::radians(pitch_)) * cos(glm::radians(yaw_)),
        sin(glm::radians(pitch_)),
        cos(glm::radians(pitch_)) * sin(glm::radians(yaw_))));
    right_ = glm::normalize(glm::cross(front_, up_));
    // up?
}