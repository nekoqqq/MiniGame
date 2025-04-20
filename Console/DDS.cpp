#include "DDS.h"
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
DDS::DWORD* DDS::get_image_data()
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