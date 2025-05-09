#include "DDS.h"
#include "GameLib/Framework.h"
#include<iostream>
#include<fstream>
#include <cmath>
#include "Vector2.h"
using GameLib::Framework;
int least_power2(int x)
{
    if ((x & (x - 1)) == 0)
        return x;
    unsigned int_max = 0x80000000;
    while ((int_max & x) == 0)
        int_max = int_max >> 1;
    return int_max << 1;
}
DDS::DDS(const char* file_name) {
    std::ifstream file(file_name, std::ios_base::binary);
    if (!file.is_open()) {
        std::cout << file_name << " " << "文件打开失败" << std::endl;
        file.close();
        throw std::exception();
    }
    // 读取文件元信息
    file.seekg(sizeof(dMagic) + sizeof(dSize) + sizeof(dFlags), std::ios_base::beg); //跳过前面的字节
    auto read_dword = [&file]() mutable {
        char s[sizeof(DWORD)];
        file.read(s, sizeof(DWORD));
        unsigned res = 0;
        for (int i = 0; i < sizeof(DWORD); i++)
            res |= static_cast<unsigned char>(s[i]) << (i * 8);

        return res;
        }; // 读取unsigned
    dHeight = read_dword();
    dWidth = read_dword();
    dTextureHeight = least_power2(dHeight);
    dTextureWidth = least_power2(dWidth);

    std::cout << file_name << " " << "读取的文件大小为: " << dHeight << "x" << dWidth << std::endl;
    std::cout << file_name << " " << "读取的纹理大小为: " << dTextureHeight << "x" << dTextureWidth << std::endl;

    // 读取图片数据
    file.seekg(sizeof(dMagic) + dSize, std::ios_base::beg);
    dData = new DWORD[dWidth * dHeight];
    for (int i = 0; i < dWidth * dHeight; i++)
        dData[i] = read_dword();

    // 创建纹理
    texture = nullptr;
    Framework::instance().createTexture(&texture, dTextureWidth, dTextureHeight, dData, dWidth, dHeight);
    SAFE_DELETE(dData);
    file.close();
}
DDS::DWORD* DDS::get_image_data() const
{
    return dData;
}
DDS::DWORD DDS::get_image_width() const
{
    return dWidth;
}
DDS::DWORD DDS::get_image_height() const
{
    return dHeight;
}
DDS::DWORD& DDS::operator[](int i)
{ // 允许修改元素    
    return dData[i];
}
DDS::~DDS() {
    if (dData) {
        delete[] dData;
        dData = nullptr;
    }
    GameLib::Framework::instance().destroyTexture(&texture);
}
unsigned DDS::alpha_mix(unsigned fg_color, unsigned bg_color) const{
    unsigned fg_color_A =(fg_color & 0xff000000) >> 24;
    unsigned fg_color_R = (fg_color & 0x00ff0000);
    unsigned fg_color_G = (fg_color & 0x0000ff00);
    unsigned fg_color_B = (fg_color & 0x000000ff);

    unsigned bg_color_R = (bg_color & 0x00ff0000);
    unsigned bg_color_G = (bg_color & 0x0000ff00);
    unsigned bg_color_B = (bg_color & 0x000000ff);

    unsigned r = bg_color_R + fg_color_A / 255.f * (fg_color_R - bg_color_R);
    unsigned g = bg_color_G + fg_color_A / 255.f * (fg_color_G - bg_color_G);
    unsigned b = bg_color_B + fg_color_A / 255.f * (fg_color_B - bg_color_B);
    return b | (g & 0x00ff00) | (r & 0xff0000);
}
// 待废弃的API
void DDS::drawCell(int src_x,int src_y) const{ // 从src_x，src_y开始的位置绘制当前图片
    unsigned* p_vram = new unsigned[dWidth * dHeight];
    int window_width = GameLib::Framework::instance().width();
    unsigned* p_img = get_image_data();
    int img_width = get_image_width();
    int img_height = get_image_height();
    // TODO 这里的混合有问题，最终总是画面偏黄, 已经修复，是因为读取图片的API有问题
    // 线性混合,z = a*x+(1-a)*y = y + a*(x-y) 

    for (int i = 0; i < img_height; i++)
        for (int j = 0; j < img_width; j++) {
            int src_index = (src_x + i) * window_width + src_y + j;
            int dst_index = i * img_width + j;
            unsigned bg_color = p_vram[src_index];
            unsigned fg_color = p_img[dst_index];
            // 颜色混合的API实现没有问题，但是现在的图片没有做处理，导致移动的时候有些问题
            unsigned mix_color = alpha_mix(fg_color, bg_color);
            p_vram[src_index] = mix_color;
        }
}
void DDS::drawImage() const
{
    render(0, 0, dWidth, dHeight, 0, 0);
}
// 这里的x、y的坐标和类库的坐标现在是反着来的
void DDS::render(int src_x, int src_y, int width, int height, int screen_x, int screen_y,unsigned font_color)const {    
    Vector2 p0(screen_y, screen_x);
    Vector2 p1(screen_y + width, screen_x);
    Vector2 p2(screen_y, screen_x + height);
    Vector2 p3(screen_y + width, screen_x + height);

    // 纹理坐标
    double u0 = 1.0 * src_x / dTextureHeight;
    double v0 = 1.0 * src_y / dTextureWidth;
    double u1 = 1.0 * (src_x + height) / dTextureHeight;
    double v1 = 1.0 * (src_y + width) / dTextureWidth;

    Vector2 t0(v0, u0);
    Vector2 t1(v1,u0);
    Vector2 t2(v0,u1);
    Vector2 t3(v1,u1);
    Framework f = Framework::instance();
    
    f.setTexture(texture);
    f.setBlendMode(Framework::BLEND_LINEAR);
    f.drawTriangle2D(p0,p1,p2,t0,t1,t2);
    f.drawTriangle2D(p3,p1,p2,t3,t1,t2);
}

void DDS::drawAtScreen(int i, int j, int screen_x, int screen_y,int pixel_size) {
    render( i * pixel_size, j * pixel_size, pixel_size,pixel_size, screen_x, screen_y);
}