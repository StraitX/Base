#ifndef STRAITX_IMAGE_HPP
#define STRAITX_IMAGE_HPP

#include "platform/types.hpp"
#include "platform/file.hpp"
#include "core/math/vector2.hpp"
#include "core/noncopyable.hpp"
#include "graphics/api/format.hpp"
#include "graphics/color.hpp"
#include "graphics/image_loader.hpp"

namespace StraitX{

// image is NonCopyable just for now
class Image: public NonCopyable{
private:
    u8 *m_Data = nullptr;
    u32 m_Width = 0;
    u32 m_Height = 0;
    PixelFormat m_Format = {};
public:   
    Image() = default;

    Image(u32 width, u32 height, const Color &color = Color::Black);

    Image(Image &&other);

    ~Image();

    Image &operator=(Image &&other);

    void Create(u32 width, u32 height, const Color &color = Color::Black);

    Result LoadFromFile(const char *filename, PixelFormat desired_format = PixelFormat::RGBA8);

    Result LoadFromFile(File &file, PixelFormat desired_format = PixelFormat::RGBA8);

    Result SaveToFile(File &file, ImageFormat save_format);

    Result SaveToFile(const char *filename);

    u8 *Data()const;

    u32 Width()const;

    u32 Height()const;

    Vector2u Size()const;

    PixelFormat Format()const;

    bool IsEmpty()const;
};

sx_inline u8 *Image::Data()const{
    return m_Data;
}

sx_inline u32 Image::Width()const{
    return m_Width;
}

sx_inline u32 Image::Height()const{
    return m_Height;
}

sx_inline Vector2u Image::Size()const{
    return {Width(), Height()};
}

sx_inline PixelFormat Image::Format()const{
    return m_Format;
}

sx_inline bool Image::IsEmpty()const{
    return m_Data == nullptr;
}

};//namespace StraitX::

#endif//STRAITX_IMAGE_HPP