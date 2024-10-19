#include <Geode/Geode.hpp>
#include <Geode/loader/SettingEvent.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>

#include <fstream>
#include <string>
#include <vector>

using namespace geode::prelude;

class $modify(Level) {
    std::vector<int> portalOrder;
    bool ctrlJPressed = false;

    void onLevelStart() override {
        Level::onLevelStart(); // Call the base class method
        if (isCtrlJPressed()) {
            scanPortalOrder();
        }
    }

    void onDeath() override {
        Level::onDeath(); // Call the base class method
        retryLevelWithJump();
    }

    bool isCtrlJPressed() {
        return (GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('J') & 0x8000);
    }

    void scanPortalOrder() {
        portalOrder.clear();
        for (auto& portal : getPortals()) {
            portalOrder.push_back(portal->getType());
        }
        ctrlJPressed = true;
    }

    void retryLevelWithJump() {
        if (ctrlJPressed) {
            for (int i = 0; i < 3; ++i) {
                jumpBeforeDeath();
            }
            logOutcome();
        }
    }

    void jumpBeforeDeath() {
        // Logic to jump before the moment of death
        auto player = getPlayer();
        if (player) {
            player->jump();
        }
    }

    void logOutcome() {
        std::ofstream logFile;
        logFile.open(getLevelName() + ".txt", std::ios::app);
        logFile << "Outcome logged\n";
        logFile.close();
    }

    std::string getLevelName() {
        auto level = getCurrentLevel();
        return level ? level->getName() : "Unknown";
    }

    std::vector<Portal*> getPortals() {
        auto level = getCurrentLevel();
        return level ? level->getPortals() : std::vector<Portal*>{};
    }

    Player* getPlayer() {
        auto level = getCurrentLevel();
        return level ? level->getPlayer() : nullptr;
    }
};
