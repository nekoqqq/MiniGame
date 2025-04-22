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

    DWORD* get_image_data()const;
    DWORD get_image_width()const;
    DWORD get_image_height()const;
    void drawCell(int,int)const;
    void drawImage()const;

    DWORD& operator[](int i);
};