#include "platform/linux/screen_impl.hpp"

namespace Linux{

ScreenImpl::ScreenImpl(unsigned int index, void *handle, const Size2i &size, const Size2f &dpi):
    m_Handle(handle),
    m_Index(index),
    m_Size(size),
    m_DPI(dpi)
{}
    
const Size2i &ScreenImpl::Size()const{
    return m_Size;
}

const Size2f &ScreenImpl::DPI()const{
    return m_DPI;
}

} // namespace Linux::