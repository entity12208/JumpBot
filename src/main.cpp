#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/logging.hpp>
#include <fstream>
#include <vector>
#include <unordered_map>

class JumpBot : public geode::Mod {
public:
    static inline std::unordered_map<float, int> jumpAttempts;
    static inline std::vector<float> successfulJumps;
    static inline std::vector<float> jumpPlayback;
    static inline std::string levelName = "";
    static inline bool recording = false;
    static inline bool playingBack = false;
    static inline size_t playbackIndex = 0;

    void onEnable() override {
        geode::utils::log("JumpBot mod enabled.");
        resetState();
    }

    void onDisable() override {
        geode::utils::log("JumpBot mod disabled.");
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
    void playerWillDie(PlayerObject* player, bool unknown) {
        if (JumpBot::recording) {
            float deathPos = player->getPositionX();
            JumpBot::jumpAttempts[deathPos]++;
            if (JumpBot::jumpAttempts[deathPos] >= 2) {
                JumpBot::successfulJumps.push_back(deathPos);
            }
        }
        PlayerObject::playerWillDie(player, unknown);
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

GEODE_MOD_REGISTER(JumpBot)

class JumpBotMod : public geode::Mod {
public:
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override {
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_TAB && cocos2d::EventKeyboard::isKeyPressed(cocos2d::EventKeyboard::KeyCode::KEY_ALT)) {
            if (!JumpBot::recording && !JumpBot::playingBack) {
                JumpBot::recording = true;
                JumpBot::jumpAttempts.clear();
                JumpBot::successfulJumps.clear();
                geode::utils::log("JumpBot recording started.");
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
                geode::utils::log("JumpBot playback started.");
            } else if (JumpBot::playingBack) {

                JumpBot::playingBack = false;
                JumpBot::jumpPlayback.clear();
                JumpBot::playbackIndex = 0;
                geode::utils::log("JumpBot playback stopped.");
            }
        }
    }
};
