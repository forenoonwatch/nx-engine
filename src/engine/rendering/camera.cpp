#include <engine/rendering/camera.hpp>

void Camera::update() {
    iView = Math::inverse(view);
    viewProjection = projection * iView;
    iViewProjection = view * iProjection;
}
