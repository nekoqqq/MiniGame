#include "GameLib/Framework.h"
#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "Library/Robot.h"
#include <sstream>
using namespace std;
using namespace GameLib::Input;
const double PI = 3.141592653589793238;
const int WIDTH = 640;
const int HEIGHT = 480;
// 动物
Model* gPlayer;
Model* gEnemy;
// 静物
Model* gStage;
Model* gAxis;
// 变换矩阵
Matrix44 gProjectionTransform;
Matrix44 gViewTransform;
// 摄像机
Camera* gCamera;
Vector3 gEyePos(10, 10, 10);
Vector3 gTargetPos(0, 0, 0);
Vector3 gEyeUp(0., 1., 0.); // 一般是向上
const double fov_y = PI / 3;
const double aspec_ratio = 1.0 * WIDTH / HEIGHT;
const double near = 1.0;
const double far = 10000.0;
// 模型细节
Painter* gPlayerPainter, * gEnemyPainter, * gStagePainter, * gAxisPainter;

namespace GameLib {
	bool firstFrame = true;
	void draw_coor(const Vector3&);
	void prepare_data(Painter*& player_painter, Painter*& enemy_painter, Painter*& stage_painter, Painter*& axis_painter);

	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			setFrameRate(144);
			firstFrame = false;
			prepare_data(gPlayerPainter, gEnemyPainter, gStagePainter, gAxisPainter);
			gPlayer = new Mecha(Model::PLAYER, { 0,0,-50 }, gPlayerPainter);
			gEnemy = new Mecha(Model::ENEMY, { 0,0,50 }, gEnemyPainter);
			gStage = new Stage(Model::STAGE, gStagePainter);
			gAxis = new Axis(Model::AXIS, gAxisPainter);
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		gPlayer->update(vr);
		Vector3 player_dir = gPlayer->getZDirection();
		gCamera->update(player_dir);

		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		gStage->draw(pv);
		gAxis->draw(pv);
		gPlayer->draw(pv);
		gEnemy->draw(pv);
		draw_coor(gTargetPos);

		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();

	}
	void draw_coor(const Vector3& v) {
		ostringstream oss;
		oss << v.x << " " << v.y << " " << v.z << std::endl;;
		Framework::instance().drawDebugString(0, 0, oss.str().c_str());
	}
	void prepare_data(Painter*& player_painter, Painter*& enemy_painter, Painter*& stage_painter, Painter*& axis_painter) {
		vector<Vector3> player_vertexes = {
			{1,1,1},  // +z
			{-1,1,1},
			{-1,0,1},
			{1,0,1},

			{-1,1,-1}, // -z
			{1,1,-1},
			{1,0,-1},
			{-1,0,-1},

			{ -1,1,1 }, // -x
			{-1,1,-1},
			{-1,0,-1},
			{-1,0,1},

			{1,1,-1}, // +x
			{1,1,1},
			{1,0,1},
			{1,0,-1},

			{-1,1,1}, // +y
			{1,1,1},
			{1,1,-1},
			{-1,1,-1},

			{1,0,1}, // -y
			{-1,0,1},
			{-1,0,-1},
			{1,0,-1}
		};
		vector<array<double, 2>> player_uvs = {
			// +z
			{0.0,0.0},
			{0.25,0.0},
			{0.25,0.25},
			{0.0,0.25},

			// -z
			{0.25,0.0},
			{0.5,0.0},
			{0.5,0.25},
			{0.25,0.25},

			// -x
			{0.5,0.0},
			{0.75,0.0},
			{0.75,0.25},
			{0.5,0.25},

			// +x
			{0.75,0.0},
			{1.0,0.0},
			{1.0,0.25},
			{0.75,0.25},

			// +y
			{0.25,0.0},
			{0.25,0.25},
			{0.25,0.375},
			{0.0,0.375},

			// -y
			{0.25,0.25},
			{0.5,0.25},
			{0.5,0.375},
			{0.25,0.375}
		};
		vector<Color> player_colors = {
			RED,
			GREEN,
			BLUE,
			RED,

			RED,
			GREEN,
			BLUE,
			RED,

			H0,
			H1,
			H2,
			H0,

			H0,
			H1,
			H2,
			H0,

			Q0,
			Q1,
			Q2,
			Q0,

			Q0,
			Q1,
			Q2,
			Q0,
		};
		VertexBuffer* player_vb = new VertexBuffer(player_vertexes, player_uvs);
		vector<array<unsigned, 3>> player_indices = {
			{0,1,2},
			{3,2,0},

			{4,5,6},
			{7,6,4},

			{8,9,10},
			{11,10,8},

			{12,13,14},
			{15,14,12},

			{16,17,18},
			{19,18,16},

			{20,21,22},
			{23,22,20}
		};
		IndexBuffer* player_ib = new IndexBuffer(player_indices);
		Texture* player_texture = nullptr;
		Framework::instance().createTexture(&player_texture, "player.tga"); // 这里是因为**的作用和*&相同，可以直接修改传进来的指针指向的地址，从而也必须初始化
		player_painter = new Painter(player_vb, player_ib, player_texture, true, Framework::BLEND_OPAQUE);

		vector<array<double, 2>> enemy_uvs = {
			// +z
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},


			// -z
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},

			// -x
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},

			// +x
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},

			// +y
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},

			// -y
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
			{0.0,0.0},
		};
		VertexBuffer* enemy_vb = new VertexBuffer(player_vertexes, enemy_uvs, player_colors);
		IndexBuffer* enemy_ib = new IndexBuffer(player_indices);
		enemy_painter = new Painter(enemy_vb, enemy_ib, nullptr, true, Framework::BLEND_OPAQUE);

		vector<Vector3>  stage_vertexes = {
			{-1000,0,-1000},
			{-1000,0,1000},
			{1000,0,1000},
			{1000,0,-1000}
		};
		vector<array<double, 2>> stage_uvs = {
				{0.0,0.0},
				{1.0,0.0},
				{1.0,1.0},
				{0.0,1.0}
		};
		VertexBuffer* stage_vb = new VertexBuffer(stage_vertexes, stage_uvs);
		vector<array<unsigned, 3>> stage_indices = {
			{0,1,2},
			{3,2,0},
		};
		IndexBuffer* stage_ib = new IndexBuffer(stage_indices);
		Texture* stage_texture = nullptr;
		Framework::instance().createTexture(&stage_texture, "stage.tga");
		stage_painter = new Painter(stage_vb, stage_ib, stage_texture, true, Framework::BLEND_OPAQUE);

		vector<Vector3> axis_vertexes = {
			// x
			{-100,1,0,1},
			{100,1,0,1},
			{100,-1,0,1},
			{-100,-1,0,1},

			// y
			{-1,-100,0,1},
			{1,-100,0,1},
			{1,100,0,1},
			{-1,100,0,1},

			// z
			{-1,0,100,1},
			{1,0,100,1},
			{1,0,-100,1},
			{-1,0,-100,1},

			// x箭头
			{ 100,5,0,1 },
			{120,0,0,1},
			{100,-5,0,1},

			// y箭头
			{-5,100,0,1},
			{5,100,0,1},
			{0,120,0,1},

			// z箭头
			{0,5,100,1},
			{0,-5,100,1},
			{0,0,120,1}
		};
		vector<array<double, 2>> axis_uvs = {
			{0,0},
			{0,0},
			{0,0},
			{0,0},

			{0,0},
			{0,0},
			{0,0},
			{0,0},

			{0,0},
			{0,0},
			{0,0},
			{0,0},

			{0,0},
			{0,0},
			{0,0},

			{0,0},
			{0,0},
			{0,0},

			{0,0},
			{0,0},
			{0,0},

		};
		vector<Color > axis_colors = {
			RED,
			RED,
			RED,
			RED,

			GREEN,
			GREEN,
			GREEN,
			GREEN,

			BLUE,
			BLUE,
			BLUE,
			BLUE,

			RED,
			RED,
			RED,

			GREEN,
			GREEN,
			GREEN,

			BLUE,
			BLUE,
			BLUE,
		};
		VertexBuffer* axis_vb = new VertexBuffer(axis_vertexes, axis_uvs, axis_colors);
		vector<array<unsigned, 3>> axis_indices = {
			{0,1,2},
			{3,2,0},

			{4,5,6},
			{7,6,4},

			{8,9,10},
			{11,10,8},

			{12,13,14},
			{15,16,17},
			{18,19,20}
		};
		IndexBuffer* axis_ib = new IndexBuffer(axis_indices);
		axis_painter = new Painter(axis_vb, axis_ib, nullptr, true, Framework::BLEND_OPAQUE);
	}
}