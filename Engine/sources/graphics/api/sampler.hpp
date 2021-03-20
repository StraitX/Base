#ifndef STRAITX_SAMPLER_HPP
#define STRAITX_SAMPLER_HPP

#include "platform/types.hpp"
#include "platform/compiler.hpp"
#include "graphics/api/gpu_configuration.hpp"
#include "graphics/api/logical_gpu.hpp"

namespace StraitX{

enum class FilteringMode: u8{
    Linear,
    Nearest
};

enum class WrapMode: u8{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

struct SamplerProperties{
    FilteringMode MinFiltering = FilteringMode::Linear;
    FilteringMode MagFiltering = FilteringMode::Linear;
    WrapMode WrapU = WrapMode::ClampToEdge;
    WrapMode WrapV = WrapMode::ClampToEdge;
    WrapMode WrapS = WrapMode::ClampToEdge;
};

namespace GL{
struct SamplerImpl;
}//namespace GL::

namespace Vk{
struct SamplerImpl;
}//namespace Vk::

class GraphicsAPILoader;

class Sampler{
public:
    struct VTable{
        using NewProc    = void (*)(Sampler &sampler, LogicalGPU &owner, SamplerProperties props);
        using DeleteProc = void (*)(Sampler &sampler);

        NewProc    New    = nullptr;
        DeleteProc Delete = nullptr;
    };
private:
    LogicalGPU *m_Owner;
    GPUResourceHandle m_Handle;

    static VTable s_VTable;
    friend class GraphicsAPILoader;
    friend class Vk::SamplerImpl;
    friend class GL::SamplerImpl;
public:
    Sampler() = default;
#ifdef SX_DEBUG
    ~Sampler();
#endif
    sx_inline void New(SamplerProperties props);

    sx_inline void Delete();

    constexpr GPUResourceHandle Handle()const;
};

#ifdef SX_DEBUG
sx_inline Sampler::~Sampler(){
    CoreAssert(m_Handle.U64 == 0, "Sampler: Delete() should be called before destruction");
}
#endif

sx_inline void Sampler::New(SamplerProperties props){
    s_VTable.New(*this, LogicalGPU::Instance(), props);
}

sx_inline void Sampler::Delete(){
    s_VTable.Delete(*this);
    m_Handle.U64 = 0;
}

constexpr GPUResourceHandle Sampler::Handle()const{
    return m_Handle;
}

};

#endif//STRAITX_SAMPLER_HPP