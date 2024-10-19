#pragma once

#include <Geode/Bindings.hpp>
#include <Geode/Geode.hpp>
#include <fstream>
#include <vector>

using namespace geode::prelude;

class JumpBot : public Mod {
public:
    virtual bool init() override;
    void recordJump(float position);
    void loadInstructions(const std::string& levelName);
    void saveInstructions(const std::string& levelName);

private:
    std::vector<float> jumpPositions;
    std::string currentLevelName;
    bool recording = false;
};

