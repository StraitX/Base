#ifndef STRAITX_SAMPLER_HPP
#define STRAITX_SAMPLER_HPP

#include "platform/types.hpp"
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
    FilteringMode MinFiltering;
    FilteringMode MagFiltering;
    WrapMode WrapU;
    WrapMode WrapV;
    WrapMode WrapS;
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
        using NewProc    = void (*)(Sampler &sampler, SamplerProperties props);
        using DeleteProc = void (*)(Sampler &sampler);

        NewProc    New    = nullptr;
        DeleteProc Delete = nullptr;
    };
private:
    LogicalGPU *const m_Owner;
    GPUResourceHandle m_Handle;

    static VTable s_VTable;
    friend class GraphicsAPILoader;
    friend class Vk::SamplerImpl;
    friend class GL::SamplerImpl;
public:
    sx_inline Sampler();

    sx_inline void New(SamplerProperties props);

    sx_inline void Delete();

    constexpr GPUResourceHandle Handle()const;
};

sx_inline Sampler::Sampler():
    m_Owner(&LogicalGPU::Instance())
{}

sx_inline void Sampler::New(SamplerProperties props){
    s_VTable.New(*this, props);
}

sx_inline void Sampler::Delete(){
    s_VTable.Delete(*this);
}

constexpr GPUResourceHandle Sampler::Handle()const{
    return m_Handle;
}

};

#endif//STRAITX_SAMPLER_HPP