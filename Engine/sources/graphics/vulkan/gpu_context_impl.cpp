#include <new>
#include "platform/memory.hpp"
#include "platform/vulkan.hpp"
#include "core/assert.hpp"
#include "graphics/vulkan/gpu_context_impl.hpp"
#include "graphics/vulkan/cpu_buffer_impl.hpp"
#include "graphics/vulkan/gpu_buffer_impl.hpp"
#include "graphics/vulkan/gpu_texture_impl.hpp"
#include "graphics/vulkan/graphics_pipeline_impl.hpp"
#include "graphics/vulkan/swapchain_impl.hpp"

namespace StraitX{
namespace Vk{
VkIndexType GPUContextImpl::s_IndexTypeTable[]={
    VK_INDEX_TYPE_UINT16,
    VK_INDEX_TYPE_UINT32
};

constexpr size_t GPUContextImpl::SemaphoreRingSize;

GPUContextImpl::GPUContextImpl(Vk::LogicalGPUImpl *owner):
    m_Owner(owner),
    m_SemaphoreRing{m_Owner, m_Owner}
{
    VkCommandPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = m_Owner->GraphicsQueue.FamilyIndex;

    CoreFunctionAssert(vkCreateCommandPool(m_Owner->Handle, &pool_info, nullptr, &m_CmdPool), VK_SUCCESS, "Vk: GPUContextImpl: Failed to create command pool");

    VkCommandBufferAllocateInfo buffer_info;
    buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandPool = m_CmdPool;
    buffer_info.commandBufferCount = 1;

    CoreFunctionAssert(vkAllocateCommandBuffers(m_Owner->Handle, &buffer_info, &m_CmdBuffer), VK_SUCCESS, "Vk: GPUContextImpl: Failed to allocate command buffer");

    { // XXX: Signal first semaphore to avoid lock
        BeginImpl();
        EndImpl();
        SubmitCmdBuffer(m_Owner->GraphicsQueue, m_CmdBuffer, ArrayPtr<const VkSemaphore>(), ArrayPtr<const VkSemaphore>(&m_SemaphoreRing[0].Handle, 1), VK_NULL_HANDLE);
    }
}


GPUContextImpl::~GPUContextImpl(){
    vkQueueWaitIdle(m_Owner->GraphicsQueue.Handle);

    vkFreeCommandBuffers(m_Owner->Handle, m_CmdPool, 1, &m_CmdBuffer);

    vkDestroyCommandPool(m_Owner->Handle, m_CmdPool, nullptr);
}

void GPUContextImpl::BeginImpl(){
    VkCommandBufferBeginInfo begin_info;
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    CoreFunctionAssert(vkBeginCommandBuffer(m_CmdBuffer, &begin_info), VK_SUCCESS, "Vk: GPUContext: Failed to begin CmdBuffer");
}

void GPUContextImpl::EndImpl(){
    CoreFunctionAssert(vkEndCommandBuffer(m_CmdBuffer),VK_SUCCESS, "Vk: GPUContext: Failed to end CmdBuffer for submission");
}

void GPUContextImpl::SubmitImpl(){
    auto semaphores = NextPair();
    SubmitCmdBuffer(m_Owner->GraphicsQueue, m_CmdBuffer, ArrayPtr<const VkSemaphore>(&semaphores.First, 1), ArrayPtr<const VkSemaphore>(&semaphores.Second, 1), VK_NULL_HANDLE);
}

void GPUContextImpl::CopyImpl(const CPUBuffer &src, const GPUBuffer &dst, u32 size, u32 src_offset, u32 dst_offset){  
    CoreAssert(size + dst_offset <= dst.Size(), "Vk: GPUContext: Copy: Dst Buffer overflow");

    VkBufferCopy copy;
    copy.dstOffset = dst_offset;
    copy.srcOffset = src_offset;
    copy.size = size;

    vkCmdCopyBuffer(m_CmdBuffer, (VkBuffer)src.Handle().U64, (VkBuffer)dst.Handle().U64, 1, &copy);

    CmdMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);
}

void GPUContextImpl::CopyImpl(const CPUTexture &src, const GPUTexture &dst){
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

void GPUContextImpl::ChangeLayoutImpl(GPUTexture &texture, GPUTexture::Layout new_layout){

    CmdImageBarrier(
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
        VK_ACCESS_MEMORY_WRITE_BIT, 
        VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT, 
        GPUTextureImpl::s_LayoutTable[(size_t)texture.GetLayout()], 
        GPUTextureImpl::s_LayoutTable[(size_t)new_layout], 
        (VkImage)texture.Handle().U64
    );

    texture.m_Layout = new_layout;
}

void GPUContextImpl::BindImpl(const GraphicsPipeline *pipeline){
    auto *pipeline_impl = static_cast<const Vk::GraphicsPipelineImpl *>(pipeline);
    vkCmdBindPipeline(m_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_impl->Handle);
    
    vkCmdSetScissor(m_CmdBuffer, 0, 1, &pipeline_impl->Scissors);

    VkViewport viewport;
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0;
    viewport.x = pipeline_impl->Scissors.offset.x;
    viewport.y = pipeline_impl->Scissors.extent.height - pipeline_impl->Scissors.offset.y;
    viewport.width  = pipeline_impl->Scissors.extent.width;
    viewport.height = -(float)pipeline_impl->Scissors.extent.height;
    vkCmdSetViewport(m_CmdBuffer, 0, 1, &viewport);

    vkCmdBindDescriptorSets(m_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_impl->Layout, 0, 1, &pipeline_impl->Set, 0, nullptr);
}

void GPUContextImpl::BeginRenderPassImpl(const RenderPass *pass, const Framebuffer *framebuffer){
    m_RenderPass = static_cast<const Vk::RenderPassImpl*>(pass);
    m_Framebuffer = static_cast<const Vk::FramebufferImpl*>(framebuffer);

    VkRenderPassBeginInfo info;
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.clearValueCount = 0;
    info.pClearValues = nullptr;
    info.framebuffer = m_Framebuffer->Handle;
    info.renderPass = m_RenderPass->Handle;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent.width = framebuffer->Size().x;
    info.renderArea.extent.height = framebuffer->Size().y;
    
    vkCmdBeginRenderPass(m_CmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void GPUContextImpl::EndRenderPassImpl(){
    vkCmdEndRenderPass(m_CmdBuffer);
    m_RenderPass = nullptr;
    m_Framebuffer = nullptr;
}

void GPUContextImpl::BindVertexBufferImpl(const GPUBuffer &buffer){
    VkBuffer handle = reinterpret_cast<VkBuffer>(buffer.Handle().U64);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(m_CmdBuffer, 0, 1, &handle, &offset);
}

void GPUContextImpl::BindIndexBufferImpl(const GPUBuffer &buffer, IndicesType indices_type){
    VkBuffer handle = reinterpret_cast<VkBuffer>(buffer.Handle().U64);
    vkCmdBindIndexBuffer(m_CmdBuffer, handle, 0, s_IndexTypeTable[(size_t)indices_type]);
}

void GPUContextImpl::DrawIndexedImpl(u32 indices_count){
    vkCmdDrawIndexed(m_CmdBuffer, indices_count, 1, 0, 0, 0);
}

void GPUContextImpl::CmdPipelineBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst){
    vkCmdPipelineBarrier(m_CmdBuffer, src, dst, 0, 0, nullptr, 0, nullptr, 0, nullptr);
}

void GPUContextImpl::CmdMemoryBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkAccessFlags src_access, VkAccessFlags dst_access){
    VkMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = src_access;
    barrier.dstAccessMask = dst_access;

    vkCmdPipelineBarrier(m_CmdBuffer, src, dst, 0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void GPUContextImpl::CmdImageBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, 
                                     VkAccessFlags src_acces, VkAccessFlags dst_access, 
                                     VkImageLayout old, VkImageLayout next, VkImage img)
{
    VkImageMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = src_acces;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = old;
    barrier.newLayout = next;
    barrier.srcQueueFamilyIndex = 0;
    barrier.dstQueueFamilyIndex = 0;
    barrier.image = img;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(m_CmdBuffer, src, dst, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


Pair<VkSemaphore, VkSemaphore> GPUContextImpl::NextPair(){
    Pair<VkSemaphore, VkSemaphore> result = {m_SemaphoreRing[m_SemaphoreRingCounter % SemaphoreRingSize].Handle, m_SemaphoreRing[(m_SemaphoreRingCounter + 1) % SemaphoreRingSize].Handle};
    ++m_SemaphoreRingCounter;
    return result;
}

void GPUContextImpl::SubmitCmdBuffer(Vk::Queue queue, VkCommandBuffer cmd_buffer, const ArrayPtr<const VkSemaphore> &wait_semaphores, const ArrayPtr<const VkSemaphore> &signal_semaphores, VkFence signal_fence){
    auto *stages = (VkPipelineStageFlags*)alloca(wait_semaphores.Size() * sizeof(VkPipelineStageFlags));
    for(size_t i = 0; i<wait_semaphores.Size(); ++i){
        stages[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }

    VkSubmitInfo info;
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &cmd_buffer;
    info.waitSemaphoreCount = wait_semaphores.Size();
    info.pWaitSemaphores = wait_semaphores.Pointer();
    info.signalSemaphoreCount = signal_semaphores.Size();
    info.pSignalSemaphores = signal_semaphores.Pointer();
    info.pWaitDstStageMask = stages;

    CoreFunctionAssert(vkQueueSubmit(queue.Handle, 1, &info, signal_fence), VK_SUCCESS, "Vk: LogicalGPU: Failed to submit CmdBuffer");
    vkQueueWaitIdle(m_Owner->GraphicsQueue.Handle); // TODO Get rid of this // Context is Immediate mode for now :'-
}

void GPUContextImpl::ClearFramebufferColorAttachmentsImpl(const Framebuffer *fb, const Vector4f &color){
    auto fb_impl = static_cast<const Vk::FramebufferImpl*>(fb);
    CoreAssert(m_Framebuffer != fb_impl, "Vk: GPUContextImpl: can't clear framebuffer which is being used in current render pass");

    VkClearColorValue value;
    value.float32[0] = color[0];
    value.float32[1] = color[1];
    value.float32[2] = color[2];
    value.float32[3] = color[3];

    VkImageSubresourceRange issr;
    issr.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    issr.baseArrayLayer = 0;
    issr.baseMipLevel = 0;
    issr.levelCount = 1;
    issr.layerCount = 1;

    for(auto att: fb_impl->Attachments){
        if(IsColorFormat(att->GetFormat())){

            auto layout = GPUTextureImpl::s_LayoutTable[(size_t)att->GetLayout()];
            constexpr auto clear_layout = VK_IMAGE_LAYOUT_GENERAL;

            CmdImageBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_WRITE_BIT, layout, clear_layout, (VkImage)att->Handle().U64);

            vkCmdClearColorImage(m_CmdBuffer, (VkImage)att->Handle().U64, clear_layout, &value, 1, &issr);

            CmdImageBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_READ_BIT, clear_layout, layout, (VkImage)att->Handle().U64);
        }
    }
}

void GPUContextImpl::SwapFramebuffersImpl(Swapchain *swapchain){
    auto *swapchain_impl = static_cast<Vk::SwapchainImpl*>(swapchain);
    auto semaphores = NextPair();

    swapchain_impl->PresentCurrent(semaphores.First);
    swapchain_impl->AcquireNext(semaphores.Second);
}

GPUContext *GPUContextImpl::NewImpl(LogicalGPU &owner){
    return new(Memory::Alloc(sizeof(GPUContextImpl))) GPUContextImpl(static_cast<Vk::LogicalGPUImpl*>(&owner));
}

void GPUContextImpl::DeleteImpl(GPUContext *context){
    context->~GPUContext();
    Memory::Free(context);
}

}//namespace Vk::
}//namespace StraitX::