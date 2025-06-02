#pragma once
#include "GameLib/Sound/Player.h"
#include "GameLib/Sound/Wave.h"
#include "GameLib/Sound/Manager.h"
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
using namespace GameLib::Sound;
using namespace GameLib;

class GameSound {
public:
	enum BGM {
		THEME,
		GAME,
		MECHA_THEME
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
	Wave bgm_[3];
	Wave progress_[3];

	Player bgm_player_;
	Player pro_player_[3];

	// 私有化
	GameSound();
	~GameSound() = default;
	// 禁止拷贝和赋值
	GameSound& operator=(const GameSound&) = delete;
	GameSound(const GameSound&) = delete;
};

GameSound& GameSound::instance() {
	static GameSound instance_;
	return instance_;
}

void GameSound::playBGM(BGM bgm)
{
	while (!isReady()) {}
	bgm_player_ = Player::create(bgm_[bgm]);
	bgm_player_.play(true);
}

void GameSound::stopBGM()
{
	bgm_player_.stop();
}

void GameSound::playPRO(Progress pro)
{
	pro_player_[pro] = Player::create(progress_[pro]);
	pro_player_[pro].setVolume(-20);
	pro_player_[pro].play();
}

bool GameSound::isReady()
{
	for (int i = 0; i < 3; i++) {
		if (!bgm_[i].isReady())
			return false;
		if (!progress_[i].isReady())
			return false;
	}
	return true;
}

GameSound::GameSound()
{
	auto create_wav = [&](Wave v[], vector<string>& wavs) {
		for (int i = 0; i < wavs.size(); i++) {
			v[i] = Wave::create(wavs[i].c_str());
		}
		};
	vector<string> bgm = {
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\theme.wav",
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\game.wav",
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\mecha_theme.wav"
	};
	create_wav(bgm_, bgm);
	vector<string> progress = {
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\boom.wav",
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\move.wav",
		"C:\\Users\\colorful\\source\\repos\\MiniGame\\Console\\music\\move.wav",

	};
	create_wav(progress_, progress);
}
