#include"Game.h"

Game::Game() :height_(0), width_(0), steps_(0) {}
bool Game::valid(pair<int, int>& pos) const{
    if (pos.first >= 0 && pos.first < height_ && pos.second >= 0 && pos.second < width_ && grid_[pos.first][pos.second] != BOUNDARY)
        return true;
    return false;
}
void Game::_update_objects(pair<int, int>& new_pos, int direction) {
    pair<int, int> box_target_pos;
    if (direction == UP) // W
    {
        new_pos.first -= 1;
        box_target_pos = make_pair(new_pos.first - 1, new_pos.second);
    }
    else if (direction == LEFT) // A
    {
        new_pos.second -= 1;
        box_target_pos = make_pair(new_pos.first, new_pos.second - 1);
    }
    else if (direction == RIGHT) // S
    {
        new_pos.first += 1;
        box_target_pos = make_pair(new_pos.first + 1, new_pos.second);
    }
    else if (direction == DOWN) // D
    {
        new_pos.second += 1;
        box_target_pos = make_pair(new_pos.first, new_pos.second + 1);
    }
    else
        throw exception();

    bool should_restore = false;
    if (valid(new_pos)) {
        if (grid_[new_pos.first][new_pos.second] == 'o') {
            if (valid(box_target_pos) && grid_[box_target_pos.first][box_target_pos.second] != BOX) { // 判断当前推的是否是箱子，如果是箱子继续往前推
                grid_[box_target_pos.first][box_target_pos.second] = BOX;
            }
            else // 无法往前走
                should_restore = true;
        }
    }
    else
        should_restore = true;

    if (should_restore)
        if (direction == 0) // W
            new_pos.first += 1;
        else if (direction == 1) // A
            new_pos.second += 1;
        else if (direction == 2) // S
            new_pos.first -= 1;
        else if (direction == 3) // D
            new_pos.second -= 1;
    return;
}
bool Game::win()const{
    int succeed = 0;
    for (auto& t : target_pos_)
        if (grid_[t.first][t.second] == BOX)
            succeed += 1;

    return succeed >= box_pos_.size();
}

void ConsoleGame::init(MAPSOURCE mapSource){
    if (mapSource == MAPSOURCE::PREDEFINED) {
        height_ = 5;
        width_ = 8;
        box_pos_ = { {2,2},{2,5} };
        target_pos_ = { {1,2},{1,3},{3,5} };
        player_pos_ = { 1,5 };

        grid_.resize(height_);
        for (int i = 0; i < grid_.size(); i++)
            grid_[i].resize(width_);

        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; j++) {
                if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
                    grid_[i][j] = BOUNDARY;
                else
                    grid_[i][j] = BLANK;
            }
        }

        // set box
        for (auto& pos : box_pos_)
            grid_[pos.first][pos.second] = BOX;

        // set target
        for (auto& pos : target_pos_)
            grid_[pos.first][pos.second] = TARGET;


        // set player
        grid_[player_pos_.first][player_pos_.second] = PLAYER;
    }
    else if (mapSource == MAPSOURCE::FILE) {
        ifstream fin("map.txt", ios_base::in);
        if (!fin.is_open()) {
            cout << "打开失败" << endl;
            exit(-1);
        }
        string line;
        int i = 0;
        while (getline(fin, line)) {
            grid_.push_back(vector<char>(line.size()));
            for (int j = 0; j < line.size(); j++)
                grid_[i][j] = line[j];
            i++;
        }

        height_ = grid_.size();

        for (int i = 0; i < grid_.size(); i++) {
            for (int j = 0; j < grid_[i].size(); j++) {
                if (grid_[i][j] == BOX) {
                    box_pos_.push_back({ i,j });
                }
                else if (grid_[i][j] == PLAYER)
                    player_pos_ = { i,j };
                else if (grid_[i][j] == TARGET)
                    target_pos_.push_back({ i,j });

            }
            width_ = max(width_, (int)grid_[i].size());
        }


    }
}
void ConsoleGame::update(string& input) {
    pair<int, int> new_pos{ player_pos_ };
    pair<int, int> old_pos{ player_pos_ };
    for (int i = 0; i < input.size(); i++) {
        switch (input[i]) {
        case 'w':
        case 'W':
            _update_objects(new_pos, 0);
            break;
        case 'a':
        case 'A':
            _update_objects(new_pos, 1);
            break;
        case 's':
        case 'S':
            _update_objects(new_pos, 2);
            break;
        case 'd':
        case 'D':
            _update_objects(new_pos, 3);
            break;
        default:
            break;
        }
        if (new_pos != old_pos) {
            // 更新玩家位置
            steps_++;
            grid_[new_pos.first][new_pos.second] = 'p';
            for (auto& t : target_pos_)
                if (old_pos.first == t.first && old_pos.second == t.second) {
                    grid_[old_pos.first][old_pos.second] = Game::OBJECT::TARGET;
                    break;
                }
                else
                    grid_[old_pos.first][old_pos.second] = Game::OBJECT::BLANK;
        }
        if (win())
        {
            draw();
            cout << "YOU WIN! Total steps(exculude invalid steps): " << steps_ << "." << endl;
            exit(0);
        }
        old_pos = new_pos;
    }
    player_pos_ = new_pos;
}
void ConsoleGame::draw() {
    for (int i = 0; i < grid_.size(); i++, cout << endl)
        for (int j = 0; j < grid_[i].size(); j++)
            cout << grid_[i][j];
}
