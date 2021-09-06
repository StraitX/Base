#ifndef STRAITX_VULKAN_BUFFER_IMPL_HPP
#define STRAITX_VULKAN_BUFFER_IMPL_HPP

#include "graphics/api/vulkan/gpu_impl.hpp"
#include "graphics/api/buffer.hpp"

namespace Vk{

class BufferImpl: public Buffer{
private:
    VkBuffer m_Handle = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    void *m_Pointer = nullptr;
    size_t m_Size;
    BufferMemoryType m_MemoryType;
    MemoryType::Type m_RealMemoryType;
    BufferUsage m_Usage;
public:
    BufferImpl(size_t size, BufferMemoryType mem_type, BufferUsage usage);

    ~BufferImpl();

    void Copy(const void *data, size_t size, size_t offset)override;

    bool IsMappable()const override;

    bool IsMapped()const override;

    void *Map()override;

    void Unmap()override;

    size_t Size()const override;

    BufferMemoryType MemoryType()const override;

    BufferUsage Usage()const override;

    operator VkBuffer()const{
        return m_Handle;
    }
};

}//namespace Vk::

#endif//STRAITX_VULKAN_BUFFER_IMPL_HPP