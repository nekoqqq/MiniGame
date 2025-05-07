#pragma once
#include "GameLib/Sound/Player.h"
#include "GameLib/Sound/Wave.h"
#include <memory>
#include <vector>

using std::vector;
using namespace GameLib::Sound;

class GameSound {
public:
	enum BGM {
		THEME,
		GAME,
	}; // 循环播放的音乐
	enum Progress {
		BOOM,
		MOVE,
	};
	static GameSound& instance();

	void playBGM(BGM);
	void stopBGM();
	void playPRO(Progress);

	bool isReady();
private:
	Wave bgm_[2];
	Wave progress_[2];

	Player bgm_player_;
	Player pro_player_[2];

	// 私有化
	GameSound();
	~GameSound()=default;
	// 禁止拷贝和赋值
	GameSound & operator=(const GameSound&) = delete;
	GameSound(const GameSound&) = delete;
};