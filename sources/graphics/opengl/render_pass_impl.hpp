#ifndef STRAITX_OPENGL_RENDER_PASS_IMPL_HPP
#define STRAITX_OPENGL_RENDER_PASS_IMPL_HPP

#include "core/push_array.hpp"
#include "graphics/render_pass.hpp"

namespace GL{

//OpenGL Spec guaranties to support at least 8 color attachments
constexpr size_t MaxColorAttachments = 8;

struct RenderPassImpl: RenderPass{
    RenderPassImpl(const RenderPassProperties &props);

    virtual ~RenderPassImpl()override;

    static RenderPass *NewImpl(const RenderPassProperties &props);

    static void DeleteImpl(RenderPass *pass);
};

}//namespace GL::

#endif//STRAITX_OPENGL_RENDER_PASS_IMPL_HPP