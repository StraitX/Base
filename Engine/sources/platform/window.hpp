#ifndef STRAITX_WINDOW_HPP
#define STRAITX_WINDOW_HPP

#include "platform/platform_detection.hpp"
#include "platform/events.hpp"
#include "platform/screen.hpp"
#include "platform/noncopyable.hpp"

#ifdef SX_PLATFORM_LINUX
    #include "platform/linux/window_impl.hpp"
    typedef Linux::WindowImpl PlatformWindowImpl;
#elif defined(SX_PLATFORM_WINDOWS)
    #include "platform/windows/window_impl.hpp"
    typedef Windows::WindowImpl PlatformWindowImpl;
#elif defined(SX_PLATFORM_MACOS)
    #include "platform/macos/window_impl.hpp"
    typedef MacOS::WindowImpl PlatformWindowImpl;
#else
    #error "Your platform does not support window creation"
#endif

class PlatformWindow: public NonCopyable{
private:
    PlatformWindowImpl m_Impl;
public:
    PlatformWindow() = default;

    PlatformWindow(PlatformWindow &&other);

    Result Open(const PlatformScreen &screen, int width, int height);

    Result Close();

    PlatformWindowImpl &Impl();

    const PlatformWindowImpl &Impl()const;

    bool IsOpen()const;

    void SetTitle(const char *title);

    bool PollEvent(Event &event);

    Size2u Size()const;
};

SX_INLINE PlatformWindow::PlatformWindow(PlatformWindow &&other):
    m_Impl((PlatformWindowImpl&&)other.Impl())
{}

SX_INLINE Result PlatformWindow::Open(const PlatformScreen &screen, int width, int height){
    return m_Impl.Open(screen.Impl(), width, height);
}

SX_INLINE Result PlatformWindow::Close(){
    return m_Impl.Close();
}

SX_INLINE PlatformWindowImpl &PlatformWindow::Impl(){
    return m_Impl;
}

SX_INLINE const PlatformWindowImpl &PlatformWindow::Impl()const{
    return m_Impl;
}

SX_INLINE bool PlatformWindow::IsOpen()const{
    return m_Impl.IsOpen();
}

SX_INLINE void PlatformWindow::SetTitle(const char *title){
    m_Impl.SetTitle(title);
}

SX_INLINE bool PlatformWindow::PollEvent(Event &event){
    return m_Impl.PollEvent(event);
}

SX_INLINE Size2u PlatformWindow::Size()const{
    return m_Impl.Size();
}

#endif // STRAITX_WINDOW_HPP