#include "JumpBot.h"

bool JumpBot::init() {
    // Set up keybind for CTRL + J
    Mod::registerKeybind(Keybind("CTRL + J", [this]() {
        this->recording = !this->recording;
        if (this->recording) {
            this->jumpPositions.clear();
            this->currentLevelName = Mod::getGame()->getLevelName();
        } else {
            this->saveInstructions(this->currentLevelName);
        }
    }));

    // Hook into the death event
    Mod::hook<GameManager>(GameManager::deathHook, [this](GameManager* self) {
        if (this->recording) {
            float deathPosition = self->getPlayer()->getPosition().x;
            this->recordJump(deathPosition);
        }
        return GameManager::deathHook(self);
    });

    // Hook into the level start event
    Mod::hook<GameManager>(GameManager::startHook, [this](GameManager* self) {
        if (!this->recording) {
            this->loadInstructions(Mod::getGame()->getLevelName());
        }
        return GameManager::startHook(self);
    });

    return true;
}

void JumpBot::recordJump(float position) {
    if (this->jumpPositions.size() >= 3) {
        // Check if the last three positions match
        if (this->jumpPositions[this->jumpPositions.size() - 1] == position &&
            this->jumpPositions[this->jumpPositions.size() - 2] == position &&
            this->jumpPositions[this->jumpPositions.size() - 3] == position) {
            return;
        }
    }
    this->jumpPositions.push_back(position);
}

void JumpBot::loadInstructions(const std::string& levelName) {
    std::ifstream file("jumpbot_" + levelName + ".txt");
    if (!file.is_open()) return;

    float position;
    this->jumpPositions.clear();
    while (file >> position) {
        this->jumpPositions.push_back(position);
    }

    file.close();
}

void JumpBot::saveInstructions(const std::string& levelName) {
    std::ofstream file("jumpbot_" + levelName + ".txt");
    for (float position : this->jumpPositions) {
        file << position << std::endl;
    }
    file.close();
}
