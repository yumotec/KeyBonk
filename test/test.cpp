#include <windows.h>
#include <shellapi.h>
#include <string>

// 常量定义
#define ID_TRAY_ICON 1001
#define IDM_EXIT 1002
#define IDM_WINDOW_PENETRATE 1003
#define IDM_SETTINGS 1004
#define IDM_ABOUT 1005
#define IDM_MUTE 1006
#define WM_TRAYICON (WM_USER + 1)

// 全局变量
HINSTANCE hInst;
NOTIFYICONDATA nid = {};
HMENU hPopupMenu = NULL;
bool bWindowPenetrate = false;
bool bMuted = false;

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateTrayIcon(HWND hwnd);
void ShowContextMenu(HWND hwnd, POINT pt);
void ToggleWindowPenetrate(HWND hwnd);
void ToggleMute();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    hInst = hInstance;
    
    // 注册窗口类
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"TrayApp";
    
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"窗口注册失败!", L"错误", MB_ICONERROR);
        return 0;
    }
    
    // 创建窗口（隐藏）
    HWND hwnd = CreateWindow(
        L"TrayApp",
        L"托盘应用",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 200,
        NULL, NULL, hInstance, NULL);
    
    if (!hwnd)
    {
        MessageBox(NULL, L"窗口创建失败!", L"错误", MB_ICONERROR);
        return 0;
    }
    
    // 创建托盘图标
    CreateTrayIcon(hwnd);
    
    // 创建右键菜单
    hPopupMenu = CreatePopupMenu();
    AppendMenu(hPopupMenu, MF_STRING, IDM_EXIT, L"退出");
    AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopupMenu, MF_STRING | (bWindowPenetrate ? MF_CHECKED : MF_UNCHECKED), 
               IDM_WINDOW_PENETRATE, L"窗口穿透");
    AppendMenu(hPopupMenu, MF_STRING, IDM_SETTINGS, L"设置");
    AppendMenu(hPopupMenu, MF_STRING, IDM_ABOUT, L"关于");
    AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopupMenu, MF_STRING | (bMuted ? MF_CHECKED : MF_UNCHECKED), 
               IDM_MUTE, L"静音");
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}

// 创建托盘图标
void CreateTrayIcon(HWND hwnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInst, IDI_APPLICATION); // 可以使用自定义图标
    
    // 设置提示文本
    wcscpy_s(nid.szTip, L"托盘应用 - 右键点击显示菜单");
    
    // 添加托盘图标
    Shell_NotifyIcon(NIM_ADD, &nid);
}

// 显示上下文菜单
void ShowContextMenu(HWND hwnd, POINT pt)
{
    // 更新菜单项状态
    CheckMenuItem(hPopupMenu, IDM_WINDOW_PENETRATE, 
                  bWindowPenetrate ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hPopupMenu, IDM_MUTE, 
                  bMuted ? MF_CHECKED : MF_UNCHECKED);
    
    // 设置前台窗口以确保菜单正确显示
    SetForegroundWindow(hwnd);
    
    // 显示菜单
    TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    
    // 发送消息以确保菜单正确关闭
    PostMessage(hwnd, WM_NULL, 0, 0);
}

// 切换窗口穿透功能
void ToggleWindowPenetrate(HWND hwnd)
{
    bWindowPenetrate = !bWindowPenetrate;
    
    if (bWindowPenetrate)
    {
        // 启用窗口穿透
        SetWindowLong(hwnd, GWL_EXSTYLE, 
                     GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
        SetLayeredWindowAttributes(hwnd, 0, 128, LWA_ALPHA); // 半透明
    }
    else
    {
        // 禁用窗口穿透
        SetWindowLong(hwnd, GWL_EXSTYLE, 
                     GetWindowLong(hwnd, GWL_EXSTYLE) & ~(WS_EX_LAYERED | WS_EX_TRANSPARENT));
    }
}

// 切换静音功能
void ToggleMute()
{
    bMuted = !bMuted;
    // 这里可以添加实际的静音逻辑
    // 例如使用 waveOutSetVolume 或其他音频API
}

// 窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        break;
        
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP)
        {
            // 获取鼠标位置
            POINT pt;
            GetCursorPos(&pt);
            ShowContextMenu(hwnd, pt);
        }
        else if (lParam == WM_LBUTTONDBLCLK)
        {
            // 双击托盘图标显示关于信息
            MessageBox(hwnd, L"托盘应用示例\n使用Win32 API实现", L"关于", MB_OK | MB_ICONINFORMATION);
        }
        break;
        
    case WM_CONTEXTMENU:
        {
            // 窗口右键菜单
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ShowContextMenu(hwnd, pt);
        }
        break;
        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
            
        case IDM_WINDOW_PENETRATE:
            ToggleWindowPenetrate(hwnd);
            break;
            
        case IDM_SETTINGS:
            MessageBox(hwnd, L"设置功能", L"设置", MB_OK | MB_ICONINFORMATION);
            break;
            
        case IDM_ABOUT:
            MessageBox(hwnd, L"托盘应用示例\n使用Win32 API实现右键菜单和托盘图标", 
                       L"关于", MB_OK | MB_ICONINFORMATION);
            break;
            
        case IDM_MUTE:
            ToggleMute();
            break;
        }
        break;
        
    case WM_DESTROY:
        // 移除托盘图标
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}