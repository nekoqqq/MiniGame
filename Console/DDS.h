#pragma once
namespace GameLib {
    class Texture;
}
struct DDS { // DirectX格式图片
    using DWORD = unsigned;
    using BYTE = unsigned char;

    const char dMagic[4] = { 'D','D','S',' ' }; // 文件头，固定为"DDS "
    DWORD dSize = 124; // 结构体大小，固定为124字节
    DWORD dFlags; // 标志位信息
    DWORD dHeight; // 图片高度，位于12开始的4字节
    DWORD dWidth; // 图片宽度，位于16开始的4字节
    DWORD dPitchOrLinearSize; // 一行的字节数
    DWORD dDepth; // 3D纹理深度
    DWORD dMipMapCount; // MipMap的数量
    DWORD dReserved1[11]; // 其他信息
    DWORD dReserved2; // 未使用信息
    DWORD* dData; // 图片数据


    DDS(const char* file_name);
    ~DDS();
    DWORD& operator[](int i);

    unsigned alpha_mix(unsigned, unsigned) const;
    DWORD* get_image_data()const;
    DWORD get_image_width()const;
    DWORD get_image_height()const;
    // 绘制图片中的某个单元格
    void drawCell(int,int)const;
    // 绘制整个图片
    void drawImage()const;

    // 绘制某个单元格到指定的位置
    void drawAtScreen(int i, int j, int screen_x, int screen_y, int pixel_size= 16);

    void render(int, int, int, int, int, int, unsigned = 0xff0000)const; // 通用的绘图函数, [src_x1, src_y1] x [src_x2, src_y2]为原始的尺寸, dst_x，dst_y为目标的位置, color为需要绘制的颜色
    
    // 纹理数据
    GameLib::Texture* texture;
    int dTextureHeight;
    int dTextureWidth;
};
