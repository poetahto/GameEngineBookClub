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

} // application

#endif // APPLICATION_H
