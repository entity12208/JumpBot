#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace geode::prelude;

std::string fileName = "level_name_jumps.txt";
std::vector<float> jumpPositions;
bool shouldJump = false;

void saveJumpPositions() {
    std::ofstream file(fileName);
    for (const auto& pos : jumpPositions) {
        file << pos << "\n";
    }
    file.close();
}

void loadJumpPositions() {
    std::ifstream file(fileName);
    float pos;
    jumpPositions.clear();
    while (file >> pos) {
        jumpPositions.push_back(pos);
    }
    file.close();
}

void onDeath() {
    auto player = GameManager::sharedState()->getPlayer();
    float deathPos = player->getPositionX();
    jumpPositions.push_back(deathPos);
    saveJumpPositions();
}

void onPlayerTick() {
    if (shouldJump) {
        auto player = GameManager::sharedState()->getPlayer();
        float playerPos = player->getPositionX();
        for (const auto& pos : jumpPositions) {
            if (fabs(playerPos - pos) < 0.1) {
                player->jump();
            }
        }
    }
}

$execute {
    if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('J')) {
        shouldJump = !shouldJump;
        if (shouldJump) {
            loadJumpPositions();
        }
    }
}

struct $modify(GameManager) {
    void onPlayerDeath(Player* player) {
        GameManager::onPlayerDeath(player);
        onDeath();
    }

    void update(float dt) {
        GameManager::update(dt);
        onPlayerTick();
    }
};
