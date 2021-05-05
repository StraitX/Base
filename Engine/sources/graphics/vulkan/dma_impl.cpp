#include "platform/memory.hpp"
#include "graphics/vulkan/dma_impl.hpp"
#include "graphics/vulkan/fence.hpp"
#include "graphics/vulkan/gpu.hpp"
#include "graphics/vulkan/gpu_texture_impl.hpp"

namespace StraitX{
namespace Vk{

u8 DMAImpl::s_Instance[sizeof(DMAImpl)];

void DMAImpl::Initialize(){
    new(s_Instance) DMAImpl();
}

void DMAImpl::Finalize(){
    DMAImpl::Get().~DMAImpl();
}

void DMAImpl::Copy(const CPUBuffer &src, const GPUBuffer &dst, u32 size, u32 src_offset, u32 dst_offset){
    if(GPU::Get().IsMappable(ToVkMemoryType(dst.MemoryType()))){
        auto memory = reinterpret_cast<VkDeviceMemory>(dst.Memory().U64);

        void *pointer;
        vkMapMemory(GPU::Get().Handle(), memory, 0, dst.Size(), 0, &pointer);
        {
            Memory::Copy((u8*)src.Pointer() + src_offset, (u8*)pointer + dst_offset, size);
        }
        vkUnmapMemory(GPU::Get().Handle(), memory);
    }else{
        m_CmdBuffer.Begin();
        {
            m_CmdBuffer.CmdBufferCopy(
                (VkBuffer)src.Handle().U64, 
                (VkBuffer)dst.Handle().U64, 
                size, 
                src_offset, 
                dst_offset
            );
        }
        m_CmdBuffer.End();

        m_CmdBuffer.Submit({nullptr, 0}, {nullptr, 0}, m_OpFence.Handle);
        m_OpFence.WaitFor();
    }
}

void DMAImpl::Copy(const CPUTexture &src, const GPUTexture &dst){
    m_CmdBuffer.Begin();
    {
        VkBufferImageCopy copy;
        copy.bufferImageHeight = src.Size().y;
        copy.bufferOffset = 0;
        copy.bufferRowLength = src.Size().x;
        copy.imageOffset = {};
        copy.imageExtent.depth = 1;
        copy.imageExtent.width = dst.Size().x;
        copy.imageExtent.height = dst.Size().y;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;
        copy.imageSubresource.mipLevel = 0;
        vkCmdCopyBufferToImage(m_CmdBuffer, (VkBuffer)src.Handle().U64, (VkImage)dst.Handle().U64, GPUTextureImpl::s_LayoutTable[(size_t)dst.GetLayout()], 1, &copy);
    }
    m_CmdBuffer.End();

    m_CmdBuffer.Submit({nullptr, 0}, {nullptr, 0}, m_OpFence.Handle);
    m_OpFence.WaitFor();
}

void DMAImpl::ChangeLayout(GPUTexture &texture, GPUTexture::Layout layout){

    m_CmdBuffer.Begin();
    {
        m_CmdBuffer.CmdImageBarrier(
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
            VK_ACCESS_MEMORY_WRITE_BIT, 
            VK_ACCESS_MEMORY_READ_BIT, 
            GPUTextureImpl::s_LayoutTable[(size_t)texture.GetLayout()], 
            GPUTextureImpl::s_LayoutTable[(size_t)layout], 
            (VkImage)texture.Handle().U64,
            (IsColorFormat(texture.GetFormat()) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT)
        );
    }
    m_CmdBuffer.End();

    m_CmdBuffer.Submit({nullptr, 0}, {nullptr, 0}, m_OpFence.Handle);
    m_OpFence.WaitFor();

    texture.m_Layout = layout;
}


void DMAImpl::CopyCPU2GPUBufferImpl(const CPUBuffer &src, const GPUBuffer &dst, u32 size, u32 src_offset, u32 dst_offset){
    DMAImpl::Get().Copy(src, dst, size, src_offset, dst_offset);
}

void DMAImpl::CopyCPU2GPUTextureImpl(const CPUTexture &src, const GPUTexture &dst){
    DMAImpl::Get().Copy(src, dst);
}

void DMAImpl::ChangeGPUTextureLayoutImpl(GPUTexture &src, GPUTexture::Layout layout){
    DMAImpl::Get().ChangeLayout(src, layout);
}

}//namespace Vk::
}//namespace StraitX::