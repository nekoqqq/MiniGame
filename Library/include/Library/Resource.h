#pragma once
#include "Model.h"
#include "Missle.h"
#include "Mecha.h"
#include "Camera.h"
#include "TransformTree.h"
#include "AnimationTree.h"
extern const int MAX_TIME;
using std::pair;
class Resource
{
public:
	Resource(const char* file_name) {
		XMLParser xml_parser(file_name);
		const Element* root = xml_parser.getRoot();

		for (auto& child : root->getChildren()) {
			const string& tag_name = child->getTagName();
			const string& name = child->getAttr("name");
			if (name == "") // 没有name的被忽略了
				continue;
			if (tag_name == "VertexBuffer") {
				vbs[name] = new VertexBuffer(child);
			}
			else if (tag_name == "IndexBuffer") {
				ibs[name] = (new IndexBuffer(child));
			}
			else if (tag_name == "Texture") {
				textures[name] = new Texture(child);
			}
			else if (tag_name == "Animation") {
				animation_trees_[name] = new AnimationTree(child);
			}
		}
		// Painter依赖他们
		for (auto& child : root->getChildren()) {
			if (child->getTagName() == "Painter") {
				const string& name = child->getAttr("name");
				const string& vb_name = child->getAttr("vertexBuffer");
				const string& ib_name = child->getAttr("indexBuffer");
				const string& texture_name = child->getAttr("texture");
				const string& blend = child->getAttr("blend");
				Framework::BlendMode blend_mode = Framework::BLEND_OPAQUE;
				if (blend == "linear") {
					blend_mode = Framework::BLEND_LINEAR;
				}
				else if (blend == "additive")
					blend_mode = Framework::BLEND_ADDITIVE;
				bool is_ztest = true;
				Painter* painter = new Painter(vbs[vb_name], ibs[ib_name], textures[texture_name], is_ztest, blend_mode);
				painter->setName(name);
				painters[name] = painter;
				const string& origin = child->getAttr("origin");
				vector<double> origin_array = Element::converToArray<double>(origin);
				if (origin_array.empty()) {
					origin_array = vector<double>{ 0.0,0.0,0.0 };
				}
				origins[name] = new Vector3(origin_array[0], origin_array[1], origin_array[2]);
			}
		}

		// TransformTree依赖painter
		for (auto& child : root->getChildren()) {
			const string& tag_name = child->getTagName();
			const string& name = child->getAttr("name");
			if (name == "") // 没有name的被忽略了
				continue;
			else if (tag_name == "TransformTree") {
				transform_trees_[name] = new TransformTree(child, this);
			}
		}

	}
	~Resource() {
		for (auto& kv : vbs)
			delete kv.second;
		for (auto& kv : ibs)
			delete kv.second;
		for (auto& kv : textures)
			delete kv.second;
		for (auto& kv : painters)
			delete kv.second;
		for (auto& kv : origins)
			delete kv.second;
	}
	TransformTree* createTransformTree(const string& name)const{
		auto it = transform_trees_.find(name);
		if (it != transform_trees_.end())
			return it->second;
		return nullptr;
	}
	Model* createModel(Model::Type type, CollisionModel::Type collision_type, const string& name) {
		Model* new_model = nullptr;
		if (painters.count(name)) {
			Painter* p = painters[name];
			const Vector3& origin = *origins[name];
			if (type == Model::PLAYER)
				new_model = new Mecha(type, origin, p, collision_type); // 这里必须让他一开始的坐标大于0，设置碰撞中心比地面高一点，不然由于和地面重合，导致无法跳起来
			else if (type == Model::ENEMY)
				new_model = new Mecha(type, origin, p, collision_type);
			else if (type == Model::STAGE)
				new_model = new Stage(type, p, collision_type);
			else if (type == Model::AXIS)
				new_model = new Axis(type, p, collision_type);
			else if (type == Model::MISSLE)
				new_model = new Missle(type, origin, p, collision_type, Matrix44::identity());
		}
		return new_model;
	}
	Painter* getPainter(const string& name)const {
		auto it = painters.find(name);
		if (it!=painters.end()) {
			return it->second;
		}
		return nullptr;
	}
	AnimationTree* getAnimation(const string& name)const {
		auto it = animation_trees_.find(name);
		if (it != animation_trees_.end()) {
			return it->second;
		}
		return nullptr;
	}

private:
	unordered_map<string, VertexBuffer*> vbs;
	unordered_map<string, IndexBuffer*> ibs;
	unordered_map<string, Texture*> textures;
	unordered_map<string, Painter*> painters;
	unordered_map<string, AnimationTree*> animation_trees_;
	unordered_map<string, TransformTree*> transform_trees_;
	unordered_map<string, Vector3*> origins; // 各个物体位于世界坐标系中的坐标
};
struct MechaInfo
{

	int player_hp;
	int player_energy;
	bool player_lock_on;
	int enemy_hp;
	int enemy_energy;
	double map_half; // 地图的一半
	Vector3 player_pos;
	Vector3 enemy_pos;

	Vector2D enemy_nvi_pos; // 敌人的二维坐标


	void update(Mecha *player,Mecha*enemy,Stage *stage,Camera *camera)
	{
		player_hp = player->getHP();
		player_energy = player->energy_;
		player_lock_on = player->lock_on_;

		enemy_hp = enemy->getHP();
		enemy_energy = enemy->energy_;

		map_half = stage->getHalf();
		player_pos = player->getPos();
		enemy_pos = enemy->getPos();
		Vector3 enemy_pvm = camera->getViewProjectionMatrix().matMul(enemy->getModelTransform()).vecMul(enemy_pos-player_pos); //TODO 不清楚这里为什么减去player的坐标之后这个就正确了，可能是因为相机的坐标是按照人物的位置设置的
		enemy_nvi_pos = { enemy_pvm.x/enemy_pvm.w,enemy_pvm.y/enemy_pvm.w };
	}
};
class FrontEnd
{
public:
	FrontEnd(MechaInfo*mech_info):mecha_info_(mech_info){}
	void draw()
	{
		drawHP();
		drawEnergy();
		drawMap();
		drawTimer();
		drawLockOn();
	}
	void update(Mecha*player,Mecha*enemy,Stage*stage,Camera*camera)
	{
		mecha_info_->update(player, enemy,stage,camera);
	}

	void drawRect(vector<Vector2D> xy_points, unsigned c1 = 0xffffffff, unsigned c2 = 0xffffffff) // 实现从左边到右边的渐变效果
	{
		vector<Vector3> points;
		for (auto& xy_point : xy_points)
			points.push_back({xy_point.x, xy_point.y,0,1, });
		assert(points.size() == 4);
		Framework f = Framework::instance();
		f.enableDepthTest(false);
		f.enableDepthWrite(false);
		f.setTexture(nullptr);
		f.drawTriangle3DH(&points[0].x, &points[1].x, &points[2].x, nullptr, nullptr, nullptr, c1, c1, c2);
		f.drawTriangle3DH(&points[0].x, &points[2].x, &points[3].x, nullptr, nullptr, nullptr, c1, c2, c2);
	}
	void drawPlayerHP()
	{
		vector<Vector2D> background = {
		{-0.9,0.9}, // 0 
		{-0.9,0.8}, // 1
		{-0.2,0.8}, // 2
		{-0.2,0.9}, // 3
		};
		unsigned raw_c1 = 0xF000C000;
		unsigned raw_c2 = 0xF000ff00;
		unsigned c1 = 0xffffffff;
		unsigned c2 = 0xffffffff;
		double rate = 1.0 * mecha_info_->player_hp / MAX_HP;
		drawProgerss(background, rate, raw_c1, raw_c2, c1, c2, { {2,1},"HP" });
	}
	void drawEnemyHp()
	{
		vector<Vector2D> background = {
			{0.2,0.8}, // 3
			{0.2,0.9}, // 2
			{0.9,0.9}, // 1
			{0.9,0.8}, // 0

		};
		unsigned raw_c1 = 0xF00000C0;
		unsigned raw_c2 = 0xF00000ff;
		unsigned c1 = 0xffffffff;
		unsigned c2 = 0xffffffff;
		double rate = 1.0 * mecha_info_->enemy_hp / MAX_HP;
		drawProgerss(background, rate, raw_c1, raw_c2, c1, c2, { {2,40},"ENEMY HP" });
		
	}
	void drawHP()
	{
		Framework::instance().setBlendMode(Framework::BLEND_LINEAR);
		drawPlayerHP();
		drawEnemyHp();
	}
	void drawEnergy()
	{
		Framework::instance().setBlendMode(Framework::BLEND_ADDITIVE);
		vector<Vector2D> background = {
			{-0.9,0.7},
			{-0.9,0.6},
			{-0.1,0.6},
			{-0.1,0.7}
		};
		unsigned raw_c1 = 0xff884422;
		unsigned raw_c2 = 0xffDD4422;
		unsigned c1 = 0xffffffff;
		unsigned c2 = 0xffffffff;
		double rate = 1.0 * mecha_info_->player_energy / MAX_ENEGY;
		drawProgerss(background, rate, raw_c1, raw_c2, c1, c2,{{5,1},"EN"});
	}
	void drawProgerss(const vector<Vector2D> &background,double rate,unsigned raw_c1,unsigned raw_c2,unsigned c1,unsigned c2,pair<pair<int,int>,const char* > texture)
	{
		drawRect(background, raw_c1, raw_c2);
		vector<Vector2D> eliminated(4);
		eliminated[0] = { background[0].x * (1 - rate) + background[3].x * rate,background[0].y * (1 - rate) + background[3].y * rate };
		eliminated[1] = { background[1].x * (1 - rate) + background[2].x * rate,background[1].y * (1 - rate) + background[2].y * rate };
		eliminated[2] = background[2];
		eliminated[3] = background[3];
		drawRect(eliminated, c1, c2);
		Framework::instance().drawDebugString(texture.first.second, texture.first.first, texture.second);
	}
	void drawTimer()
	{
		Framework::instance().setBlendMode(Framework::BLEND_LINEAR);
		vector<Vector2D> background = {
			{-0.8,-0.9},
			{-0.8,-0.95},
			{0.8,-0.95},
			{0.8,-0.9}
		};
		unsigned raw_c1 = 0xff884422;
		unsigned raw_c2 = 0xffDD4422;
		unsigned c1 = 0xffffffff;
		unsigned c2 = 0xffffffff;
		double rate = 1.0-1.0*gCounter/ MAX_TIME;
		drawProgerss(background, rate, raw_c1, raw_c2, c1, c2,{{28,0},"Timer"});
	}
	void drawMap()
	{
		Framework::instance().setBlendMode(Framework::BLEND_LINEAR);
		Vector2D center = { 0.75,-0.6 };
		double a = 0.20; // 宽的一半
		double b = 0.20; // 高的一半
		vector<Vector2D> background = {
			{center.x - b,center.y + a},
			{center.x - b,center.y - a},
			{center.x + b,center.y - a},
			{center.x + b,center.y + a},
		};

		double r = 0.01;
		Vector2D player_center = { center.x +b*mecha_info_->player_pos.x / mecha_info_->map_half, center.y + a*mecha_info_->player_pos.z / mecha_info_->map_half };
		vector<Vector2D> player_rect = {
			{player_center.x -r,player_center.y + r},
			{player_center.x - r,player_center.y - r},
			{player_center.x + r,player_center.y - r},
			{player_center.x + r,player_center.y + r},
		};

		Vector2D enemy_center = { center.x + b*mecha_info_->enemy_pos.x / mecha_info_->map_half, center.y + a*mecha_info_->enemy_pos.z / mecha_info_->map_half };
		vector<Vector2D> enemy_rect = {
			{enemy_center.x - r,enemy_center.y + r},
			{enemy_center.x - r,enemy_center.y - r},
			{enemy_center.x + r,enemy_center.y - r},
			{enemy_center.x + r,enemy_center.y + r},
		};
		unsigned bg_color = 0x7f402020;
		drawRect(background,bg_color,bg_color);
		unsigned player_color = 0xff009f00;
		unsigned enemy_color = 0xf0ff2000;
		drawRect(player_rect,player_color,player_color);
		drawRect(enemy_rect,enemy_color,enemy_color);
	}
	void drawLockOn()
	{
		if (!mecha_info_->player_lock_on)
			return;
		Framework::instance().setBlendMode(Framework::BLEND_ADDITIVE);

		double a_half = 0.05; // 宽
		double b_half = 0.1; // 长

		double r_a = 0.01; // 宽的一半
		double r_b = 0.02; // 长的一半
		vector<Vector2D> left = {
			{mecha_info_->enemy_nvi_pos.x-a_half - r_b,mecha_info_->enemy_nvi_pos.y + r_a},
			{mecha_info_->enemy_nvi_pos.x-a_half - r_b,mecha_info_->enemy_nvi_pos.y  - r_a},
			{mecha_info_->enemy_nvi_pos.x-a_half + r_b,mecha_info_->enemy_nvi_pos.y  - r_a},
			{mecha_info_->enemy_nvi_pos.x-a_half + r_b,mecha_info_->enemy_nvi_pos.y + r_a},
		};
		vector<Vector2D> right = {
			{mecha_info_->enemy_nvi_pos.x + a_half - r_b,mecha_info_->enemy_nvi_pos.y + r_a},
			{mecha_info_->enemy_nvi_pos.x + a_half - r_b,mecha_info_->enemy_nvi_pos.y - r_a},
			{mecha_info_->enemy_nvi_pos.x + a_half + r_b,mecha_info_->enemy_nvi_pos.y - r_a},
			{mecha_info_->enemy_nvi_pos.x + a_half + r_b,mecha_info_->enemy_nvi_pos.y + r_a},
		};
		vector<Vector2D> down = {
			{mecha_info_->enemy_nvi_pos.x - r_a,mecha_info_->enemy_nvi_pos.y - b_half + r_b},
			{mecha_info_->enemy_nvi_pos.x - r_a,mecha_info_->enemy_nvi_pos.y - b_half - r_b},
			{mecha_info_->enemy_nvi_pos.x + r_a,mecha_info_->enemy_nvi_pos.y - b_half - r_b},
			{mecha_info_->enemy_nvi_pos.x + r_a,mecha_info_->enemy_nvi_pos.y - b_half + r_b},
		};		
		vector<Vector2D> up = {
			{mecha_info_->enemy_nvi_pos.x - r_a,mecha_info_->enemy_nvi_pos.y + b_half + r_b},
			{mecha_info_->enemy_nvi_pos.x - r_a,mecha_info_->enemy_nvi_pos.y + b_half - r_b},
			{mecha_info_->enemy_nvi_pos.x + r_a,mecha_info_->enemy_nvi_pos.y + b_half - r_b},
			{mecha_info_->enemy_nvi_pos.x + r_a,mecha_info_->enemy_nvi_pos.y + b_half + r_b},
		};

		unsigned c1 = 0xffff000f;
		unsigned c2 = c1;
		drawRect(left, c1, c2);
		drawRect(right, c1, c2);
		drawRect(up, c1, c2);
		drawRect(down, c1, c2);
	}

private:
	MechaInfo* mecha_info_;
};