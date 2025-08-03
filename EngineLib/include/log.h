#pragma once

// --- Text Color ---
enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

// --- Internal Logging ---
static inline void _LogVA(const char* prefix, const char* file, int line, int color, const char* fmt, va_list args)
{
    static const char* colorCodes[TEXT_COLOR_COUNT] = {
        "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m",
        "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m",
        "\x1b[90m", "\x1b[91m", "\x1b[92m", "\x1b[93m",
        "\x1b[94m", "\x1b[95m", "\x1b[96m", "\x1b[97m"};

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    printf("%s%s [%s:%d] %s\x1b[0m\n", colorCodes[color], prefix, file, line, buffer);
}

static inline void _Log(const char* prefix, const char* file, int line, int color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _LogVA(prefix, file, line, color, fmt, args);
    va_end(args);
}
