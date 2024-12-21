#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/cocos.hpp>
#include <fstream>
#include <vector>
#include <unordered_map>

class JumpBot {
public:
    static inline std::unordered_map<float, int> jumpAttempts;
    static inline std::vector<float> successfulJumps;
    static inline std::vector<float> jumpPlayback;
    static inline std::string levelName = "";
    static inline bool recording = false;
    static inline bool playingBack = false;
    static inline size_t playbackIndex = 0;

    void onEnable() {
        resetState();
    }

    void onDisable() {
        resetState();
    }

    void resetState() {
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
    void die(bool unknown) {
        if (JumpBot::recording) {
            float deathPos = this->getPositionX();
            JumpBot::jumpAttempts[deathPos]++;
            if (JumpBot::jumpAttempts[deathPos] >= 2) {
                JumpBot::successfulJumps.push_back(deathPos);
            }
        }
        PlayerObject::die(unknown);
    }

    void update(float dt) {
        if (JumpBot::playingBack && JumpBot::playbackIndex < JumpBot::jumpPlayback.size()) {
            float currentPos = this->getPositionX();
            if (currentPos >= JumpBot::jumpPlayback[JumpBot::playbackIndex]) {
                PlayerObject::jump(false); 
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

class JumpBotMod {
public:
    void onKeyPressed(geode::EventKeyboard::KeyCode keyCode, geode::Event* event) {
        if (keyCode == geode::EventKeyboard::KeyCode::KEY_TAB && geode::EventKeyboard::isKeyPressed(geode::EventKeyboard::KeyCode::KEY_ALT)) {
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
    }
};
