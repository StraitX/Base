#ifndef STRAITX_COMMAND_BUFFER_HPP
#define STRAITX_COMMAND_BUFFER_HPP

#include "core/noncopyable.hpp"
#include "graphics/color.hpp"
#include "graphics/api/texture.hpp"

class RenderPass;
class Framebuffer;
class GraphicsPipeline;
class Buffer;
class DescriptorSet;

class CommandBuffer;

class CommandPool: public NonCopyable{
public:
    CommandPool() = default;

    virtual ~CommandPool() = default;

    virtual CommandBuffer *Alloc() = 0;

    virtual void Free(CommandBuffer *buffer) = 0;

    //virtual void Reset() = 0

    static CommandPool *Create();
};

enum class IndicesType{
    Uint16,
    Uint32
};

class CommandBuffer: public NonCopyable{
public:
    CommandBuffer() = default;

    virtual ~CommandBuffer() = default;

    virtual void Begin() = 0;

    virtual void End() = 0;

    virtual void Reset() = 0;

    virtual void ChangeLayout(Texture2D *texture, TextureLayout new_layout) = 0;

    virtual void BeginRenderPass(const RenderPass *rp, const Framebuffer *fb) = 0;

    virtual void EndRenderPass() = 0;

    virtual void Bind(const GraphicsPipeline *pipeline) = 0;

    virtual void Draw(u32 vertices_count) = 0;

    virtual void SetScissor(s32 x, s32 y, u32 width, u32 height) = 0;

    virtual void SetViewport(s32 x, s32 y, u32 width, u32 height) = 0;

    virtual void BindVertexBuffer(const Buffer *buffer) = 0;

    virtual void BindIndexBuffer(const Buffer *buffer, IndicesType type) = 0;

    virtual void DrawIndexed(u32 indices_count, u32 indices_offset = 0) = 0;

    virtual void Copy(const Buffer *src, const Buffer *dst, size_t size, size_t src_offset = 0, size_t dst_offset = 0) = 0;

    void Copy(const Buffer *src, const Buffer *dst);

    //during operations texture it is implicitly translated into transferDstOptimal layout and back
    //if texture layout is undefined, back translation won't happen
    //buffer should contain data in the same format as dst texture
    virtual void Copy(const Buffer *src, Texture2D *dst, Vector2u src_size, Vector2u dst_offset = {0, 0}) = 0;

    void Copy(const Buffer *src, Texture2D *dst){
        Copy(src, dst, dst->Size(), {0, 0});
    }

    virtual void ClearColor(Texture2D *texture, const Color &color) = 0; 

    virtual void Bind(const DescriptorSet *set) = 0;

};

#endif//STRAITX_COMMAND_BUFFER_HPP