#pragma once
#include <cmath>
#include <utility>
class Vector3 {
public:
	double x, y, z;
	double w;

	Vector3():Vector3(0,0,0,0){}

	Vector3(double x,double y, double z,double w=1):x(x),y(y),z(z),w(w){}

	Vector3(const Vector3& other) = default;

	Vector3& operator=(const Vector3& other) {
		if (this == &other)
			return *this;
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;	
		this->w = other.w;
		return *this;
	}
	
	void set(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3 operator-() const{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator-(const Vector3& other) {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
		
	//Vector3& operator-=(const Vector3& other) {
	//	Vector3 tmp = -other;
	//	*this += -other;
	//	return *this;
	//}

	Vector3 operator+(int t) {
		return Vector3(x + t, y + t, z + t);
	}

	Vector3 operator-(int t) {
		return *this + (-t);
	}

	Vector3 operator+(const Vector3& o) {
		if (this == &o)
			return *this;
		return Vector3(x + o.x, y + o.y, z + o.z);
	}
	
	//Vector3 operator-(const Vector3& o) {
	//	Vector3 tmp = -o;
	//	return this + tmp;
	//}

	// 只读
	double operator[](int i)const{
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else if (i == 2)
			return z;
		else
			return w;
	}

	// 修改
	 double& operator[](int i) {
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else if (i == 2)
			return z;
		else
			return w;
	}

	operator double* () {
		return &this->x;
	}

	Vector3 cross(const Vector3& b) {
		return {
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x
		};
	}

	Vector3& normalize() {
		double r = pow((x * x + y * y + z * z), 0.5);
		x /= r;
		y /= r;
		z /= r;
		return *this;
	}


	friend Vector3 setAdd(const Vector3& a, const Vector3& b) {
		return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	friend Vector3 setSub(const Vector3& a, const Vector3& b) {
		return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}
};


class Matrix44 { // 4阶齐次矩阵
public:
	double p[4][4];
	static const int N = 4;
	Matrix44()
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				p[i][j] = 0.f;
	}
	Matrix44(const double raw[][4]) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				p[i][j] = raw[i][j];
			}
		}
	}

	Matrix44(const double raw[]) {
		for (int i = 0; i < N * N; i++)
			p[i / 4][i % 4] = raw[i];
	}

	~Matrix44() = default;
	const double* operator[](int row)const {
		return p[row];
	}
	double* operator[](int row) {
		return p[row];
	}

	Vector3 vecMul(const Vector3& o) {
		Vector3 res;
		for (int i = 0; i < N; i++) {
			double cur = 0;
			for (int j = 0; j < N; j++) {
				cur += p[i][j] * o[j];
			}
			res[i] = cur;
		}
		return res;
	}


	Matrix44 matMul(const Matrix44& o) {
		if (this == &o)
			return *this;
		Matrix44 res;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++) {
				double tmp = 0;
				for (int k = 0; k < N; k++)
					tmp += p[i][k] * o[k][j];
				res[i][j] = tmp; 
			}
		return res;
	}


	// 视图变化
	void setView(Vector3 &eye_pos, Vector3 & target_pos, Vector3&up) {
		Vector3 e3 = (target_pos - eye_pos).normalize();
		Vector3 e1 = up.cross(e3).normalize();
		Vector3 e2 = e3.cross(e1).normalize();

		// 过渡矩阵需要取逆，并且由于是标准正交基之间的变换，可以直接取转置
		const double rot_t[][4] = {
			{ e1.x,e1.y,e1.z,0 },
			{e2.x,e2.y,e2.z,0},
			{e3.x,e3.y,e3.z,0},
			{0,0,0,1}
		};
		Matrix44 rotation(rot_t);
		
		const double trans_t[][4] = {
			{1.,0.,0.,-eye_pos.x },
			{0.,1.,0.,-eye_pos.y },
			{ 0.,0.,1.,-eye_pos.z},
			{0,0,0,1}
		};
		Matrix44 trans(trans_t); 
		*this = rotation.matMul(trans);
	}

	// 投影变换
	void setProjection(double fov_y,double aspect_ratio,double near,double far){
		// 现在要进行左右手坐标系的变换了，从右手系变成左手系，z要变成-z
		p[1][1] = 1 / tan(fov_y * 0.5);
		p[0][0] = p[1][1] / aspect_ratio ;
		p[2][2] = far / (far-near);
		p[2][3] = -near * far / (far - near);
		p[3][2] = 1;
	}

};
