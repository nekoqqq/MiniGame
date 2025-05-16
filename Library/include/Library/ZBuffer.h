#pragma once
#include <cassert>
#include <limits>
class ZBuffer {
public:
	ZBuffer(int width,int height):width_(width),height_(height)
	{
		p_ = new float[width * height];
	}
	~ZBuffer()
	{
		delete[]p_;
		p_ = nullptr;
	}
	// 在每帧的开始进行操作
	void clear() {
		const int n = width_ * height_;
		for (int i = 0; i < n; i++) {
			p_[i] = std::numeric_limits<float>::infinity();
		}
	}
	float& At(int x, int y) {
		assert(0 <= x && x < width_);
		assert(0 <= y && y < height_);
		return p_[y * width_ + x];
	}
	const float& At(int x, int y)const {
		return const_cast<ZBuffer*>(this)->At(x, y);
	}
	bool testAndSet(int x, int y, int depth) {
		float& prev = At(x, y);
		if (depth < prev) {
			prev = depth;
			return true;
		}
		return false;
	}

private:
	float* p_;
	int width_;
	int height_;
	ZBuffer(const ZBuffer&) = delete;
	ZBuffer& operator=(const ZBuffer&) = delete;
};