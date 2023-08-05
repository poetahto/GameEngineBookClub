#ifndef PLATFORM_H
#define PLATFORM_H

#include "math/rect.h"
#include "math/vec2.h"

namespace platform
{
    void createWindow(const wchar_t *name, int width, int height);
    Rect getWindowRect();
    void swapWindowBuffers();

    // Platform-level renderer setup, add more if needed.
    void setupOpenGl();

    // === Input ===
    void updateInput();
    bool wantsToQuit();
    Vec2 getInputDirection();
    Vec2 getMouseDelta();

} // namespace platform

#endif // PLATFORM_H
