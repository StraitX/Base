#include "graphics/vulkan/semaphore.hpp"
#include "graphics/vulkan/gpu.hpp"

namespace StraitX{
namespace Vk{

Semaphore::Semaphore()
{
    VkSemaphoreCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    CoreFunctionAssert(vkCreateSemaphore(GPU::Get().Handle(), &info, nullptr, &Handle),VK_SUCCESS, "Vk: Can't create semaphore");
}

Semaphore::~Semaphore(){
    vkDestroySemaphore(GPU::Get().Handle(), Handle, nullptr);
}

}//Vk::
}//StraitX::