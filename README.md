## 项目概述

参照【游戏开发：世嘉新人培训教材】一书，基于C++开发的控制台、2D和3D游戏演示项目，实现了完整的绘图系统、碰撞系统、光照和游戏玩法系统的游戏demo。

https://github.com/user-attachments/assets/55b77249-ff9f-400d-aef0-f191623fa360

### **特点**：

- 原生C++实现，尽可能减少第三方引擎的依赖
- 从XML文件中读取模型配置
- 动画系统数据化
- Lambert光照系统
- 高效的矩阵变换和数学运算库
- 模块化架构设计，易于扩展新游戏状态

[![GitHub stars](https://img.shields.io/github/stars/yourusername/game-engine-demo?style=social)](https://github.com/nekoqqq/MiniGame)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 系统简介

### 🚀 核心子系统

- **自定义3D渲染管线**：
  - 易用的绘制系统
  - 支持纹理贴图和UV映射
  - 法线贴图和Lambert光照系统
- **物理引擎**：
  - 三维、四维齐次矩阵线性变换计算库
  - 碰撞检测系统（三角形、球体、立方体）
  - 重力系统
  - 矩形、三角形、球面等几何体碰撞检测
- **资源管理系统**：
  - XML文件读取模块
  - 顶点/索引缓存/模型/纹理资源数据化加载
- **动画系统：**
  - XML文件配置化读取
  - 支持物体变换如缩放、平移、旋转等

### 🎮 游戏功能实现

- **角色控制系统**：
  - 角色移动和跳跃物理
  - 视角旋转控制
  - 导弹发射和跟踪系统
  - 目标自动锁定判定机制
- **AI行为系统**：
  - 敌人随机移动和攻击
- **游戏机制**：
  - 生命值(HP)系统
  - 倒计时和游戏状态管理
  - 小地图实现
- **动画系统**：
  - 爆炸冲击波（二维游戏）
  - 动画插值（线性、二次、三次）

### ⚙️ 性能优化

- **碰撞检测**
  - 人物和地面碰撞采用简化的线段和三角形位置判断
  - 物体之间采用球型包围盒判断

- **渲染优化**：
  - 光照模型法线计算本地化减少矩阵运算
  - z-buffer是否开启深度测试区分透明和不透明的物体绘制
  - 采用高效的数据结构Hash表，从模型文件中读取绘图类，复用顶点数据
- **内存优化**：
  - 动态内存管理
  - STL数据结构支持系统

## 系统架构

```mermaid
graph TD
    A[游戏引擎核心] --> B[渲染系统]
    A --> C[物理系统]
    A --> D[资源管理系统]
    A --> E[输入系统]
    
    B --> F[3D模型渲染]
    B --> G[纹理贴图]
    B --> H[光照计算]
    
    C --> J[碰撞检测]
    C --> K[运动模拟]
    C --> L[碰撞响应]
    
    D --> M[模型加载]
    D --> N[动画管理]
    D --> O[配置文件解析]
    
    E --> P[键盘输入]
    E --> Q[鼠标控制]
    
    A --> R[游戏逻辑]
    R --> S[角色控制]
    R --> T[AI系统]
    R --> U[游戏状态管理]
```

## 关键实现细节

### 绘制系统

```cpp
class Model{
    virtual void draw(const Matrix44 &pv, const Light* light)=0 {
		painter_->draw(pv, getModelTransform(), light);
	}
}	

class Painter{
    	void draw(const Matrix44& pv, const Matrix44& wm, const Light* light)const {
		vector<Vector3> res(vb_->size());
		vector<Vector3> world_coords(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			world_coords[i] = wm.vecMul(vb_->vertex(i));
		}
		for (int i = 0; i < vb_->size(); i++) {
			res[i] = pv.vecMul(world_coords[i]);
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
		vector<unsigned> colors(vb_->size());
		for (int i = 0; i < vb_->size(); i++) {
			Matrix44 wm_tmp = wm.dropRotation(); // 这里需要剔除掉法线向量平移的分量
			Vector3 transformed_norm = wm.vecMul(norms_[i]).normalize(); 
			colors[i] = light->calculate(transformed_norm,vb_->color(i));
		}
		for (int i = 0; i < ib_->size(); i++) {
			int i0 = (*ib_)[i][0], i1 = (*ib_)[i][1], i2 = (*ib_)[i][2];
			unsigned c0 = vb_->color(i0);
			unsigned c1 = vb_->color(i1);
			unsigned c2 = vb_->color(i2);
			Vector3 norm = (world_coords[i1] - world_coords[i0]).cross(world_coords[i2] - world_coords[i0]).normalize(); // 光照的计算使用世界坐标 
			
			// 注意这里不能用提前算好的法向量，因为wm矩阵有旋转成分
			c0=light->calculate(norm, c0);
			c1 = light->calculate(norm, c1);
			c2 = light->calculate(norm, c2);
			f.drawTriangle3DH(res[i0], res[i1], res[i2], vb_->uv(i0).data(), vb_->uv(i1).data(), vb_->uv(i2).data(), c0,c1,c2);
		}
	}
}

```

### 光照模型

```cpp
struct Light
{
	Light(const Vector3& light_dir, const Vector3& light_color,const Vector3&ambient)
	{
		this->light_dir = light_dir;
		this->light_color = light_color;
		this->ambient = ambient;
	}
	void updateLight(const Vector3&new_dir)
	{
		light_dir = new_dir;
	}
	unsigned calculate(const Vector3& norm, unsigned diffuse_color)const
	{
		Vector3 diffuse = { (diffuse_color >> 16 & 0xff)/255.0, ((diffuse_color >> 8) &0xff)/255.0, (diffuse_color & 0xff)/255.0 };
		// 三角形表面法向量, n
		// diffuse 物体本身的性质,漫反射率,反射RGB三种颜色的强度,0到1,1最大, R
		double c = light_dir.dot(norm)/light_dir.norm();
		c = max(0.0, c);
		Vector3 color = light_color.elementMul(diffuse) * c + ambient; // RGB
		unsigned r = max(0.0, min(1.0, color.x))*255.0;
		unsigned g = max(0.0, min(1.0, color.y))*255.0;
		unsigned b = max(0.0, min(1.0, color.z))*255.0;
		return (0xff << 24) | (r << 16) | (g << 8) | b;
	}
	// 基础光照模型, I = I0*R*(n.l)/ d^2 +a ，由于距离较远，可以将分母忽略
	// 这里为了复用Vector3这个类库，所以设置了归一化的RGB值
	Vector3 light_dir; // 光线的方向向量，由物体指向光源, l
	Vector3 light_color; // 光源的性质,RGB三种颜色的强度,0到1,1最强，表示光的强度, I0
	Vector3 ambient; // 环境光，各个颜色的分量强度其实就是一个unsigned, a
};
```

## 动画系统

- 实现基于关键帧进行插值的动画系统
- 支持三种插值算法：线性、二次和三次
- 从配置文件加载动画数据
- 变换树和动画树关联，用动画树管理不同状态间的过渡

```cpp
// 动画插值示例
class Curve{
double get(double time) const {
	time = fmod(time, datas_.back()->time); // 确保时间在循环范围内
	switch (interpolation_type_)
	{
	case Curve::NONE:
		{
			// 找到第一个大于等于time的点
			for (int i = 0; i < datas_.size(); i++) {
				if (datas_[i]->time >= time) {
					return datas_[i]->value;
				}
			}
			return datas_.back()->value; // 如果没有找到，返回最后一个点的值
		}
		break;
	case Curve::LINEAR:
		// 线性插值
		for (int i = 0; i < datas_.size() - 1; i++) {
			if (datas_[i]->time <= time && datas_[i + 1]->time >= time) {
				double t = (time - datas_[i]->time) / (datas_[i + 1]->time - datas_[i]->time);
				return datas_[i]->value + t * (datas_[i + 1]->value - datas_[i]->value);
			}
		}
		break;
	case Curve::CUBIC:
	{
		int beg = 0;
		int end = 0;
		for ( end = 0; end < datas_.size(); end++) {
			if (datas_[end]->time > time)
				break;
			beg = end;
		}
		double t0 = datas_[beg]->time;
		double t1 = datas_[end]->time;
		double p0 = datas_[beg]->value;
		double p1 = datas_[end]->value;
		double v0 = datas_[beg]->right_slope_;
		double v1 = datas_[end]->left_slope_;
		time = (time - t0) / (t1 - t0); // 归一化时间
		double a = 2.0 * (p0 - p1) + v0 + v1;
		double b = 3.0 * (p1 - p0) - (2.0 * v0) - v1;
		double r = a;
		r *= time;
		r += b;
		r *= time;
		r += v0;
		r *= time;
		r += p0;
		return r;
	}
		break;
	case Curve::HERMITE:
		break;
	default:
		break;
	}
}
}
```

## 导弹跟踪系统

- 实现一定程度自动跟踪目标
- 简化的碰撞检测

```cpp
class Missle{
    void updateVelocity(const Vector3&dir, double rotation_speed)
{
	// 旧的代码
		 //velocity_ = (velocity_ * 0.95 + dir * 0.05).normalize() ;
		 //rotateZ(MISSLE_ROTATION_SPEED); // 旧的代码，绕着自身的速度方向旋转

		// 可以按照先x再y的顺序，也可以按照先y再x的顺序，但是两次的角度是不一样的
	double y_rotation = atan2(dir.x, dir.z) * 180 / PI;
	double x_rotation = atan2(dir.y, sqrt((dir.x * dir.x + dir.z * dir.z))) * 180 / PI;
	double offset = 1.0;
	if (fabs(rotation.y - y_rotation) < offset) {
		rotation.y = y_rotation;
	}
	else if (rotation.y - y_rotation > 0.0) {
		rotation.y -= offset;
	}
	else {
		rotation.y += offset;
	}

	if (fabs(rotation.x - x_rotation) < offset) {
		rotation.x = x_rotation;
	}
	else if (rotation.x - x_rotation > 0.0) {
		rotation.x -= offset;
	}
	else {
		rotation.x += offset;
	}
	rotation.z += rotation_speed;
	setRotationY(rotation.y);
	rotateX(-rotation.x);
	rotateZ(rotation.z);
	velocity_ = getModelRotation().vecMul({ 0,0,0.5 });
}

```

## 物理碰撞检测

- 支持多种碰撞体类型（球体、AABB）
- 碰撞响应和物体反弹

```cpp
class Mecha{
    void collisionTest()
{
	// 存在一个方向，使得和其他所有物体都不相撞，才可以移动
	// 反之，存在一个物体，所有方向都和他相撞，则不可以移动
	// TODO 目前的处理存在抖动现象，相当于说每帧物体的移动方向都会发生改变，比如在爬很抖的坡的时候，一会儿向前，一会儿向后
	const Vector3 old_pos = getPos();
	if (getCollsionModel()->getType() == CollisionModel::Type::CUBOID) {
		vector<Vector3> possible_move_vectors = {
			velocity_,
			{0.0,velocity_.y,velocity_.z},
			{velocity_.x,0.0,velocity_.z},
			{velocity_.x,velocity_.y,0.0},
			{0.0,0.0,velocity_.z},
			{0.0,velocity_.y,0.0},
			{velocity_.x,0.0,0.0}
		};
		for (auto& v : possible_move_vectors) {
			updateCollisionPos(old_pos + v);
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
	}
	else if (getCollsionModel()->getType() == CollisionModel::Type::SPHERE) {
		Vector3 old_origin = getCollsionModel()->getOrigin();
		bool keep_origin = false;
		auto tri_loop_test = [&](Model* other_model) {
			const Stage& o = dynamic_cast<const Stage&> (*other_model);
			for (auto& tri : o.getTriangles()) {
				if (tri.isCollision(old_origin, velocity_)) {
					keep_origin = true;
					break;
				}
			}
			};

		for (auto& other_model : getCollisionModels()) {
			updateCollisionPos(old_origin + velocity_);
			if (other_model->getCollsionModel()->getType() == CollisionModel::SPHERE && isCollision(other_model)) {
				Vector3 t = other_model->getCollsionModel()->getOrigin() - old_origin;
				double s = 1 / t.squareDist();
				velocity_ -= t * (velocity_.dot(t)) * (1 / t.squareDist());
			}
			else if (other_model->getCollsionModel()->getType() == CollisionModel::TRIANGLE) { // 碰撞检测的部分可以继续优化，这部分写的不太优雅
				const Stage& o = dynamic_cast<const Stage&> (*other_model);
				for (auto& tri : o.getTriangles()) {
					if (tri.isCollision(old_origin, velocity_)) {
						Vector3 n = tri.getNorm();
						double lambda = n.dot(velocity_) / n.dot(n);
						velocity_ -= n * lambda;
					}
				}
				// (*) 三角形是数组，因此要循环，
				// 为了避免间隙处的穿透问题，使用两次循环，第一次循环如果没有发生碰撞，则直接使用就可以
				// 如果发生了碰撞，使用校正后的向量进行第二次循环，因此本次是不会和之前已经碰撞修复过的再碰撞，如果还是发生了碰撞则不可以使用这次的移动，否则会穿透之前的物体
				tri_loop_test(other_model);
			}
		}
		// 同注释（*），对多个物体循环两次
		for (auto& other_model : getCollisionModels()) {
			if (other_model->getCollsionModel()->getType() == CollisionModel::TRIANGLE) {
				tri_loop_test(other_model);
			}
		}
		if (keep_origin) {
			updateCollisionPos(old_origin); // 之前的bug是由与没有同步更新这个向量导致
		}
		else {
			updateCollisionPos(old_origin + velocity_);
			setPos(old_pos + velocity_);
		}
	}
}
    
}

```

##  如何运行

### 系统要求

- Windows 10
- Visual Studio 2022

### 构建步骤

1. 克隆仓库：

   ```bash
   git clone https://github.com/nekoqqq/MiniGame
   ```

2. 打开解决方案文件`MiniGame.sln`

3. 设置`3D`为启动项目

4. 构建并调试（F5）或者运行(Ctrl+F5)

### 控制说明

- **WASD**：角色移动
- **空格**：跳跃+锁定敌人
- J：发射跟踪导弹
- **U**：视角左移
- **I**：视角右移
- **C**：锁定敌人
- **Q**：退出游戏

## 未来计划

- 实现自己的Shader
- 画面优化，场景中引入更多的元素
- 添加更加高级的游戏照明系统，如*Blinn-Phong*模型
- 优化的导弹追踪系统，现在导弹系统有一定几率会原地的盘旋
- 场景优化，增加复杂植被和天空
- 碰撞系统优化，采用BVH类似的方法实现高效地碰撞检测
- 完善游戏UI
- 资源读取优化

## 许可证

本项目采用MIT许可证 - 详情请参阅[LICENSE](https://license/)文件。
