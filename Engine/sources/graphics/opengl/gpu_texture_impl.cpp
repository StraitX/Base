#include "platform/opengl.hpp"
#include "graphics/opengl/debug.hpp"
#include "graphics/opengl/gpu_texture_impl.hpp"


namespace StraitX{
namespace GL{

GLint InternalFormatTable[]={
    0,
    GL_RGBA8,
    GL_DEPTH_STENCIL
};

GLenum FormatTable[] = {
    0,
    GL_RGBA,
    GL_DEPTH_STENCIL
};

GLenum TypeTable[] = {
    0,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE
};

void GPUTextureImpl::NewImpl(GPUTexture &texture, GPUTexture::Format format, GPUTexture::Usage usage, u32 width, u32 height){
    CoreAssert(format != GPUTexture::Format::Unknown,"GPUTexture: Can't be created with Format::Unknown");

    texture.m_Width = width;
    texture.m_Height = height;
    texture.m_Layout = GPUTexture::Layout::Undefined;
    texture.m_Format = format;
    texture.m_Usage = usage;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture.m_Handle.U32);
    glBindTexture(GL_TEXTURE_2D, texture.m_Handle.U32);
    GL(glTexImage2D(GL_TEXTURE_2D, 0, InternalFormatTable[(u32)format], texture.m_Width, texture.m_Height, 0, FormatTable[(u32)format], TypeTable[(u32)format], nullptr));
}

void GPUTextureImpl::DeleteImpl(GPUTexture &texture){
    glDeleteTextures(1, &texture.m_Handle.U32);
}

}//namespace GL::
}//namespace StraitX::