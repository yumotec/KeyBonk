#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <wmp.h>
#include <mmsystem.h>
#include <digitalv.h>

#pragma comment(lib, "winmm.lib")


// 向前声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 主程序
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    // 注册窗口类
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

	// 初始化COM
	CoInitialize(NULL);
	
    // 安装钩子
    HHOOK KeyboardHook = NULL;
    KeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandle(NULL),
        0
    );

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,                              // 可选的窗口风格
        CLASS_NAME,                     // 窗口类
        L"Learn to Program Windows",    // 窗口文本
        WS_OVERLAPPEDWINDOW,            // 窗口风格
		// 大小+位置
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // 父窗口   
        NULL,       // 菜单
        hInstance,  // 示例句柄
        NULL        // 附带的软件数据
		);

    if (hwnd == NULL){
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// 消息处理
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				// 所有绘图操作发生在这里也就是BeginPaint和EndPaint之间
				FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
				EndPaint(hwnd, &ps);
			}
			return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;
        // 判断是否为按键按下事件
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // 检查特定的虚拟键码，例如 F1
            if (pKeyInfo->vkCode == VK_F1) {
                // 触发你的特定行为，例如播放音效
                PlaySound(TEXT("trigger.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
        }
    }
    // 将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}