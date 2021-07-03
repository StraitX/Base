#ifndef STRAITX_CPU_TEXTURE_HPP
#define STRAITX_CPU_TEXTURE_HPP

#include "core/math/vector2.hpp"
#include "graphics/api/cpu_buffer.hpp"
#include "graphics/api/format.hpp"

class CPUTexture: private CPUBuffer{
private:
    u32 m_Width = 0;
    u32 m_Height = 0;
    TextureFormat m_Format = {};
public:
    CPUTexture() = default;

    CPUTexture(u32 width, u32 height, TextureFormat format);

    CPUTexture(u32 width, u32 height, TextureFormat format, const void *pixels);

    void New(u32 width, u32 height, TextureFormat format);

    void New(u32 width, u32 height, TextureFormat format, const void *pixels);

    void Delete();

    void *Pointer()const;

    void CopyData(const void *source);

    GPUResourceHandle Handle()const;

    Vector2u Size()const;

    TextureFormat Format()const;
};

SX_INLINE CPUTexture::CPUTexture(u32 width, u32 height, TextureFormat format){
    New(width, height, format);
}

SX_INLINE CPUTexture::CPUTexture(u32 width, u32 height, TextureFormat format, const void *pixels){
    New(width, height, format, pixels);
}

SX_INLINE void CPUTexture::New(u32 width, u32 height, TextureFormat format){
    CPUBuffer::New(width * height * GetPixelSize(format));
    m_Format = format;
    m_Width = width;
    m_Height = height;
}

SX_INLINE void CPUTexture::New(u32 width, u32 height, TextureFormat format, const void *pixels){
    New(width, height, format);
    CopyData(pixels);
}

SX_INLINE void CPUTexture::Delete(){
    CPUBuffer::Delete();
}

SX_INLINE void *CPUTexture::Pointer()const{
    return CPUBuffer::Pointer();
}

SX_INLINE void CPUTexture::CopyData(const void *source){
    CPUBuffer::CopyData(source, m_Width * m_Height * GetPixelSize(m_Format));
}

SX_INLINE GPUResourceHandle CPUTexture::Handle()const{
    return CPUBuffer::Handle();
}

SX_INLINE Vector2u CPUTexture::Size()const{
    return {m_Width, m_Height};
}

SX_INLINE TextureFormat CPUTexture::Format()const{
    return m_Format;
}

#endif//STRAITX_CPU_TEXTURE_HPP