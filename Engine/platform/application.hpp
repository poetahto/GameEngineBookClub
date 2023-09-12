#ifndef APPLICATION_H
#define APPLICATION_H

struct Vec2;
struct Rect;

namespace Application
{
    struct Settings
    {
        static constexpr s32 DEFAULT_X = 100;
        static constexpr s32 DEFAULT_Y = 100;
        static constexpr s32 DEFAULT_WIDTH = 800;
        static constexpr s32 DEFAULT_HEIGHT = 600;

        const char* title = "";
        s32 x = DEFAULT_X;
        s32 y = DEFAULT_Y;
        s32 width = DEFAULT_WIDTH;
        s32 height = DEFAULT_HEIGHT;
    };

    void init(const Settings& settings = Settings {});
    void free();

    void swapBuffers();

    const char* getTitle();
    void setTitle(const char* title);

    Vec2 getPosition();
    void setPosition(Vec2 position);

    Vec2 getSize();
    void setSize(Vec2 size);

    s32 getRefreshRate();
    void setRefreshRate(s32 rate);

    u64 getPerformanceCounter();
    u64 getPerformanceFrequency();

} // Application

namespace Input
{
    enum KeyCode
    {
        W = 1 << 0,
        A = 1 << 1,
        S = 1 << 2,
        D = 1 << 3,
        Escape = 1 << 4,
        Space = 1 << 5,
        LeftShift = 1 << 6,
    };

    typedef void (*ResizeHandler) (s32 width, s32 height);
    void setResizeHandler(ResizeHandler resizeHandler);
    void pollInput();
    f32 getAxis(KeyCode positive, KeyCode negative);
    bool getKey(KeyCode key);
    bool getKeyDown(KeyCode key);
    bool getKeyUp(KeyCode key);
    bool wantsToQuit();
    Vec2 getMouseDelta();

} // Input

#endif // APPLICATION_H
