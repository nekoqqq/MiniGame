#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Mouse.h"
#include "Library/Model.h"
#include "Library/Graph.h"
#include "Library/Camera.h"
#include "Library/Resource.h"
#include "Library/Mecha.h"
using namespace std;
using namespace GameLib::Input;
const double PI = 3.141592653589793238;
const int WIDTH = 640;
const int HEIGHT = 480;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
// 游戏
const int FRAMES = 180;
const int MAX_TIME = 120 * FRAMES; // 最大对局时间
// 动物
Model* gPlayer;
Model* gEnemy;
// 静物
Model* gStage;
Model* gWall;
Model* gAxis;
// 变换矩阵
Matrix44 gProjectionTransform;
Matrix44 gViewTransform;
// 摄像机
Camera* gCamera;
Vector3 gEyePos(0, 0, 1);
Vector3 gTargetPos(0, 0, 0);
Vector3 gEyeUp(0., 1., 0.); // 一般是向上
const double fov_y = PI / 3;
const double aspec_ratio = 1.0 * WIDTH / HEIGHT;
const double near = 1.0;
const double far = 1000.0;
int gCounter = 0;
// 资源类
Resource* gResource;
FrontEnd* gFrontEnd;
MechaInfo* gMechaInfo;
// 光源
Vector3 gLightColor = { 1.0, 1.0,1.0 };
Vector3 gAmbient = { 0.01,0.01,0.01};
Vector3 gLightDir = { 0.0,0.0,0.0 };
Light* gLight = nullptr;
// 太阳系模拟
Model* gSun;
Model* gEarth;
Model* gMoon;
TransformNode* gSunRotation;
TransformNode* gSunTranslation;
TransformNode* gEarthRotation;
TransformNode* gEarthTranslation;
TransformNode* gMoonRotation;
TransformNode* gMoonTranslation;
// 太阳系v3模拟
TransformTree* gSolarSystem;
TransformNode* gTSun;
TransformNode* gTEarth;
TransformNode* gTMoon;
AnimationTree* gPlayerAnimation;
namespace GameLib {
	bool firstFrame = true;
	void deleteAll() {
		SAFE_DELETE(gPlayer);
		SAFE_DELETE(gEnemy);
		SAFE_DELETE(gStage);
		SAFE_DELETE(gWall);
	}
	void solarSystem() {
		gEyePos.set(0, 200, 100);
		gTargetPos.set(0, 0, 0);
		gEyeUp.set(0., 1., 0.); // 一般是向上
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			Framework::instance().setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gSun = gResource->createModel(Model::PLAYER, CollisionModel::SPHERE, "player");
			gEarth = gResource->createModel(Model::PLAYER, CollisionModel::SPHERE, "player");
			gMoon = gResource->createModel(Model::PLAYER, CollisionModel::SPHERE, "player");
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");
			++gCounter;
			// 设置光源,模拟太阳东升西落
			gLight = new Light(gLightDir, gLightColor, gAmbient);
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);

		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		// 光处理
		double theta = gCounter;
		double t = 1.0 * gCounter / FRAMES / 6;
		//gLight->updateLight({cos(t)*cos(t),cos(t)*sin(t)*sin(t),sin(t)});
		gLight->updateLight({ 1,1,1 });
		Matrix44 R_solar;

		R_solar.setRotationY(gCounter * 100.0 / 365.0);

		Matrix44 T_earth;
		T_earth[0][3] = 100.0;
		T_earth[0][0] = T_earth[1][1] = T_earth[2][2] = T_earth[3][3] = 1.0;

		Matrix44 R_earth;
		R_earth.setRotationY(gCounter * 100.0 / 1.0);

		Matrix44 R_moon;
		R_moon.setRotationY(gCounter * 100.0 / 30.0);

		Matrix44 T_moon;
		T_moon[0][3] = 30.0;
		T_moon[0][0] = T_moon[1][1] = T_moon[2][2] = T_moon[3][3] = 1.0;

		gStage->draw(pv, gLight);
		gSun->draw(pv.matMul(R_solar), gLight);
		gEarth->draw(pv.matMul(R_solar).matMul(T_earth).matMul(R_earth), gLight); // 地球自转的同时绕着太阳公转
		gMoon->draw(pv.matMul(R_solar).matMul(T_earth).matMul(R_moon).matMul(T_moon), gLight); // 月球自转的同时绕着地球公转

		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		gCounter++;
	}
	void solarSystemV2() {
		gEyePos.set(0, 200, 100);
		gTargetPos.set(0, 0, 0);
		gEyeUp.set(0., 1., 0.); // 一般是向上
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			Framework::instance().setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");

			gSunRotation = new TransformNode(gResource->getPainter("player"));
			gEarthRotation = new TransformNode(gResource->getPainter("player"));
			gEarthTranslation = new TransformNode();
			gMoonRotation = new TransformNode();
			gMoonTranslation = new TransformNode(gResource->getPainter("player"));

			gSunRotation->addChild(gEarthTranslation);
			gEarthTranslation->addChild(gEarthRotation);
			gEarthTranslation->addChild(gMoonRotation);
			gMoonRotation->addChild(gMoonTranslation);
			++gCounter;
			// 设置光源,模拟太阳东升西落
			gLight = new Light(gLightDir, gLightColor, gAmbient);
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);

		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		// 光处理
		double theta = gCounter;
		double t = 1.0 * gCounter / FRAMES / 6;
		gLight->updateLight({cos(t),sin(t),1});
		//gLight->updateLight({ 1,1,1 });

		gSunRotation->setRotation({ 0,gCounter * 100.0 / 365.0 ,0 });
		gEarthRotation->setRotation({ 0,gCounter * 100.0 / 1.0 ,0 });
		gEarthTranslation->setTranslation({ 100,0,0 });
		gMoonTranslation->setTranslation({ 30.0,0,0 });
		gMoonRotation->setRotation({ 0,gCounter * 100.0 / 30.0 ,0 });

		gSunRotation->draw(pv, Matrix44::identity(), gLight);

		gStage->draw(pv, gLight);
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		gCounter++;
	}
	void solarSystemV3() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			gEyePos.set(0, 200, 100);
			gTargetPos.set(0, 0, 0);
			gEyeUp.set(0., 1., 0.); // 一般是向上
			Framework::instance().setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");
			gSolarSystem =  gResource->createTransformTree("SolarSystem");
			gTSun = gSolarSystem->getNode("SunRotation");
			gTEarth = gSolarSystem->getNode("EarthRotation");
			gTMoon = gSolarSystem->getNode("MoonRotation");
			// 设置光源,模拟太阳东升西落
			gLight = new Light(gLightDir, gLightColor, gAmbient);
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		// 光处理
		double theta = gCounter * PI / 180.0;
		double t = 1.0 * gCounter / FRAMES / 6;
		gLight->updateLight({ cos(t) * cos(t),cos(t) * sin(t) * sin(t),sin(t) });

		gStage->draw(pv, gLight);
		gTSun->setRotation({ 0.0, -gCounter * 10.0 / 365.0, 0.0 });
		gTEarth->setRotation({ 0.0, -gCounter * 10.0 / 1.0, 0.0 });
		gTMoon->setRotation({ 0.0, -gCounter * 10.0 / 30.0, 0.0 });
		gSolarSystem->draw(pv,Matrix44::identity(),gLight);
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		gCounter++;
	}
	void solarSystemV4() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			gEyePos.set(0, 200, 100);
			gTargetPos.set(0, 0, 0);
			gEyeUp.set(0., 1., 0.); // 一般是向上
			Framework::instance().setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");
			gSolarSystem = gResource->createTransformTree("SolarSystem");
			gSolarSystem->setAnimation(gResource->getAnimation("SolarSystem"));
			// 设置光源,模拟太阳东升西落
			gLight = new Light(gLightDir, gLightColor, gAmbient);
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		// 光处理
		double theta = gCounter * PI / 180.0;
		double t = 1.0 * gCounter / FRAMES / 6;
		gLight->updateLight({ cos(t) * cos(t),cos(t) * sin(t) * sin(t),sin(t) });


		gSolarSystem->update();

		gStage->draw(pv, gLight);
		gSolarSystem->draw(pv, Matrix44::identity(), gLight);
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		gCounter++;
	}
	void Framework::update() {
		Keyboard k = Manager::instance().keyboard();
		Mouse m = Manager::instance().mouse();
		if (firstFrame) {
			setFrameRate(FRAMES);
			firstFrame = false;
			gResource = new Resource("model.xml");
			gPlayerAnimation = gResource->getAnimation("player");
			gPlayer = gResource->createModel(Model::PLAYER, CollisionModel::SPHERE, "player");
			for (int i = 0; i < MAX_MISSLES; i++) {
				Model * missle = gResource->createModel(Model::MISSLE, CollisionModel::SPHERE, "missle");
				dynamic_cast<Mecha*>(gPlayer)->addMissle(*missle);
			}
			gEnemy = gResource->createModel(Model::ENEMY, CollisionModel::SPHERE, string("enemy"));
			for (int i= 0;i<MAX_MISSLES;i++)
			{
				Model* missle = gResource->createModel(Model::MISSLE, CollisionModel::SPHERE, "missle");
				dynamic_cast<Mecha*>(gEnemy)->addMissle(*missle);
			}
			dynamic_cast<Mecha*>(gPlayer)->addEnemy(gEnemy);
			dynamic_cast<Mecha*>(gEnemy)->addEnemy(gPlayer);
			gStage = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "stage");
			gWall = gResource->createModel(Model::STAGE, CollisionModel::TRIANGLE, "wall");
			gAxis = gResource->createModel(Model::AXIS, CollisionModel::SPHERE, "axis");
			gCamera = new Camera(gEyePos, gTargetPos, gEyeUp, fov_y, near, far, aspec_ratio);
			// 设置碰撞物体
			gPlayer->addCollisionModel(gEnemy);
			gPlayer->addCollisionModel(gStage);
			gPlayer->addCollisionModel(gWall);
			gEnemy->addCollisionModel(gPlayer);
			gEnemy->addCollisionModel(gStage);
			gEnemy->addCollisionModel(gWall);
			gMechaInfo = new MechaInfo;
			gFrontEnd = new FrontEnd(gMechaInfo);
			++gCounter;
			// 设置光源,模拟太阳东升西落
			gLight = new Light(gLightDir, gLightColor, gAmbient);
		}
		// 更新
		// 注意，移动视点是在世界坐标系中移动，需要先算出世界坐标再减去长度，比如世界坐标1对应1m
		Matrix44 vr = gCamera->getViewRotation();
		gPlayer->update(vr); // 玩家和敌人共用一套逻辑
		gEnemy->update(vr);
		gCamera->update(gPlayer);
		// 绘制
		Matrix44 pv = gCamera->getViewProjectionMatrix();
		gFrontEnd->update(dynamic_cast<Mecha*>(gPlayer), dynamic_cast<Mecha*>(gEnemy), dynamic_cast<Stage*>(gStage), gCamera);
		// 光处理
		double theta = gCounter * PI / 180.0;
		double t = 1.0*gCounter / FRAMES/30;
		gLight->updateLight({cos(t)*cos(t),cos(t)*sin(t),sin(t)});
		gStage->draw(pv, gLight);
		gWall->draw(pv, gLight);
		gAxis->draw(pv, gLight);
		gPlayer->draw(pv, gLight);
		gEnemy->draw(pv, gLight);
		gFrontEnd->draw();
		
		if (!dynamic_cast<Mecha*>(gEnemy)->isAlive() || ++gCounter>=MAX_TIME) {
			drawDebugString(1, 0, "Game Over!");
			sleep(1000);
			deleteAll();
			firstFrame = true;
			gCounter = 0;
		}
		if (k.isOn('q'))
			GameLib::Framework::instance().requestEnd();
		if (isEndRequested()) {
			deleteAll();
		}
		gCounter++;
	}
}