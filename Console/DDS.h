#pragma once
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

    unsigned alpha_mix(unsigned fg_color, unsigned bg_color) const;

    DWORD* get_image_data()const;
    DWORD get_image_width()const;
    DWORD get_image_height()const;
    void drawCell(int,int)const;
    void drawImage()const;
    void render(int src_x1,int src_y1,int src_x2,int src_y2, int dst_x,int dst_y)const; // 通用的绘图函数, [src_x1, src_y1] x [src_x2, src_y2]为原始的尺寸, dst_x，dst_y为目标的位置

    void edit();

    DWORD& operator[](int i);
};