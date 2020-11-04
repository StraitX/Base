#include "platform/platform.hpp"
#include "platform/windows/hinstance.hpp"
#include "platform/windows/window_win32.hpp"

namespace StraitX {

Error Platform::Initialize() {
    Windows::HInstance::Instance().Initialize();
    Windows::WindowWin32::RegClass();
    return ErrorCode::Success;
}


Error Platform::Finalize() {
    return ErrorCode::Success;
}

}; // namespace StratiX::