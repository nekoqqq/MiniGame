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
void DDS::drawCell(int src_x,int src_y) const{
    unsigned* p_vram = GameLib::Framework::instance().videoMemory();
    int window_width = GameLib::Framework::instance().width();
    unsigned* p_img = get_image_data();
    int img_width = get_image_width();
    int img_height = get_image_height();
    // TODO 这里的混合有问题，最终总是画面偏黄, 已经修复，是因为读取图片的API有问题
    // 线性混合,z = a*x+(1-a)*y = y + a*(x-y) 
    auto alpha_mix = [&](unsigned src_data, unsigned dst_data) {
        unsigned src_data_A = (src_data & 0xff000000) >> 24;
        unsigned src_data_R = (src_data & 0x00ff0000);
        unsigned src_data_G = (src_data & 0x0000ff00);
        unsigned src_data_B = (src_data & 0x000000ff);

        unsigned dst_data_A = (dst_data && 0xff000000) >> 24;
        unsigned dst_data_R = (dst_data & 0x00ff0000);
        unsigned dst_data_G = (dst_data & 0x0000ff00);
        unsigned dst_data_B = (dst_data & 0x000000ff);

        unsigned r = dst_data_R + dst_data_A / 255.f * (src_data_R - dst_data_R);
        unsigned g = dst_data_G + dst_data_A / 255.f * (src_data_G - dst_data_G);
        unsigned b = dst_data_B + dst_data_A / 255.f * (src_data_B - dst_data_B);
        return b | (g & 0x00ff00) | (r & 0xff0000);
        };

    for (int i = 0; i < img_height; i++)
        for (int j = 0; j < img_width; j++) {
            int src_index = (src_x + i) * window_width + src_y + j;
            int dst_index = i * img_width + j;
            unsigned src_data = p_vram[src_index];
            unsigned dst_data = p_img[dst_index];
            unsigned mix_data = alpha_mix(src_data, dst_data);
            p_vram[src_index] = mix_data;
        }
}
void DDS::drawImage() const
{
    drawCell(0, 0);
}
