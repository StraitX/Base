#ifndef STRAITX_LINUX_SCREEN_IMPL_HPP
#define STRAITX_LINUX_SCREEN_IMPL_HPP

#include "platform/types.hpp"


namespace Linux{

class WindowImpl;

class ScreenImpl{
private:
    void *m_Handle;
    unsigned int m_Index;
    Size2i m_Size;
    Size2f m_DPI;

    friend class WindowImpl;
public:
    ScreenImpl(unsigned int index, void *handle, const Size2i &size, const Size2f &dpi);

    ~ScreenImpl() = default;
    
    const Size2i &Size()const;

    const Size2f &DPI()const;
};

} // namespace Linux::


#endif // STRAITX_LINUX_SCREEN_IMPL_HPP