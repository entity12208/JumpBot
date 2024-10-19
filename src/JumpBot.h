#pragma once

#include <Geode/Bindings.hpp>
#include <Geode/modify/GameManager.hpp>
#include <fstream>
#include <vector>

using namespace geode::prelude;

class JumpBot : public Interface<JumpBot> {
public:
    JumpBot();
    void loadInstructions(const std::string& levelName);

private:
    std::vector<float> deathPositions;
    std::string currentLevel;
    void saveInstructions();
    void jumpBeforeDeathPoint(float deathPosition);
};
