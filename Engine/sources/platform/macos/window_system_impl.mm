#import <Cocoa/Cocoa.h>
#include "platform/window_system.hpp"

namespace StraitX{

Result WindowSystem::Initialize(){

    NSApplication *app = [NSApplication sharedApplication];
    [app setActivationPolicy: NSApplicationActivationPolicyRegular];
    [app activateIgnoringOtherApps: YES];

    return ResultError(NSApp == nil);
}

Result WindowSystem::Finalize(){
    return Result::Success;
}

Screen WindowSystem::MainScreen(){
    NSScreen *screen = [NSScreen mainScreen];

    NSDictionary *description = [screen deviceDescription];
    NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
    CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);

    return MacOS::ScreenImpl(
        {(s32)screen.frame.size.width, (s32)screen.frame.size.height}, 
        {float(displayPixelSize.width / displayPhysicalSize.width) * 25.4f, float(displayPixelSize.height / displayPhysicalSize.height) * 25.4f}
    );
}

}//namespace StraitX::