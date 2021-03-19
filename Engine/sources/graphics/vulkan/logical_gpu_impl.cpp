#include "platform/memory.hpp"
#include "core/log.hpp"
#include "core/assert.hpp"
#include "core/array_ptr.hpp"
#include "core/string.hpp"
#include "graphics/vulkan/logical_gpu_impl.hpp"
#include "graphics/vulkan/cpu_buffer_impl.hpp"

namespace StraitX{
namespace Vk{

LogicalGPUImpl LogicalGPUImpl::Instance;

Result LogicalGPUImpl::Initialize(const PhysicalGPU &gpu){
    { //First Stage Init
        PhysicalHandle = reinterpret_cast<VkPhysicalDevice>(gpu.Handle.U64);
        Vendor         = gpu.Vendor;
        Type           = gpu.Type;

        QueryQueues();
        Memory.Query(PhysicalHandle);    
    }
    
    { // Device creation
        const char *ext[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceQueueCreateInfo qinfo[2];
        //GraphicsQueue
        float gprior = 1.0;
        qinfo[0].sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qinfo[0].pNext              = nullptr;
        qinfo[0].flags              = 0;
        qinfo[0].queueFamilyIndex   = GraphicsQueueFamily;
        qinfo[0].queueCount         = 1;
        qinfo[0].pQueuePriorities   = &gprior;

        //Optional Transfer Queue
        float tprior = 0.8;
        qinfo[1].sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qinfo[1].pNext              = nullptr;
        qinfo[1].flags              = 0;
        qinfo[1].queueFamilyIndex   = TransferQueueFamily;
        qinfo[1].queueCount         = 1;
        qinfo[1].pQueuePriorities   = &tprior;


        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(PhysicalHandle, &features);

        VkDeviceCreateInfo info;
        info.sType                  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;
        info.enabledExtensionCount  = sizeof(ext)/sizeof(const char *);
        info.ppEnabledExtensionNames= ext;
        info.enabledLayerCount      = 0;
        info.ppEnabledLayerNames    = nullptr;
        info.pEnabledFeatures       = &features;
        info.queueCreateInfoCount   = TransferQueueFamily != InvalidIndex ? 2 : 1;
        info.pQueueCreateInfos      = qinfo;

        LogInfo("Vulkan: Creating device with % queues", info.queueCreateInfoCount);

        if(vkCreateDevice(PhysicalHandle,&info,nullptr,&Handle) != VK_SUCCESS)
            return Result::NullObject;
    }

    {//Obtain Queues
        GraphicsQueue.Obtain(Handle, GraphicsQueueFamily, 0);
        CoreAssert(GraphicsQueue.IsInitialized(), "LogicalDevice: can't retrieve GraphicsQueue handle");
        
        if(TransferQueueFamily != InvalidIndex){
            TransferQueue.Obtain(Handle, TransferQueueFamily, 0);
            CoreAssert(TransferQueue.IsInitialized(), "LogicalDevice: can't retrieve TransferQueue handle");       
        }

    }
    Allocator.Initialize(this);

    VkCommandPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = GraphicsQueue.FamilyIndex;

    CoreFunctionAssert(vkCreateCommandPool(Handle, &pool_info, nullptr, &Pool), VK_SUCCESS, "Vk: LogicalGPU: Failed to create command pool");

    VkCommandBufferAllocateInfo buffer_info;
    buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandPool = Pool;
    buffer_info.commandBufferCount = 1;

    CoreFunctionAssert(vkAllocateCommandBuffers(Handle, &buffer_info, &TransferCmdBuffer), VK_SUCCESS, "Vk: LogicalGPU: Failed to allocate command buffer");

    return Result::Success;
}

void LogicalGPUImpl::Finalize(){
    vkDeviceWaitIdle(Handle);

    Allocator.Finalize();
    
    vkDestroyDevice(Handle,nullptr);
}

sx_inline bool IsGraphics(VkQueueFlags flags){
    return flags & VK_QUEUE_GRAPHICS_BIT
    &&     flags & VK_QUEUE_COMPUTE_BIT
    &&     flags & VK_QUEUE_TRANSFER_BIT;
}

// pure compute queue // Most probably a DMA engine queue
sx_inline bool IsTransfer(VkQueueFlags flags){
    return !(flags & VK_QUEUE_GRAPHICS_BIT)
    &&     !(flags & VK_QUEUE_COMPUTE_BIT)
    &&       flags & VK_QUEUE_TRANSFER_BIT;
}
// we treat non-graphics queue as compute
sx_inline bool IsCompute(VkQueueFlags flags){
    return  !(flags & VK_QUEUE_GRAPHICS_BIT)
    &&        flags & VK_QUEUE_COMPUTE_BIT
    &&        flags & VK_QUEUE_TRANSFER_BIT;
}

void LogicalGPUImpl::QueryQueues(){
    u32 props_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalHandle, &props_count, nullptr);
    VkQueueFamilyProperties *props = (VkQueueFamilyProperties*)alloca(props_count*sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalHandle,&props_count,props);

    int i = 0;
    for(; i<props_count; i++){
        if(IsGraphics(props[i].queueFlags))
            GraphicsQueueFamily = i;
        if(IsCompute(props[i].queueFlags))
            ComputeQueueFamily = i;
        if(IsTransfer(props[i].queueFlags))
            TransferQueueFamily = i;
    }

    DLogInfo("Vulkan: GraphicsQueue  %",GraphicsQueueFamily != InvalidIndex);
    DLogInfo("Vulkan: ComputeQueue   %",ComputeQueueFamily != InvalidIndex);
    DLogInfo("Vulkan: TransferQueue  %",TransferQueueFamily != InvalidIndex);


    //higher level queue fallback
    //we are going to have one queue instance of each family
    if(GraphicsQueueFamily != InvalidIndex){
        if(ComputeQueueFamily == InvalidIndex && props[GraphicsQueueFamily].queueCount >= 2){
            ComputeQueueFamily = GraphicsQueueFamily;
            props[GraphicsQueueFamily].queueCount-=1;
            
            DLogInfo("Vulkan: Fallback ComputeQueue   %",ComputeQueueFamily != InvalidIndex);
	    }
        if(TransferQueueFamily == InvalidIndex && props[GraphicsQueueFamily].queueCount >= 2){
            TransferQueueFamily = GraphicsQueueFamily;
            DLogInfo("Vulkan: Fallback TransferQueue  %",TransferQueueFamily != InvalidIndex);
        }
        if(TransferQueueFamily == InvalidIndex && ComputeQueueFamily != InvalidIndex && props[ComputeQueueFamily].queueCount >= 2){
            TransferQueueFamily = ComputeQueueFamily;
            DLogInfo("Vulkan: Fallback TransferQueue  %",TransferQueueFamily != InvalidIndex);
        }
    }

}

}//namespace Vk::
}//namespace StraitX::