#include <windows.h>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;

            HBRUSH hbrush = CreateSolidBrush(RGB(255, 255, 255));

            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, hbrush);
            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            NULL;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

static int create_window(char *window_name, char *window_class, int x, int y, int width, int height) {

    WNDCLASSEX wc;
    HMODULE hInstance = 0;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = window_class;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return 0;
    }
    HWND hwnd = CreateWindowEx(0, (LPCSTR) window_class, window_name,
                               WS_OVERLAPPEDWINDOW, x, y, width, height, NULL,
                               NULL, hInstance, NULL);
    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, hwnd, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
