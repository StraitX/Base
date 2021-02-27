#ifndef STRAITX_GRAPHICS_PIPELINE_HPP
#define STRAITX_GRAPHICS_PIPELINE_HPP

#include "core/array_ptr.hpp"
#include "core/noncopyable.hpp"
#include "core/validable.hpp"
#include "graphics/api/shader.hpp"
#include "graphics/api/logical_gpu.hpp"
#include "graphics/api/render_pass.hpp"

namespace StraitX{


enum class VertexAttribute{
    Int = 0,
    Int2,
    Int3,
    Int4,

    Uint,
    Uint2,
    Uint3,
    Uint4,

    Float,
    Float2,
    Float3,
    Float4
};
/*
enum class ShaderBindingType{
    UniformBuffer = 1,
    //Sampler       = 2
};


struct ShaderBinding{
    ShaderBindingType Type;
    Shader::Type ShaderStage;

    constexpr ShaderBinding(ShaderBindingType type, Shader::Type shader_stage):
        Type(type),
        ShaderStage(shader_stage)
    {}
};
*/
enum class PrimitivesTopology{
    Points = 0,
    Lines,
    LinesStrip,
    Triangles,
    TrianglesStrip
};

enum class RasterizationMode{
    Fill = 0,
    Lines,
    Points
};

enum class BlendFunction{
    Add,
};

enum class BlendFactor{
    Zero,
    One,
    OneMinusSrcAlpha,
    OneMinusDstAlpha,
    SrcAlpha,
    DstAlpha 
};

struct Viewport{
    s32 x;
    s32 y;
    u32 Width;
    u32 Height;
};

struct GraphicsPipelineProperties{
    ArrayPtr<const Shader* const> Shaders;
    ArrayPtr<const VertexAttribute> VertexAttributes;
    PrimitivesTopology Topology;
    Viewport FramebufferViewport;
    RasterizationMode Rasterization;
    BlendFunction BlendFunc;
    BlendFactor SrcBlendFactor;
    BlendFactor DstBlendFactor;
    const RenderPass *Pass;
    //ArrayPtr<const ShaderBinding> ShaderBindings;
};
//OpenGL said that
constexpr size_t MaxVertexAttributes = 8;

class GraphicsAPILoader;

class GraphicsPipeline: public Validable, public NonCopyable{
public:
    struct VTable{
        using NewProc    = GraphicsPipeline * (*)(LogicalGPU &owner, const GraphicsPipelineProperties &props);
        using DeleteProc = void (*)(GraphicsPipeline *);

        NewProc    New    = nullptr;
        DeleteProc Delete = nullptr;
    };
private:
    static VTable s_VTable;

    friend class GraphicsAPILoader;
protected:
    static u32 s_VertexAttributeSizeTable[];
public:
    GraphicsPipeline(const GraphicsPipelineProperties &props);

    virtual ~GraphicsPipeline() = default;

    sx_inline static GraphicsPipeline *New(const GraphicsPipelineProperties &props);

    sx_inline static void Delete(GraphicsPipeline *pipeline);

    static size_t CalculateStride(const ArrayPtr<const VertexAttribute> &attributes);

};

sx_inline GraphicsPipeline *GraphicsPipeline::New(const GraphicsPipelineProperties &props){
    return s_VTable.New(LogicalGPU::Instance(), props);
}

sx_inline void GraphicsPipeline::Delete(GraphicsPipeline *pipeline){
    s_VTable.Delete(pipeline);
}

}//namespace StraitX::

#endif//STRAITX_GRAPHICS_PIPELINE_HPP