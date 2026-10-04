// language: C++17, file: main.cpp, app: hodlcalc — staking rewards calculator
// Windows 11, MSVC, WinAPI. No external dependencies, fully offline.
// Input: principal, nominal APY, compounding cadence, horizon in years.
// Output: effective APY, compound vs simple final values, the edge.
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <string>

static HWND g_principal, g_apy, g_cadence, g_years, g_result;

static double GetDouble(HWND h) {
    wchar_t buf[64];
    GetWindowTextW(h, buf, 64);
    return _wtof(buf);
}

static void Calculate() {
    double P = GetDouble(g_principal);
    double r = GetDouble(g_apy) / 100.0;
    int n = (int)GetDouble(g_cadence);
    double y = GetDouble(g_years);
    if (P <= 0 || r <= 0 || n <= 0 || y <= 0) {
        SetWindowTextW(g_result, L"enter valid numbers in all fields");
        return;
    }
    double effApy = pow(1.0 + r / n, n) - 1.0;
    double compound = P * pow(1.0 + r / n, n * y);
    double simple = P * (1.0 + r * y);
    wchar_t buf[512];
    swprintf(buf, 512,
             L"effective APY:  %.2f%%\r\n"
             L"final (compound): %.2f\r\n"
             L"final (simple):   %.2f\r\n"
             L"compound edge:    %.2f",
             effApy * 100.0, compound, simple, compound - simple);
    SetWindowTextW(g_result, buf);
}

static LRESULT CALLBACK WndProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        HFONT f = CreateFontW(17, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        auto label = [&](const wchar_t* text, int x, int y) {
            HWND h = CreateWindowW(L"STATIC", text, WS_CHILD | WS_VISIBLE, x, y, 200, 22,
                                   w, nullptr, nullptr, nullptr);
            SendMessageW(h, WM_SETFONT, (WPARAM)f, TRUE);
        };
        auto input = [&](HWND& store, const wchar_t* def, int x, int y) {
            store = CreateWindowW(L"EDIT", def, WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  x, y, 140, 26, w, nullptr, nullptr, nullptr);
            SendMessageW(store, WM_SETFONT, (WPARAM)f, TRUE);
        };
        label(L"Staked amount", 24, 22);   input(g_principal, L"5000", 240, 20);
        label(L"Nominal APY (%)", 24, 62); input(g_apy, L"7.4", 240, 60);
        label(L"Compound per year", 24, 102); input(g_cadence, L"52", 240, 100);
        label(L"Horizon (years)", 24, 142);   input(g_years, L"2", 240, 140);
        CreateWindowW(L"BUTTON", L"Calculate", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      24, 184, 140, 34, w, (HMENU)1, nullptr, nullptr);
        g_result = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                                 24, 234, 420, 120, w, nullptr, nullptr, nullptr);
        SendMessageW(g_result, WM_SETFONT, (WPARAM)f, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == 1) Calculate();
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, LPWSTR, int show) {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc; wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(13, 17, 23));
    wc.lpszClassName = L"HodlCalcWnd";
    RegisterClassW(&wc);
    HWND w = CreateWindowExW(0, L"HodlCalcWnd", L"HodlCalc — staking rewards calculator (offline)",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
                             nullptr, nullptr, inst, nullptr);
    ShowWindow(w, show);
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0)) { TranslateMessage(&m); DispatchMessageW(&m); }
    return 0;
}
