#include "DDS.h"
#include "GameLib/Framework.h"
#include<iostream>
#include<fstream>

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
    std::cout << file_name << " " << "读取的文件大小为: " << dHeight << "x" << dWidth << std::endl;
    // 读取图片数据
    file.seekg(sizeof(dMagic) + dSize, std::ios_base::beg);
    dData = new DWORD[dWidth * dHeight];
    for (int i = 0; i < dWidth * dHeight; i++)
        dData[i] = read_dword();
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
void DDS::drawCell(int src_x,int src_y) const{ // 从src_x，src_y开始的位置绘制当前图片
    unsigned* p_vram = GameLib::Framework::instance().videoMemory();
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
    drawCell(0, 0);
}
void DDS::render(int src_x, int src_y, int width, int height, int screen_x, int screen_y,unsigned font_color)const {
    unsigned* p_vram = GameLib::Framework::instance().videoMemory();
    int window_width = GameLib::Framework::instance().width();
    for(int i = 0;i<height;i++)
        for (int j = 0; j < width; j++) {
            int src_index = (screen_x + i) * window_width + (screen_y + j);
            int dst_index = (src_x + i) * dWidth + (src_y + j);
            int fg_color = dData[dst_index];
            if (dData[dst_index] > 0xffff0000) // 原本的不是严格的黑色和白色画成的图片，这里做下近似处理
                fg_color = 0;
            else
                fg_color = font_color | (0xff <<24); // 该颜色做不透明处理
            p_vram[src_index] = alpha_mix(fg_color, p_vram[src_index]);
        }
}

void DDS::drawFrom(int i, int j, int screen_i,int screen_j, int pixel_size) const
{
    unsigned* p_vram = GameLib::Framework::instance().videoMemory();
    int window_width = GameLib::Framework::instance().width();
    for (int x = 0; x < pixel_size; x++) 
        for (int y = 0; y < pixel_size; y++) {
            int src_index = (screen_i *pixel_size+x)* window_width + screen_j *pixel_size + y;
            int dst_index = i * pixel_size * dWidth +x*dWidth +  j * pixel_size + y;
            int fg_color = dData[dst_index];
            p_vram[src_index] = alpha_mix(fg_color, p_vram[src_index]);
        }
}


void DDS::drawAtScreen(int i, int j, int screen_x, int screen_y,int pixel_size) {
    unsigned* p_vram = GameLib::Framework::instance().videoMemory();
    int window_width = GameLib::Framework::instance().width();
    for (int x = 0; x < pixel_size; x++)
        for (int y = 0; y < pixel_size; y++) {
            int src_index = (screen_x +  x) * window_width + screen_y + y;
            int dst_index = i * pixel_size * dWidth + x * dWidth + j * pixel_size + y;
            int fg_color = dData[dst_index];
            p_vram[src_index] = alpha_mix(fg_color, p_vram[src_index]);
        }


}