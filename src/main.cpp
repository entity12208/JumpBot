/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <fstream>
#include <vector>
#include <unordered_map>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

class JumpBot {
public:
    static inline std::unordered_map<float, int> jumpAttempts;
    static inline std::vector<float> successfulJumps;
    static inline std::vector<float> jumpPlayback;
    static inline std::string levelName = "";
    static inline bool recording = false;
    static inline bool playingBack = false;
    static inline size_t playbackIndex = 0;

    static void resetState() {
        jumpAttempts.clear();
        successfulJumps.clear();
        jumpPlayback.clear();
        levelName.clear();
        recording = false;
        playingBack = false;
        playbackIndex = 0;
    }
};

class $modify(PlayerObject) {
    void destroyPlayer(PlayerObject* player, bool isDeath) {
        if (JumpBot::recording) {
            float deathPos = this->getPositionX();
            JumpBot::jumpAttempts[deathPos]++;
            if (JumpBot::jumpAttempts[deathPos] >= 2) {
                JumpBot::successfulJumps.push_back(deathPos);
            }
        }
        PlayerObject::destroyPlayer(player, isDeath);
    }

    void update(float dt) {
        if (JumpBot::playingBack && JumpBot::playbackIndex < JumpBot::jumpPlayback.size()) {
            float currentPos = this->getPositionX();
            if (currentPos >= JumpBot::jumpPlayback[JumpBot::playbackIndex]) {
                this->jump(false);
                JumpBot::playbackIndex++;
            }
        }
        PlayerObject::update(dt);
    }
};

class $modify(GameManager) {
    void levelComplete() {
        if (!JumpBot::successfulJumps.empty()) {
            std::ofstream outFile(JumpBot::levelName + ".txt");
            for (const auto& pos : JumpBot::successfulJumps) {
                outFile << pos << std::endl;
            }
            outFile.close();
            JumpBot::successfulJumps.clear();
        }
        GameManager::levelComplete();
    }
};

class JumpBotMod : public geode::EventListener {
public:
    void setup() {
        geode::EventDispatcher::get()->registerEvent<geode::KeyEvent>([this](geode::KeyEvent* event) {
            if (event->getKeyCode() == geode::KeyCode::KEY_TAB && event->isAltPressed()) {
                handleKeyEvent();
                return true;
            }
            return false;
        });
    }

    void handleKeyEvent() {
        if (!JumpBot::recording && !JumpBot::playingBack) {
            JumpBot::recording = true;
            JumpBot::jumpAttempts.clear();
            JumpBot::successfulJumps.clear();
        } else if (JumpBot::recording) {
            JumpBot::recording = false;
            JumpBot::playingBack = true;
            JumpBot::playbackIndex = 0;

            std::ifstream inFile(JumpBot::levelName + ".txt");
            float pos;
            while (inFile >> pos) {
                JumpBot::jumpPlayback.push_back(pos);
            }
            inFile.close();
        } else if (JumpBot::playingBack) {
            JumpBot::playingBack = false;
            JumpBot::jumpPlayback.clear();
            JumpBot::playbackIndex = 0;
        }
    }
};

// Initialize the Geode mod
extern "C" GEODE_API void GEODE_CALL geode_load(Mod* mod) {
    JumpBotMod* listener = new JumpBotMod();
    listener->setup();
    JumpBot::resetState();
}
