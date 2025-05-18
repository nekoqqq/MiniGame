#pragma once
#include <vector>
#include <array>
#include <cassert>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"
#include "Math.h"
#include "Xml.h"
#include "Collision.h"

using namespace GameLib::Input;
using GameLib::Framework;
using std::vector;
using std::array;
namespace GameLib {
	class Texture;
}

enum Color {
	WHITE = 0xffffffff,
	RED = 0xffff0000,
	GREEN = 0xff00ff00,
	BLUE = 0xff0000ff,
	H0 = 0xff8f0000,
	H1 = 0xff008f00,
	H2 = 0xff00008f,
	Q0 = 0xff4f0000,
	Q1 = 0xff004f00,
	Q2 = 0xff00004f
};


class VertexBuffer {
public:
	VertexBuffer() {}
	VertexBuffer(int n) // 存了多少点的坐标
	{
		points_.resize(n);
		uvs_.resize(n);
		colors_.resize(n);
	}
	VertexBuffer(const vector<Vector3>& vertexes, const vector<array<double, 2>>& uvs, const vector<Color>& colors = {}) {
		points_ = vertexes;
		uvs_ = uvs;
		if (colors.empty()) {
			for (int i = 0; i < points_.size(); i++) {
				colors_.push_back(WHITE);
			}
		}
		else
			colors_ = colors;
	}
	VertexBuffer(const Element* e) {
		for (auto& child : e->getChildren()) {
			string raw = child->getAttr("position");
			vector<double> point = Element::converToArray<double>(raw);
			assert(point.size() == 3);
			points_.push_back({point[0], point[1], point[2]});

			string raw_uv = child->getAttr("uv");
			vector<double> uv = Element::converToArray<double>(raw_uv);
			assert(uv.size() == 2);
			uvs_.push_back({ uv[0],uv[1] });

			string raw_color = child->getAttr("color");
			unsigned color = Element::convertHexToUnsigned(raw_color);
			colors_.push_back(static_cast<Color>(color));
		}
		this->name = e->getAttr("name");
		GameLib::cout << "create vertex buffer: " << name.c_str() << GameLib::endl;
	}
	~VertexBuffer()
	{
		GameLib::cout << "delete vertex buffer: " << name.c_str() << GameLib::endl;
	}
	int size()const {
		return points_.size();
	}
	void setInternal(const vector<Vector3>& vertexes,const vector<array<double, 2>>& uv){
		points_ = vertexes;
		uvs_ = uv;
	}
	void setPoint(int i, Vector3& v) {
		assert(0 <= i && i < points_.size());
		points_[i] = v;
	}
	const Vector3 vertex(int i)const {
		assert(0 <= i && i < points_.size());
		return points_[i];
	}
	Vector3& vertex(int i) {
		return points_[i];
	}
	const array<double, 2> uv(int i)const {
		assert(0 <= i && i < points_.size());
		return uvs_[i];
	}
	array<double, 2>& uv(int i) {
		return uvs_[i];
	}
	Color& color(int i) {
		return colors_[i];
	}
	const Color& color(int i)const {
		return colors_[i];
	}

private:
	string name;
	vector<Vector3> points_;
	vector<array<double, 2>> uvs_;
	vector<Color> colors_;
};

class IndexBuffer {
public:
	IndexBuffer() {}
	IndexBuffer(int n) {
		indices.resize(n);
	}
	IndexBuffer(const vector<array<unsigned, 3>>& arr) {
		indices = arr;
	}
	IndexBuffer(const Element* e) {
		for (auto& child : e->getChildren()) {
			string raw = child->getAttr("indices");
			vector<unsigned> point = Element::converToArray<unsigned>(raw);
			assert(point.size() == 3);
			indices.push_back({ point[0], point[1], point[2] });
		}
		this->name = e->getAttr("name");
		GameLib::cout << "create index buffer: " << name.c_str() << GameLib::endl;
	}
	~IndexBuffer() {
		GameLib::cout << "delete index buffer: " << name.c_str() << GameLib::endl;
	}
	void setIndex(int i, array<unsigned, 3> arr) {
		assert(0 <= i && i < indices.size());
		indices[i] = arr;
	}
	int size()const {
		return indices.size();
	}
	const array<unsigned, 3> operator[](unsigned i)const {
		return indices[i];
	}
	array<unsigned, 3>& operator[](unsigned i) {
		return indices[i];
	}
private:
	string name;
	vector<array<unsigned, 3>> indices;
};

class Texture {
public:
	Texture(const Element* e) {
		const string file_name = e->getAttr("file_name");
		Framework::instance().createTexture(&texture, file_name.c_str());
		this->name = e->getAttr("name");
		GameLib::cout << "create texture: " << name.c_str() << GameLib::endl;
	}
	~Texture() {
		Framework::instance().destroyTexture(&texture);
		GameLib::cout << "delete texture: "<<name.c_str() << GameLib::endl;
	}
	void set()const {
		Framework::instance().setTexture(texture);
	}
private:
	string name;
	GameLib::Texture* texture;
};

class Painter {
public:
	Painter(VertexBuffer*vb,IndexBuffer *ib,Texture* t,bool isZTest,GameLib::Framework::BlendMode mode):vb_(vb),ib_(ib),texture_(t),isZTest_(isZTest),blend_mode_(mode){}
	~Painter() {
		delete vb_;
		vb_ = nullptr;

		delete ib_;
		ib_ = nullptr;

		delete texture_;
		texture_ = nullptr;
	}
	const string& getName()const {
		return name;
	}
	void draw(Matrix44 &pvm){
		vector<Vector3> res(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			res[i] = pvm.vecMul(vb_->vertex(i));
		}
		Framework f = Framework::instance();
		if (blend_mode_ == Framework::BLEND_OPAQUE) {
			f.enableDepthWrite(true);
		}
		else {
			f.enableDepthWrite(false);
		}
		if (texture_)
			texture_->set();
		else
			f.setTexture(nullptr); // TODO这里的封装不太优雅
		f.enableDepthTest(isZTest_);
		f.setBlendMode(blend_mode_);
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data(),vb_->color(i0), vb_->color(i1), vb_->color(i2));
		}
	}
private:
	string name; // 文件中的名称
	VertexBuffer* vb_;
	IndexBuffer* ib_;
	Texture* texture_;
	bool isZTest_;
	GameLib::Framework::BlendMode blend_mode_;
};

class Model {
public:
	enum Type {
		UNKNOW,
		STAGE,
		PLAYER,
		ENEMY,
		AXIS
	};
	Model(Type type,const Vector3 &pos, Painter *painter, const Matrix44 &m ):type_(UNKNOW),pos_(pos), painter_(painter),world_rotation_(m) {}
	virtual ~Model() {
		delete painter_;
		painter_ = nullptr;
	}
	void draw(const Matrix44 &pv){
		Matrix44 model_transform = getModelTransform();
		Matrix44 pvm = pv.matMul(model_transform);
		painter_->draw(pvm);
	}
	virtual void update(const Matrix44& vr) = 0;


	void setCuboid(const Vector3 & center, const Vector3 & half) {
		cuboid_.x = center.x;
		cuboid_.y = center.y;
		cuboid_.z = center.z;
		cuboid_.half_x = half.x;
		cuboid_.half_y = half.y;
		cuboid_.half_z = half.z;
	}

	void setCollisionModels(const vector<Model*>& collision_model) {
		collision_models_ = collision_model;
	}
	const vector<Model*>& getCollisionModels()const {
		return collision_models_;
	}

	bool isCollision(const Model* other) {
		return this->cuboid_.isCollision(other->cuboid_);
	}
	const Vector3& getPos() const {
		return pos_;
	}
	Vector3 getZDirection() {
		Vector3 v(0, 0, 1);
		Matrix44 world_transform = getModelRotation();
		return world_transform.vecMul(v);
	}
	Vector3 getWorldCoor(const Vector3 &v) {

	}
protected:
	const Matrix44& getModelRotation()const{
		return world_rotation_;
	}
	void setPos(double x, double y, double z) {
		setPos({ x,y,z });
	}
	void setPos(const Vector3& v) {
		pos_ = v;
	}
	Matrix44 getModelTransform() const {
		Matrix44 r = getModelRotation();
		r[0][3] = pos_.x;
		r[1][3] = pos_.y;
		r[2][3] = pos_.z;
		return r;
	}
private:
	Type type_;
	Vector3 pos_;
	Painter* painter_;
	Matrix44 world_rotation_;
	Cuboid cuboid_;
	vector<Model*> collision_models_; // 会发生碰撞的其他物体
};

class Mecha :public Model {
public:
	Mecha(Type type, const Vector3& pos, Painter* painter, const Matrix44& m = Matrix44::identity()) :Model(type, pos, painter, m) {
		setCuboid(pos,getCuboidHalf());
	}
	~Mecha() {
	}
	virtual void update(const Matrix44& vr)override {
		static int y_counter = 0;
		Keyboard k = Manager::instance().keyboard();
		double dx = 0., dy = 0., dz = 0.;
		if (k.isOn('w')) {
			dz = 1.0;
		}
		if (k.isOn('a')) {
			dx = -1.0;
		}
		if (k.isOn('s')) {
			dz = -1.0;
		}
		if (k.isOn('d')) {
			dx = 1.0;
		}
		// 这里的移动是在相机坐标系内移动
		// 移动前坐标为Y，世界坐标X = AY+C，
		// 移动后坐标为Y',世界坐标X' = AY'+C,
		// 两式相减X'-X = A(Y'-Y) => X'=X+A(Y'-Y) => X'=X+A delta，其中delta是相机坐标系中的位移量
		// 或者 X=AY+C => X=A(Y+delta)+c => X=AY + c +A delta，增加量还是旋转*delta
		const Vector3 old_pos = getPos();
		Vector3 move_vector = vr.vecMul({ dx,dy,dz });
		// 这里对于y的方向不需要根据相机的位置来决定，绝对和世界垂直
		if (y_counter > 0) { // 跳跃过程中
			move_vector.y = 1.0;
			if (y_counter++ == 20)
				y_counter = 0;
		}
		else if(k.isTriggered(' ')){ // 按下空格键
			y_counter = 1;
			move_vector.y = 1;
		}
		else {
			move_vector.y = -1.0;
		}
		// 存在一个方向，使得和其他所有物体都不相撞，才可以移动
		// 反之，存在一个物体，所有方向都和他相撞，则不可以移动
		vector<Vector3> possible_move_vectors = {
			move_vector,
			{0.0,move_vector.y,move_vector.z},
			{move_vector.x,0.0,move_vector.z},
			{move_vector.x,move_vector.y,0.0},
			{0.0,0.0,move_vector.z},
			{0.0,move_vector.y,0.0},
			{move_vector.x,0.0,0.0}
		};
		for (auto& v : possible_move_vectors) {
			setCuboid(old_pos + v, getCuboidHalf());
			bool could_move = true;
			for (auto& other_model : getCollisionModels()) {
				if (isCollision(other_model)) {
					could_move = false;
					break;
				}
			}
			if (could_move) {
				setPos(old_pos + v);
				break;
			}
		}
	};
private:
	Vector3 getCuboidHalf()const {
		return { 10,10,10 };
	}
};

class Stage :public Model
{
public:
	Stage(Type type, Painter* painter) :Model(type, { 0.0,0.0,0.0 }, painter, Matrix44::identity()) {
		setCuboid({ 0.0,0.0,0.0 }, { 1000.0, 0.0, 1000.0 });
	};
	~Stage() {
	};
	virtual void update(const Matrix44& vr)override {}
};

// 绘制辅助坐标轴
class Axis :public Model {
public:
	Axis(Type type, Painter* painter) :Model(type, { 0.0,0.0,0.0 }, painter, Matrix44::identity()) {}
	~Axis() {}
	virtual void update(const Matrix44& pvm) override {}
};


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
				vbs[name]=new VertexBuffer(child);
			}
			else if (tag_name == "IndexBuffer") {
				ibs[name] = (new IndexBuffer(child));
			}
			else if (tag_name == "Texture") {
				textures[name] = new Texture(child);
			}
		}
		// Painter依赖他们
		for (auto& child : root->getChildren()) {
			if (child->getTagName() == "Painter"){
				const string& name = child->getAttr("name");
				const string& vb_name = child->getAttr("vertexBuffer");
				const string& ib_name = child->getAttr("indexBuffer");
				const string& texture_name = child->getAttr("texture");
				const string& blend = child->getAttr("blend");
				Framework::BlendMode blend_mode=Framework::BLEND_OPAQUE;
				if (blend == "linear") {
					blend_mode = Framework::BLEND_LINEAR;
				}else if (blend == "additive")
					blend_mode = Framework::BLEND_ADDITIVE;
				bool is_ztest = true;
				Painter* painter = new Painter(vbs[vb_name], ibs[ib_name], textures[texture_name], is_ztest, blend_mode);
				painters[name] = painter;
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
	}
	Model* createModel(Model::Type type, const char* name) {
		Model* new_model = nullptr;
		if (painters.count(name)) {
			Painter *p = painters[name];
			if (type == Model::PLAYER)
				new_model = new Mecha(type, { 0.0,10.0,-50.0 }, p);
			else if (type == Model::ENEMY)
				new_model = new Mecha(type, { 0.0,10.0,50.0 }, p);
			else if (type == Model::STAGE)
				new_model = new Stage(type, p);
			else if (type == Model::AXIS)
				new_model = new Axis(type, p);
		}
		return new_model;
	}
private:
	unordered_map<string,VertexBuffer*> vbs;
	unordered_map<string,IndexBuffer*> ibs;
	unordered_map<string,Texture*> textures;
	unordered_map<string,Painter*> painters;

};

class Camera {
public:
	Camera(const Vector3& eye_pos, const Vector3& target_pos, const Vector3& up, double fov_y, double near, double far, double aspect_ratio) :eye_pos_(eye_pos), target_pos_(target_pos), up_(up), fov_y(fov_y), near(near), far(far), aspect_ratio(aspect_ratio) {
		setProjectionTransform();
	}
	~Camera() {}
	Matrix44 getViewRotation() {
		Vector3 e3 = (target_pos_ - eye_pos_).normalize();
		Vector3 e1 = up_.cross(e3).normalize();
		Vector3 e2 = e3.cross(e1).normalize();
		const double rot_t[][4] = {
			{e1.x,e2.x,e3.x,0},
			{e1.y,e2.y,e3.y,0},
			{e1.z,e2.z,e3.z,0},
			{0,0,0,1}
		};

		return Matrix44(rot_t);
	}

	Matrix44 getViewProjectionMatrix() {
		Matrix44 rotation = getViewRotation().transpose();
		const double trans_t[][4] = {
			{1.,0.,0.,-eye_pos_.x },
			{0.,1.,0.,-eye_pos_.y },
			{ 0.,0.,1.,-eye_pos_.z},
			{0,0,0,1}
		};
		Matrix44 trans(trans_t);
		return projectionTransform.matMul(rotation.matMul(trans));
	}
	void update(Model* player) {
		const Vector3& origin = player->getPos();
		const Vector3& z_dir = player->getZDirection();
		eye_pos_ = origin;
		eye_pos_ -= z_dir * 60;
		eye_pos_.y += 20;
		target_pos_ = origin + z_dir*10;
	}
private:
	Vector3 eye_pos_;
	Vector3 target_pos_;
	Vector3 up_;
	double fov_y;
	double near;
	double far;
	double aspect_ratio;
	Matrix44 projectionTransform;

	void setProjectionTransform() {
		double(&p)[4][4] = projectionTransform.p;
		p[1][1] = 1 / tan(fov_y * 0.5);
		p[0][0] = p[1][1] / aspect_ratio;
		p[2][2] = far / (far - near);
		p[2][3] = -near * far / (far - near);
		p[3][2] = 1;
	}
};