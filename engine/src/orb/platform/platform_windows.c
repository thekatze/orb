#include "platform.h"

#ifdef ORB_PLATFORM_WINDOWS
#include "../core/application.h"
#include "../core/event.h"
#include "../core/input.h"
#include "../core/logger.h"

#include <Windows.h>
#include <stdlib.h>
#include <windowsx.h>

typedef struct internal_state {
    HINSTANCE h_instance;
    HWND hwnd;
} internal_state;

static internal_state *state;
static LARGE_INTEGER start_time;

LRESULT CALLBACK process_win32_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 orb_platform_init(usize *memory_requirement, void *memory,
                     struct orb_application_config *config) {
    *memory_requirement = sizeof(internal_state);
    if (memory == nullptr) {
        return true;
    }

    state = (internal_state *)memory;

    state->h_instance = GetModuleHandleA(nullptr);
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);

    const char *WINDOW_CLASS_NAME = "orb_window_class";

    WNDCLASSA window_class = {
        .style = CS_DBLCLKS,
        .lpfnWndProc = process_win32_message,
        .hInstance = state->h_instance,
        .hIcon = icon,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .lpszClassName = WINDOW_CLASS_NAME,
    };

    if (!RegisterClassA(&window_class)) {
        ORB_ERROR("Window class registration failed");
        return false;
    }

    // client size and window size are different; window size includes borders and title bar
    // we want our config to be the client size
    i32 window_x = config->x;
    i32 window_y = config->y;
    i32 window_width = config->width;
    i32 window_height = config->height;

    u32 window_style =
        WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    u32 window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {};
    AdjustWindowRectEx(&border_rect, window_style, false, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    state->hwnd = CreateWindowExA(window_ex_style, WINDOW_CLASS_NAME, config->name,
                                  window_style, window_x, window_y, window_width, window_height,
                                  nullptr, nullptr, state->h_instance, nullptr);

    if (!state->hwnd) {
        ORB_ERROR("Window creation failed");
        return false;
    }

    ShowWindow(state->hwnd, SW_SHOW);

    QueryPerformanceCounter(&start_time);

    return true;
}

void orb_platform_shutdown(orb_platform_state *platform) {
    (void)platform;
    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 orb_platform_events_pump(orb_platform_state *platform) {
    (void)platform;
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return TRUE;
}

void *orb_platform_allocate(u64 size, b8 aligned) {
    (void)aligned;
    return malloc(size);
}

void orb_platform_free(void *block, b8 aligned) {
    (void)aligned;
    free(block);
}

void *orb_platform_memory_zero(void *block, u64 size) {
    return orb_platform_memory_set(block, 0, size);
}

void *orb_platform_memory_copy(void *destination, const void *source, u64 size) {
    return memcpy(destination, source, size);
}

void *orb_platform_memory_set(void *destination, i32 value, u64 size) {
    return memset(destination, value, size);
}

const u8 log_level_color[LOG_LEVEL_MAX_ENUM] = {64, 4, 6, 2, 1, 8};

void orb_platform_console_write(const char *message, u8 color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(console_handle, log_level_color[color]);

    OutputDebugStringA(message);
    usize length = strlen(message);
    LPDWORD written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, written, nullptr);
}
void orb_platform_console_write_error(const char *message, u8 color) {
    {
        HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);

        SetConsoleTextAttribute(console_handle, log_level_color[color]);

        OutputDebugStringA(message);
        usize length = strlen(message);
        LPDWORD written = 0;
        WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, written, nullptr);
    }
}

typedef struct win32_handle_info {
    HINSTANCE hinstance;
    HWND hwnd;
} win32_handle_info;

void orb_platform_get_window_handle_info(usize *out_size, void *memory) {
    *out_size = sizeof(win32_handle_info);
    if (memory == nullptr) {
        return;
    }

    win32_handle_info *out_typed = (win32_handle_info *)memory;
    out_typed->hinstance = state->h_instance;
    out_typed->hwnd = state->hwnd;
}

f64 orb_platform_time_now() {
    static f64 clock_frequency = 0.0;
    if (unlikely(clock_frequency == 0.0)) {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        clock_frequency = 1.0 / (f64)frequency.QuadPart;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return (f64)now.QuadPart * clock_frequency;
}

void orb_platform_time_sleep(u64 ms) { Sleep((DWORD)ms); }

LRESULT CALLBACK process_win32_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
    case WM_ERASEBKGND: {
        // notify windows that we will handle clearing the client area
        return 1;
    }
    case WM_CLOSE: {
        orb_event_context context = {};
        orb_event_send(ORB_EVENT_APPLICATION_QUIT, nullptr, context);
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: {
        RECT bounds;
        GetClientRect(hwnd, &bounds);
        i32 width = bounds.right - bounds.left;
        i32 height = bounds.bottom - bounds.top;

        orb_event_context context = {};
        context.data.u16[0] = (u16)width;
        context.data.u16[1] = (u16)height;
        orb_event_send(ORB_EVENT_RESIZED, nullptr, context);
    } break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP: {
        b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        orb_keyboard_keys key;

        switch (w_param) {
        case VK_MENU: {
            b8 extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;
            key = extended ? KEY_RALT : KEY_LALT;
        } break;
        case VK_SHIFT: {
            u32 left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
            u32 scancode = ((l_param & (0xFF << 16)) >> 16);
            key = scancode == left_shift ? KEY_LSHIFT : KEY_RSHIFT;
        } break;
        case VK_CONTROL: {
            b8 extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;
            key = extended ? KEY_RCONTROL : KEY_LCONTROL;
        } break;
        default:
            key = (orb_keyboard_keys)w_param;
            break;
        }

        orb_input_process_key(key, pressed);
    } break;
    case WM_MOUSEMOVE: {
        i16 x_position = (i16)GET_X_LPARAM(l_param);
        i16 y_position = (i16)GET_Y_LPARAM(l_param);

        orb_input_process_mouse_move(x_position, y_position);
    } break;
    case WM_MOUSEWHEEL: {
        i8 z_delta = (i8)GET_WHEEL_DELTA_WPARAM(w_param);

        if (z_delta != 0) {
            z_delta = (z_delta < 0) ? -1 : 1;
        }

        orb_input_process_mouse_wheel(z_delta);
    } break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN;

        orb_mouse_buttons mouse_button = MOUSE_BUTTON_MAX_BUTTONS;

        switch (msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            mouse_button = MOUSE_BUTTON_LEFT;
            break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            mouse_button = MOUSE_BUTTON_MIDDLE;
            break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            mouse_button = MOUSE_BUTTON_RIGHT;
            break;
        }

        if (mouse_button != MOUSE_BUTTON_MAX_BUTTONS) {
            orb_input_process_mouse_button(mouse_button, pressed);
        }
    } break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif
