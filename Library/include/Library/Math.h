#pragma once
#include <cmath>
#include <array>
#include <utility>
extern const double eps;
using std::array;
class Vector3 {
public:
	double x, y, z;
	double w;
	Vector3():Vector3(0.0,0.0,0.0,1.0){}
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
	Vector3& operator-=(const Vector3& other) {
		*this += -other;
		return *this;
	}
	Vector3 operator+(double t) {
		return Vector3(x + t, y + t, z + t);
	}
	Vector3 operator-(double t) {
		return *this + (-t);
	}
	Vector3 operator+(const Vector3& o) const{
		if (this == &o)
			return *this;
		return Vector3(x + o.x, y + o.y, z + o.z);
	}
	Vector3 operator-(const Vector3& o)const {
		return *this + (-o);
	}
	Vector3 operator*(double t)const {
		return Vector3(x * t, y * t, z * t);
	}
	Vector3& operator*=(double t) {
		this->x *= t;
		this->y *= t;
		this->z *= t;
		return *this;
	}
	Vector3 operator/(double t) {
		assert(fabs(t) > eps);
		return Vector3(x / t, y / t, z / t);
	}

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
	double squareDist(const Vector3& b = { 0,0,0 })const {
		Vector3 tmp = *this - b;
		return tmp.x* tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
	}

	Vector3 cross(const Vector3& b)const {
		return {
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x
		};
	}
	double norm()const {
		return pow(squareDist(),0.5);
	}
	Vector3& normalize() { // 这个接口的设计不好
		double r = pow((x * x + y * y + z * z), 0.5);
		x /= r;
		y /= r;
		z /= r;
		return *this;
	}
	double dot(const Vector3& o) const{
		return x * o.x + y * o.y + z * o.z;
	}

	friend Vector3 setAdd(const Vector3& a, const Vector3& b) {
		return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	friend Vector3 setSub(const Vector3& a, const Vector3& b) {
		return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	operator array<double, 3>();
};

class Matrix44 { // 4阶齐次矩阵
public:
	double p[4][4];
	static const int N = 4;
	static const Matrix44 &identity() {
		static Matrix44 m;
		m.p[0][0] = m.p[1][1] = m.p[2][2] = m.p[3][3] = 1;
		return m;
	}
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
	Matrix44 transpose() {
		Matrix44 res;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				res[i][j] = p[j][i];
			}
		}
		return res;
	}
	const double* operator[](int row)const {
		return p[row];
	}
	double* operator[](int row) {
		return p[row];
	}
	Vector3 vecMul(const Vector3& o)const {
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
	Matrix44 matMul(const Matrix44& o) const {
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
};
