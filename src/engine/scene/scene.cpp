#include "engine/scene/scene.hpp"

#include <engine/scene/scene-manager.hpp>

void BaseScene::setFPS(uint32 fps) {
    SceneManager::ref().fps = fps;
}

bool BaseScene::isFPSUnlocked() const {
    return SceneManager::ref().isFPSUnlocked();
}
