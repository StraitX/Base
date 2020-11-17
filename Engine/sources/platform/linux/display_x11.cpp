#include "platform/linux/display_x11.hpp"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <string.h>
#undef Success
#include "platform/linux/screen_x11.hpp"
#include "platform/types.hpp"
//undef X11 macro to resolve conflicts

#include "platform/error.hpp"


namespace StraitX{
namespace Linux{

Error DisplayX11::Open(){
    m_Handle = XOpenDisplay(nullptr);
    return m_Handle != nullptr ? Error::Success : Error::Failure;
}
Error DisplayX11::Close(){
    return XCloseDisplay(m_Handle)==0 ? Error::Success : Error::Failure;
}

bool DisplayX11::IsOpened(){
    return m_Handle;
}

bool DisplayX11::CheckSupport(Ext extension){
    switch (extension) {
    case Ext::DoubleBuffer: return CheckX11Extension("DOUBLE-BUFFER");
    case Ext::OpenGLLegacy: return CheckX11Extension("GLX");
    case Ext::OpenGLCore: return (CheckX11Extension("GLX") ? CheckGLXExtension("GLX_ARB_create_context") : false);
    case Ext::Count: return false;
    }
    return false;
}

ScreenX11 DisplayX11::MainScreen(){
    Screen *screen = DefaultScreenOfDisplay(m_Handle);

    Size2i size;
    size.width = XWidthOfScreen(screen);
    size.height = XHeightOfScreen(screen);

    Size2f dpi;
    dpi.width = float(size.width) / (float(XWidthMMOfScreen(screen)) / 25.4f);
    dpi.height = float(size.height) / (float(XHeightMMOfScreen(screen)) / 25.4f);

    return ScreenX11(DefaultScreen(m_Handle),screen,size,dpi);
}

::Display *DisplayX11::Handle(){
    return m_Handle;
}

bool DisplayX11::CheckX11Extension(const char *name){
    int opcode, event_base, error_base;
    return XQueryExtension(m_Handle, name, &opcode, &event_base, &error_base);
}

bool DisplayX11::CheckGLXExtension(const char *name){
    const char *extensions = glXQueryExtensionsString(m_Handle, DefaultScreen(m_Handle));

    return strstr(extensions, name) != 0;
}

};// namespace Linux::
};// namespace StraitX::