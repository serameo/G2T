/*
File: g2t.c
Purpose: Display Graphic UI as Text UI
Author: Seree Rakwong
Date: 23-SEP-2019
 */
#include <stdlib.h>
#include "g2t.h"
#include "g2tprop.h"

#define G2T_ENVWNDCLS          "G2T_ENVWNDCLS"

struct _g2t_wndproc_t
{
    g2t_wndproc wndproc;
    g2t_string clsname;
    struct _g2t_wndproc_t* prev;
    struct _g2t_wndproc_t* next;
};
typedef struct _g2t_wndproc_t g2t_wndproc_t;

struct _g2t_msgq_t
{
    g2t_wnd wnd;
    g2t_uint32 msg;
    g2t_wparam wparam;
    g2t_lparam lparam;

    struct _g2t_msgq_t *next;
};
typedef struct _g2t_msgq_t g2t_msgq_t;

struct _g2t_env
{
    /* procedure registered */
    g2t_wndproc_t* firstproc;
    g2t_wndproc_t* lastproc;
    /* messages in queue */
    g2t_msgq_t* headq;
    g2t_msgq_t* tailq;
    g2t_lockable lockedq;
    /* windows */
    g2t_wnd firstwnd;
    g2t_wnd lastwnd;
    g2t_wnd activewnd;
    g2t_accel* accel;
    /* window message */
    HINSTANCE hinst;
    HWND hMainWnd;
    HWND hwnd;
    HDC hdc;
    HDC hmemdc;
    HBITMAP hbmp;
    TEXTMETRIC tm;
    HFONT hfont;
    g2t_int cxChar;
    g2t_int cxCaps;
    g2t_int cyChar;
    g2t_int cyLine;
    g2t_wndproc prewndproc; /* to translate other messages */
    g2t_void* args; /* to pass though wndproc */
    safeprop_t* props;
};

struct _g2t_wnd
{
    g2t_string clsname;
    g2t_char wndname[G2T_MAX_WNDTEXT + 1];
    g2t_dword style;
    g2t_dword exstyle;
    g2t_int x;
    g2t_int y;
    g2t_int cols;
    g2t_int lines;
    g2t_wnd parent;
    g2t_uint32 id;
    g2t_lparam lparam;
    g2t_wndproc wndproc;
    g2t_bool enable;
    g2t_bool visible;
    g2t_validateproc validateproc;
    g2t_char infotext[G2T_MAX_WNDTEXT + 1];
    g2t_dword textcolor;
    g2t_dword bgcolor;
    g2t_bool lockedupdate;
    /* links */
    g2t_wnd prevwnd;
    g2t_wnd nextwnd;
    g2t_wnd firstchild;
    g2t_wnd lastchild;
    g2t_wnd activechild;
};
LRESULT _g2t_EnvWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT g2t_MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

g2t_wnd _g2t_FindMnemonic(g2t_wnd, g2t_char);

g2t_wnd _g2t_FindMnemonic(g2t_wnd wnd, g2t_char ch)
{
    g2t_char buf[G2T_MAX_WNDTEXT+1];
    g2t_char mnemonic[3] = { '&', ch, 0 };
    g2t_dword style = 0;
    g2t_wnd childwnd = (wnd ? wnd->firstchild : 0);

    while (childwnd)
    {
        g2t_GetClassName(childwnd, buf, G2T_MAX_WNDTEXT);
        if (0 == strcmp(buf, G2T_STATIC))
        {
            g2t_GetWndText(childwnd, buf, G2T_MAX_WNDTEXT);
            if (strstr(buf, mnemonic))
            {
                break;
            }
        }
        childwnd = childwnd->nextwnd;
    }
    return childwnd;
}

/* global environment */
g2t_env genvptr = 0;

/*typedef g2t_int32(*g2t_wndproc)(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);*/
g2t_long G2TSTATICPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TEDITBOXPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TLISTBOXPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TBUTTONPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TLISTCTRLPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TTREECTRLPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_long G2TTSCHARTPROC(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);

/* helper functions */
g2t_wndproc_t* _g2t_FindWndProc(g2t_env env, g2t_string clsname)
{
    g2t_wndproc_t* proc = env->firstproc;
    while (proc)
    {
        if (strcmp(proc->clsname, clsname) == 0)
        {
            break;
        }
        proc = proc->next;
    }
    return proc;
}

g2t_wndproc g2t_SetPreTranslateMsg(g2t_wndproc newproc, g2t_void* args)
{
    g2t_env env = g2t_GetEnv();
    g2t_wndproc oldproc = env->prewndproc;
    env->prewndproc = newproc;
    env->args = args;
    return oldproc;
}

/*
g2t_int32 g2t_StartUp();
    Start g2t environment
 */
g2t_status g2t_StartUp(HINSTANCE hinst)
{
    g2t_status rc = G2T_OK;
    WNDCLASS wc;
    struct _g2t_env* env = (struct _g2t_env*) g2t_malloc(sizeof (struct _g2t_env));

    if (!env)
    {
        rc = G2T_ENOMEM;
        return rc;
    }
    /* allocate environment */
    genvptr = env;
    env->props = CreateSafeProp(0);

    /* register standard controls */
    g2t_RegisterCls(G2T_STATIC, G2TSTATICPROC);
    g2t_RegisterCls(G2T_EDITBOX, G2TEDITBOXPROC);
    g2t_RegisterCls(G2T_LISTBOX, G2TLISTBOXPROC);
    g2t_RegisterCls(G2T_LISTCTRL, G2TLISTCTRLPROC);
    g2t_RegisterCls(G2T_TREECTRL, G2TTREECTRLPROC);

    env->hinst = hinst;
    rc = g2t_InitLockedObject(&env->lockedq);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) _g2t_EnvWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = env->hinst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = G2T_ENVWNDCLS;

    RegisterClass(&wc);
    return rc;
}

g2t_status g2t_InitApplication(g2t_string szClsName, g2t_string szTitle)
{
    g2t_env env = g2t_GetEnv();
    g2t_status rc = G2T_OK;
    WNDCLASS wndclass;
    HWND hwnd;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = (WNDPROC)g2t_MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = env->hinst;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szClsName;
    if (!RegisterClass(&wndclass))
    {
        rc = G2T_ERROR;
        return rc;
    }
    hwnd = CreateWindow(szClsName, // window class name
                        szTitle, // window caption
                        WS_OVERLAPPEDWINDOW, // window style
                        CW_USEDEFAULT, // initial x position
                        CW_USEDEFAULT, // initial y position
                        CW_USEDEFAULT, // initial x size
                        CW_USEDEFAULT, // initial y size
                        NULL, // parent window handle
                        NULL, // window menu handle
                        env->hinst, // program instance handle
                        NULL); // creation parameters
    if (!hwnd)    
    {
        rc = G2T_ERROR;
        return rc;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    env->hMainWnd = hwnd;
    return rc;
}

g2t_long   g2t_Run()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

g2t_status g2t_LoadScreen(g2t_string scrproc, g2t_string scrname, g2t_frmwndtemplate* templ)
{
    g2t_wnd wnd = g2t_CreateFrameWnd(
        scrproc, scrname,
        GWS_WINDOW | GWS_VISIBLE, 0,
        0, 0, 25, 80,
        templ,
        0);
    if (!wnd)
    {
        return -1;
    }
    return g2t_ShowWnd(wnd, GW_SHOW);
}

LRESULT g2t_MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (msg)
    {
        case WM_CREATE:
            g2t_AllocateEnvWnd(0, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, 1);
            return 0;

        case WM_DESTROY:
            g2t_ReleaseEnvWnd();
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            GetClientRect(hwnd, &rect);
            g2t_MoveEnvWnd(rect.left, rect.top, rect.right, rect.bottom);
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            SetForegroundWindow(hwnd);
            return 0;

    }
    if (G2T_BYPASS != g2t_GetMsgWndProc(msg, wparam, lparam))
    {
        return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

g2t_status g2t_LoadProperties(g2t_string filename)
{
    g2t_env env = g2t_GetEnv();
    g2t_status rc = G2T_ERROR;
    FILE* fp = 0;
    errno_t en = fopen_s(&fp, filename, "r");
    if (fp)
    {
        if (SPROP_OK == env->props->LoadFromFile(env->props, fp, 0, 0))
        {
            rc = G2T_OK;
        }
        fclose(fp);
    }
    return rc;
}

g2t_dword  g2t_GetReverseColor(g2t_dword color)
{
    g2t_byte red = 0xff & (color >> 16);
    g2t_byte green = 0xff & (color >> 8);
    g2t_byte blue = 0xff & color;

    red = 0xff & labs(255 - red);
    green = 0xff & labs(255 - green);
    blue = 0xff & labs(255 - blue);
    return RGB(red, green, blue);
}
g2t_dword g2t_GetSysColor(g2t_string sysname)
{
    g2t_dword color = G2T_RGB_BLACK;
    g2t_env env = g2t_GetEnv();
    g2t_char val[G2T_MAX_WNDTEXT + 1];
    g2t_int red = 255;
    g2t_int green = 255;
    g2t_int blue = 255;

    env->props->GetString(env->props, sysname, val, "255,255,255");
    sscanf_s(val, "%d,%d,%d", &red, &green, &blue);
    red = G2T_MAX(0, G2T_MIN(255, red));
    green = G2T_MAX(0, G2T_MIN(255, green));
    blue = G2T_MAX(0, G2T_MIN(255, blue));

    color = RGB(red, green, blue);
    return color;
}

g2t_dword g2t_GetSysTextColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_TEXTCOLOR);
    return color;
}

g2t_dword g2t_GetSysBgColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_BGCOLOR);
    return color;
}

g2t_dword g2t_GetSysDisabledTextColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_DISABLED_TEXTCOLOR);
    return color;
}

g2t_dword g2t_GetSysDisabledBgColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_DISABLED_BGCOLOR);
    return color;
}

g2t_dword g2t_GetSysHighlightedTextColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_HIGHLIGHTED_TEXTCOLOR);
    return color;
}

g2t_dword g2t_GetSysHighlightedBgColor()
{
    g2t_dword color = g2t_GetSysColor(ENV_HIGHLIGHTED_BGCOLOR);
    return color;
}

HWND g2t_GetHwnd()
{
    g2t_env env = g2t_GetEnv();
    return env->hwnd;
}
g2t_void g2t_ReleaseEnvWnd()
{
    g2t_env env = g2t_GetEnv();
    if (env->hwnd)
    {
        g2t_wnd wnd = env->firstwnd;
        g2t_wnd tempwnd = wnd;
        while (wnd)
        {
            wnd = wnd->nextwnd;
            g2t_DestroyWnd(tempwnd);
            tempwnd = wnd;
        }
        env->firstwnd = env->lastwnd = 0;
        DeleteObject(env->hfont);
        /*DeleteDC(env->hdc);*/
        DeleteDC(env->hmemdc);
        ReleaseDC(env->hwnd, env->hdc);
        g2t_ShowCaret(GW_HIDE);
        g2t_DelCaret();
        DestroyWindow(env->hwnd);
        env->hwnd = 0;
        env->hdc = 0;
        env->hfont = 0;
    }
}

g2t_status g2t_AllocateEnvWnd(
                              g2t_dword dwExStyle,
                              g2t_dword dwStyle,
                              g2t_int x,
                              g2t_int y,
                              g2t_int cx,
                              g2t_int cy,
                              HWND hwndParent,
                              UINT nID)
{
    g2t_status rc = G2T_OK;
    g2t_env env = g2t_GetEnv();
    g2t_char fontname[G2T_BUFSIZ] = "";
    g2t_long hspaces = 0;

    HWND hwnd = CreateWindowEx(
                               dwExStyle, G2T_ENVWNDCLS, G2T_ENVWNDCLS, dwStyle,
                               x, y, cx, cy, hwndParent,
                               (HMENU) nID, env->hinst,
                               NULL);
    if (!hwnd)
    {
        DWORD dw = GetLastError();
        rc = G2T_ERROR;
        return rc;
    }
    env->hwnd = hwnd;
    env->hdc = GetDC(hwnd);
    env->hmemdc = CreateCompatibleDC(env->hdc);
    env->hbmp = CreateCompatibleBitmap(env->hdc, 
        GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    SelectObject(env->hmemdc, env->hbmp);

    env->props->GetString(env->props, ENV_FONT, fontname, GF_CONSOLAS);
    env->hfont = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                            FF_DONTCARE|FIXED_PITCH,
                            fontname);
    SelectObject(env->hdc, env->hfont);
    SelectObject(env->hmemdc, env->hfont);

    GetTextMetrics(env->hdc, &env->tm);
    env->cxChar = env->tm.tmAveCharWidth;
    env->cxCaps = (env->tm.tmPitchAndFamily & 1 ? 3 : 2) * env->cxChar / 2;
    env->cyChar = env->tm.tmHeight + env->tm.tmExternalLeading;

    hspaces = 0;
    env->props->GetInt(env->props, ENV_FONT_HSPACES, &hspaces, G2T_HTEXTSPACES);
    env->cyLine = env->cyChar + hspaces;

    g2t_NewCaret();
    g2t_ShowCaret(GW_SHOW);

    return rc;
}

g2t_long g2t_LoadAccel(g2t_accel* accel)
{
    g2t_env env = g2t_GetEnv();
    if (accel)
    {
        env->accel = accel;
    }
    return 0;
}

g2t_long _g2t_DequeMsg()
{
    g2t_env env = g2t_GetEnv();
    g2t_msgq_t* msgq = 0;
    /* deque */
    while (env && env->headq)
    {
        msgq = env->headq;

        g2t_Lock(&env->lockedq);
        env->headq = env->headq->next;
        g2t_UnLock(&env->lockedq);

        g2t_SendMsg(msgq->wnd,
                    msgq->msg,
                    msgq->wparam,
                    msgq->lparam);

        msgq->next = 0;
        g2t_free(msgq);
        g2t_Lock(&env->lockedq);
        if (!env->tailq)
        {
            env->tailq = env->headq = 0;
        }
        g2t_UnLock(&env->lockedq);
        /* pause thread */
        /*TuiSetTimerEvent(TWND_DUMMY, TUI_DEQUEUE_ID, TTIMER_SUSPEND);*/
    }
    return G2T_OK;
}

g2t_long _g2t_DispatchMsg(g2t_msg* msg)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = env->activewnd;
    g2t_wnd child = (wnd ? wnd->activechild : 0);

    if (child)
    {
        msg->wnd = child;
    }
    else if (wnd)
    {
        child = g2t_GetFirstActiveChildWnd(wnd);
        if (child)
        {
            msg->wnd = child;
        }
        else
        {
            msg->wnd = wnd;
        }
    }

    /* deque message */
    _g2t_DequeMsg();

    return G2T_OK;
}
#if 0
g2t_status _g2t_TranslateMsg(g2t_msg* msg)
{
    g2t_env env = g2t_GetEnv();
    g2t_int ch = (g2t_int) msg->wparam;
    g2t_long rc = 0;
    g2t_wnd nextwnd = 0;
    g2t_wnd prevwnd = 0;
    g2t_wnd parent = 0;
    g2t_wnd grandparent = 0;
    g2t_wnd nextchild = 0;
    g2t_rect rcwnd;
    g2t_int i;

    /* send message to the current active window */
    if (!msg->wnd)
    {
        return G2T_CONTINUE;
    }

    /* convert keyboard code to message */

    parent = g2t_GetParent(msg->wnd);
    if (ch == VK_RETURN)
    {
        /* kill focus the current active window */
        nextwnd = g2t_GetNextActiveChildWnd(parent);
        nextchild = g2t_GetNextActiveChildWnd(msg->wnd);
        if (nextchild)
        {
            rc = g2t_SendMsg(msg->wnd, GWM_KILLFOCUS, 0, (g2t_lparam) nextchild);
        }
        else
        {
            rc = g2t_SendMsg(msg->wnd, GWM_KILLFOCUS, 0, (g2t_lparam) nextwnd);
        }

        if (rc != G2T_CONTINUE)
        {
            /* move cursor */
            g2t_GetWndRect(msg->wnd, &rcwnd);
            g2t_MoveXY(rcwnd.x, rcwnd.y);
            return rc;
        }

        /* set focus the new active window */
        prevwnd = msg->wnd;
        if (parent)
        {
            if (msg->wnd == g2t_GetLastActiveChildWnd(parent))
            {
                grandparent = g2t_GetParent(parent);
                if (grandparent)
                {
                    msg->wnd = g2t_GetNextActiveChildWnd(grandparent);
                }
                else
                {
                    msg->wnd = g2t_GetFirstActiveChildWnd(parent);
                }
            }
            else
            {
                msg->wnd = nextwnd;
            }
            parent->activechild = msg->wnd;
        }
        else
        {
            msg->wnd = g2t_GetPrevActiveChildWnd(env->activewnd);
            env->activewnd->activechild = msg->wnd;
        }

        if (msg->wnd)
        {
            rc = g2t_SendMsg(msg->wnd, GWM_SETFOCUS, 0, (g2t_lparam) prevwnd);
            g2t_MoveXY(msg->wnd->x, msg->wnd->y);
        }
        return 0;
    }
    else
    {
        /* otherwise key */
        /* check if the pressed key has been loaded into the accelerator table */
        if (env->accel)
        {
            for (i = 0; env->accel[i].cmd != 0; ++i)
            {
                if (env->accel[i].vkey == ch)
                {
                    g2t_SendMsg(g2t_GetFrameWnd(msg->wnd),
                                GWM_COMMAND,
                                env->accel[i].cmd, 0);
                    return 0;
                }
            }
        }
    }

    /* parent window */
    rc = g2t_SendMsg(env->activewnd, GWM_KEYDOWN, msg->wparam, 0);
    rc = g2t_SendMsg(env->activewnd, GWM_CHAR, msg->wparam, 0);
    rc = g2t_SendMsg(env->activewnd, GWM_KEYUP, msg->wparam, 0);

    if (msg->wnd != env->activewnd)
    {
        rc = g2t_SendMsg(msg->wnd, GWM_KEYDOWN, msg->wparam, 0);
        rc = g2t_SendMsg(msg->wnd, GWM_CHAR, msg->wparam, 0);
        rc = g2t_SendMsg(msg->wnd, GWM_KEYUP, msg->wparam, 0);
    }

    return 0;
}
#endif

g2t_void _g2t_EnvWndProc_OnKeyDown(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnKeyUp(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnChar(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnPaint(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnCommand(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnTimer(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnMsgPost(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnNetMessage(HWND hwnd, WPARAM wparam, LPARAM lparam);
g2t_void _g2t_EnvWndProc_OnScrRefresh(HWND hwnd, WPARAM wparam, LPARAM lparam);

g2t_long g2t_GetMsgWndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
    return _g2t_EnvWndProc(env->hwnd, msg, wparam, lparam);
}

LRESULT _g2t_EnvWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    g2t_status rc = G2T_FALSE;
    g2t_env env = g2t_GetEnv();

    /* to translate message before doing default messages */
    if (env->prewndproc)
    {
        rc = env->prewndproc(env->activewnd,
                             (g2t_uint32) msg,
                             (g2t_wparam) wparam,
                             (g2t_lparam) lparam);
        if (G2T_TRUE == rc)
        {
            return 0;
        }
    }
    switch (msg)
    {
    case WM_KEYDOWN:
        _g2t_EnvWndProc_OnKeyDown(hwnd, wparam, lparam);
        return 0;
    case WM_CHAR:
        _g2t_EnvWndProc_OnChar(hwnd, wparam, lparam);
        return 0;
    case WM_KEYUP:
        _g2t_EnvWndProc_OnKeyUp(hwnd, wparam, lparam);
        break;
    case WM_PAINT:
        _g2t_EnvWndProc_OnPaint(hwnd, wparam, lparam);
        return 0;
    case WM_COMMAND:
        _g2t_EnvWndProc_OnCommand(hwnd, wparam, lparam);
        return 0;
    case WM_TIMER:
        _g2t_EnvWndProc_OnTimer(hwnd, wparam, lparam);
        return 0;

    case WM_SETFOCUS:
        /*g2t_NewCaret();*/
        /*g2t_SetCaretPos(0, 0);*/
        g2t_ShowCaret(GW_SHOW);
        return 0;

    case WM_KILLFOCUS:
        g2t_ShowCaret(GW_HIDE);
        /*g2t_DelCaret();*/
        return 0;

    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    {
        g2t_wnd activewnd = g2t_GetActiveWnd();
        g2t_wnd activechild = g2t_GetActiveChildWnd(activewnd);
        g2t_SetFocus(activechild);
        return 0;
    }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            SetForegroundWindow(hwnd);
            return 0;

    case GWM_MSGPOST:
        _g2t_EnvWndProc_OnMsgPost(hwnd, wparam, lparam);
        return 0;

    case GWM_NETMESSAGE:
        _g2t_EnvWndProc_OnNetMessage(hwnd, wparam, lparam);
        return 0;

    case GWM_SCREENREFRESH:
        _g2t_EnvWndProc_OnScrRefresh(hwnd, wparam, lparam);
        return 0;
    }
    /*return DefWindowProc(hwnd, msg, wparam, lparam);*/
    return G2T_BYPASS;
}

g2t_void _g2t_EnvWndProc_OnMsgPost(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    /*TODO: translate message and process it*/
    g2t_msgq_t* msgq = (g2t_msgq_t*)lparam;
    if (msgq)
    {
        g2t_SendMsg(msgq->wnd, msgq->msg, msgq->wparam, msgq->lparam);
        g2t_free(msgq);
    }
}


g2t_void _g2t_EnvWndProc_OnScrRefresh(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    /*TODO: translate message and process it*/
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = g2t_GetActiveWnd();
    g2t_SendMsg(wnd, GWM_SCREENREFRESH, (g2t_wparam)wparam, (g2t_lparam)lparam);
}

g2t_void _g2t_EnvWndProc_OnNetMessage(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    /*TODO: translate message and process it*/
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = g2t_GetActiveWnd();
    g2t_SendMsg(wnd, GWM_NETMESSAGE, (g2t_wparam)wparam, (g2t_lparam)lparam);
}

g2t_void _g2t_EnvWndProc_OnKeyDown(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = env->activewnd;
    g2t_long rc = G2T_CONTINUE;
    g2t_wnd child = (wnd ? wnd->activechild : 0);
    g2t_wnd parent = 0;
    g2t_wnd nextwnd = 0;
    g2t_wnd nextchild = 0;
    g2t_wnd prevwnd = 0;
    g2t_wnd prevchild = 0;
    g2t_rect rect;

    if (child)
    {
        wnd = child;
    }
    else if (wnd)
    {
        child = g2t_GetFirstActiveChildWnd(wnd);
        wnd = (child ? child : wnd);
    }

    if (VK_RETURN == wparam)
    {
        /* move to the previous window */
        if (GetAsyncKeyState(VK_SHIFT))
        {
            parent = g2t_GetParent(wnd);
            prevwnd = g2t_GetPrevActiveChildWnd(parent);
            prevchild = g2t_GetPrevActiveChildWnd(wnd);
            if (prevchild)
            {
                rc = g2t_SendMsg(wnd, GWM_KILLFOCUS, 0, (g2t_lparam) prevchild);
            }
            else
            {
                rc = g2t_SendMsg(wnd, GWM_KILLFOCUS, 0, (g2t_lparam) prevwnd);
            }
            if (rc != G2T_CONTINUE)
            {
                /* DO NOT MOVE to another window */
                g2t_GetWndRect(wnd, &rect);
                g2t_MoveXY(rect.x, rect.y);
                return;
            }
            nextwnd = wnd;
            if (parent)
            {
                if (wnd == g2t_GetFirstActiveChildWnd(parent))
                {
                    g2t_wnd grandparent = g2t_GetParent(parent);
                    if (grandparent)
                    {
                        wnd = g2t_GetPrevActiveChildWnd(grandparent);
                    }
                    else
                    {
                        wnd = g2t_GetLastActiveChildWnd(parent);
                    }
                }
                else
                {
                    wnd = prevwnd;
                }
                parent->activechild = wnd;
            }
            else
            {
                wnd = g2t_GetNextActiveChildWnd(env->activewnd);
                env->activewnd->activechild = wnd;
            }
            if (wnd)
            {
                rc = g2t_SendMsg(wnd, GWM_SETFOCUS, 0, (g2t_lparam) nextwnd);
                g2t_MoveXY(wnd->x, wnd->y);
            }
            
            return;
        }
        else /*no shift pressed */
        {
            parent = g2t_GetParent(wnd);
            nextwnd = g2t_GetNextActiveChildWnd(parent);
            nextchild = g2t_GetNextActiveChildWnd(wnd);
            if (nextchild)
            {
                rc = g2t_SendMsg(wnd, GWM_KILLFOCUS, 0, (g2t_lparam) nextchild);
            }
            else
            {
                rc = g2t_SendMsg(wnd, GWM_KILLFOCUS, 0, (g2t_lparam) nextwnd);
            }
            if (rc != G2T_CONTINUE)
            {
                /* DO NOT MOVE to another window */
                g2t_GetWndRect(wnd, &rect);
                g2t_MoveXY(rect.x, rect.y);
                return;
            }
            prevwnd = wnd;
            if (parent)
            {
                if (wnd == g2t_GetLastActiveChildWnd(parent))
                {
                    g2t_wnd grandparent = g2t_GetParent(parent);
                    if (grandparent)
                    {
                        wnd = g2t_GetNextActiveChildWnd(grandparent);
                    }
                    else
                    {
                        wnd = g2t_GetFirstActiveChildWnd(parent);
                    }
                }
                else
                {
                    wnd = nextwnd;
                }
                parent->activechild = wnd;
            }
            else
            {
                wnd = g2t_GetPrevActiveChildWnd(env->activewnd);
                env->activewnd->activechild = wnd;
            }
            if (wnd)
            {
                rc = g2t_SendMsg(wnd, GWM_SETFOCUS, 0, (g2t_lparam) prevwnd);
                g2t_MoveXY(wnd->x, wnd->y);
            }
            return;
        }
    }
    else
    {
        if ((wparam >= '0' && wparam <= '9') ||
            (wparam >= 'A' && wparam <= 'Z') ||
            (wparam >= 'a' && wparam <= 'z'))
        {
            /* mnemomic to static text */
            if (GetAsyncKeyState(VK_CONTROL))
            {
                g2t_wnd childwnd = _g2t_FindMnemonic(
                                        g2t_GetFrameWnd(env->activewnd),
                                        (g2t_char)wparam);
                if (childwnd)
                {
                    childwnd = g2t_GetNextActiveWnd(childwnd);
                    rc = g2t_SendMsg(
                            g2t_GetActiveChildWnd(env->activewnd),
                            GWM_KILLFOCUS, 0, (g2t_lparam) childwnd);
                    if (rc != G2T_CONTINUE)
                    {
                        /* DO NOT MOVE to another window */
                        g2t_GetWndRect(wnd, &rect);
                        g2t_MoveXY(rect.x, rect.y);
                    }
                    g2t_SetFocus(childwnd);
                    /*rc = g2t_SendMsg(childwnd, GWM_SETFOCUS, 0, (g2t_lparam) env->activewnd);
                    g2t_MoveXY(childwnd->x, childwnd->y);*/
                }
            }
        }
    }
    g2t_SendMsg(wnd, GWM_KEYDOWN, (g2t_wparam) wparam, (g2t_lparam) lparam);
}

g2t_void _g2t_EnvWndProc_OnKeyUp(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = env->activewnd;
    g2t_SendMsg(wnd, GWM_KEYUP, (g2t_wparam) wparam, (g2t_lparam) lparam);
}

g2t_void _g2t_EnvWndProc_OnChar(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = env->activewnd;
    g2t_long rc = G2T_CONTINUE;
    g2t_wnd child = (wnd ? wnd->activechild : 0);

    if (child)
    {
        wnd = child;
    }
    else if (wnd)
    {
        child = g2t_GetFirstActiveChildWnd(wnd);
        wnd = (child ? child : wnd);
    }

    g2t_SendMsg(env->activewnd, GWM_CHAR, (g2t_wparam) wparam, (g2t_lparam) lparam);
    if (wnd != env->activewnd)
    {
        g2t_SendMsg(wnd, GWM_CHAR, (g2t_wparam) wparam, (g2t_lparam) lparam);
    }

}

g2t_void _g2t_EnvWndProc_OnPaint(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = g2t_GetFirstWnd();
    RECT rc;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    if (wnd && G2T_FALSE == wnd->lockedupdate)
    {
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));
        while (wnd)
        {
            g2t_InvalidateWnd(wnd);
            wnd = g2t_GetNextWnd(wnd);
        }
    }

    EndPaint(hwnd, &ps);
}


g2t_void _g2t_EnvWndProc_OnCommand(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();
}

g2t_void _g2t_EnvWndProc_OnTimer(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    g2t_env env = g2t_GetEnv();

}

g2t_wnd g2t_GetActiveWnd()
{
    g2t_env env = g2t_GetEnv();
    return env->activewnd;
}

g2t_wnd g2t_GetFirstWnd()
{
    g2t_env env = g2t_GetEnv();
    return env->firstwnd;
}

g2t_wnd g2t_GetLastWnd()
{
    g2t_env env = g2t_GetEnv();
    return env->lastwnd;
}

g2t_wnd g2t_GetFirstChildWnd(g2t_wnd wnd)
{
    return (wnd ? wnd->firstchild : 0);
}

g2t_wnd g2t_GetLastChildWnd(g2t_wnd wnd)
{
    return (wnd ? wnd->lastchild : 0);
}

g2t_wnd g2t_GetNextWnd(g2t_wnd wnd)
{
    return (wnd ? wnd->nextwnd : 0);
}

g2t_wnd g2t_GetPrevWnd(g2t_wnd wnd)
{
    return (wnd ? wnd->prevwnd : 0);
}

g2t_wnd g2t_GetFrameWnd(g2t_wnd wnd)
{
    g2t_wnd frame = wnd;
    g2t_dword style = g2t_GetWndStyle(frame);

    while (frame && !(style & GWS_WINDOW))
    {
        frame = g2t_GetParent(frame);
        style = g2t_GetWndStyle(frame);
    }
    return frame;
}

/*
g2t_void g2t_ShutDown();
    Shut down g2t environment
 */
g2t_void g2t_ShutDown()
{
    g2t_env env = g2t_GetEnv();
    DestroySafeProp(env->props);
    g2t_ReleaseLockedObject(&env->lockedq);
}

/*
g2t_env g2t_GetEnv();
    Get g2t environment
 */
g2t_env g2t_GetEnv()
{
    return genvptr;
}

g2t_bool g2t_NewCaret()
{
    g2t_env env = g2t_GetEnv();
    return (g2t_bool) CreateCaret(env->hwnd, NULL, env->cxChar, env->cyChar);
}

g2t_bool g2t_SetCaretPos(g2t_int x, g2t_int y)
{
    return (g2t_bool) SetCaretPos(x, y);
}

g2t_bool g2t_ShowCaret(g2t_bool show)
{
    g2t_env env = g2t_GetEnv();
    return (g2t_bool) (GW_HIDE == show ? HideCaret(env->hwnd) : ShowCaret(env->hwnd));
}

g2t_bool g2t_DelCaret()
{
    return (g2t_bool) DestroyCaret();
}

HWND g2t_GetEnvWnd()
{
    g2t_env env = g2t_GetEnv();
    return env->hwnd;
}

g2t_void g2t_MoveEnvWnd(g2t_int x, g2t_int y, g2t_int cx, g2t_int cy)
{
    g2t_env env = g2t_GetEnv();
    MoveWindow(env->hwnd, x, y, cx, cy, TRUE);
    /* calculate screen dimension */
}

/*
 * g2t_RegisterCls()
 *   All window classes MUST be registered before creating its window instance
 *   clsname - a unique name in an application
 *   wndproc - window procedure
 */
g2t_status g2t_RegisterCls(g2t_string clsname, g2t_wndproc wndproc)
{
    g2t_status rc = G2T_OK;
    g2t_env env = g2t_GetEnv();
    g2t_wndproc_t* proc = _g2t_FindWndProc(env, clsname);

    if (proc)
    {
        /* already registered */
        return rc;
    }
    proc = (g2t_wndproc_t*) g2t_malloc(sizeof (g2t_wndproc_t));
    proc->clsname = clsname;
    proc->wndproc = wndproc;
    proc->prev = 0;
    proc->next = 0;
    if (env->firstproc)
    {
        proc->prev = env->lastproc;
        env->lastproc->next = proc;
        env->lastproc = proc;
    }
    else
    {
        env->firstproc = env->lastproc = proc;
    }
    return rc;
}

g2t_status g2t_SendMsg(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    if (wnd)
    {
        return wnd->wndproc(wnd, msg, wparam, lparam);
    }
    return G2T_ERROR;
}

g2t_status g2t_PostMsg(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    g2t_long size = 0;
    if (wnd)
    {
        g2t_msgq_t* msgq = (g2t_msgq_t*) g2t_malloc(sizeof (g2t_msgq_t));
        if (msgq)
        {
            g2t_env env = g2t_GetEnv();

            g2t_memset((g2t_buffer) msgq, 0, sizeof (g2t_msgq_t));
            msgq->wnd = wnd;
            msgq->msg = msg;
            msgq->wparam = wparam;
            if (GWM_NOTIFY == msg)
            {
                g2t_nhdr* nmhdr = (g2t_nhdr*) g2t_malloc(G2T_MAX_NMHDR);
                g2t_memcpy((g2t_buffer) nmhdr, (g2t_nhdr*) lparam, G2T_MAX_NMHDR);

                msgq->lparam = (g2t_lparam) nmhdr;
            }
            else if (GWM_SETCURSOR == msg)
            {
                g2t_pos* pos = 0;
                size = sizeof (g2t_pos);

                pos = (g2t_pos*) g2t_malloc(size);
                g2t_memcpy(pos, (g2t_pos*) lparam, size);

                msgq->lparam = (g2t_lparam) pos;
            }
            else
            {
                msgq->lparam = lparam;
            }
            /*
            g2t_Lock(&env->lockedq);
            if (env->tailq)
            {
                env->tailq->next = msgq;
                env->tailq = msgq;
            }
            else
            {
                env->headq = env->tailq = msgq;
            }
            g2t_UnLock(&env->lockedq);
             */
            PostMessage(env->hwnd, GWM_MSGPOST, 0, (LPARAM) msgq);
            return G2T_OK;
        }
    }
    return G2T_ERROR;
}

g2t_uint32 g2t_strlen(g2t_string str)
{
    return strlen(str);
}

g2t_string g2t_strcpy(g2t_string dst, g2t_string src)
{
    return (g2t_string) strcpy(dst, src);
}

g2t_int g2t_strcmp(g2t_string dst, g2t_string src)
{
    return strcmp(dst, src);
}

g2t_string g2t_strnew(g2t_uint32 len)
{
    g2t_string str = 0;
    if (len > 0)
    {
        str = (g2t_string) g2t_malloc(len);
        if (str)
        {
            g2t_memset(str, 0, len);
        }
    }
    return str;
}

g2t_void g2t_strdel(g2t_string str)
{
    g2t_free((g2t_buffer) str);
}

g2t_string g2t_strcat(g2t_string dst, g2t_string src)
{
    return (g2t_string) strcat(dst, src);
}

g2t_string g2t_strncpy(g2t_string dst, g2t_string src, g2t_uint32 len)
{
    return strncpy(dst, src, len);
}

g2t_buffer g2t_malloc(g2t_uint32 len)
{
    g2t_buffer buf = 0;
    if (len > 0)
    {
        buf = (g2t_buffer) malloc(len);
        if (buf)
        {
            g2t_memset(buf, 0, len);
        }
    }
    return buf;
}

g2t_void g2t_free(g2t_buffer buf)
{
    if (buf)
    {
        free(buf);
        buf = 0;
    }
}

g2t_status g2t_memcpy(g2t_buffer dst, g2t_buffer src, g2t_uint32 len)
{
    memcpy(dst, src, len);
    return G2T_OK;
}

g2t_int g2t_memcmp(g2t_buffer dst, g2t_buffer src, g2t_uint32 len)
{
    return memcmp(dst, src, len);
}

g2t_status g2t_memset(g2t_buffer dst, g2t_int val, g2t_uint32 len)
{
    memset(dst, val, len);
    return G2T_OK;
}
/*-------------------------------------------------------------------
 * DefWndProc functions
 *-----------------------------------------------------------------*/
g2t_long _g2t_DefWndProc_OnEraseBk(g2t_wnd wnd, HDC dc);
g2t_void _g2t_DefWndProc_OnSetText(g2t_wnd wnd, g2t_string text);
g2t_long _g2t_DefWndProc_OnGetText(g2t_wnd wnd, g2t_string text, g2t_long cb);
g2t_void _g2t_DefWndProc_OnSetTextAlign(g2t_wnd wnd, g2t_int align);
g2t_dword _g2t_DefWndProc_OnSetTextAttrs(g2t_wnd wnd, g2t_dword textcolor, g2t_dword bgcolor);
g2t_dword _g2t_DefWndProc_OnGetTextAttrs(g2t_wnd wnd);
g2t_void _g2t_DefWndProc_OnSetFocus(g2t_wnd wnd);
g2t_long _g2t_DefWndProc_OnKillFocus(g2t_wnd wnd);
g2t_void _g2t_DefWndProc_OnSetInfoText(g2t_wnd wnd, g2t_string text);
g2t_long _g2t_DefWndProc_OnGetInfoText(g2t_wnd wnd, g2t_string text, g2t_long cb);
g2t_long _g2t_DefWndProc_OnShow(g2t_wnd wnd, g2t_long show);
g2t_long _g2t_DefWndProc_OnEnable(g2t_wnd wnd, g2t_long enable);
g2t_long _g2t_DefWndProc_OnGetCursor(g2t_wnd wnd, g2t_pos* pos);
g2t_long _g2t_DefWndProc_OnSetCursor(g2t_wnd wnd, g2t_pos* pos);
g2t_void _g2t_DefWndProc_OnDestroy(g2t_wnd wnd);
/*------------------------------------------------------------------------*/
g2t_void _g2t_DestroyWnd(g2t_wnd wnd);

g2t_wnd g2t_GetNextActiveWnd(g2t_wnd wnd)
{
    g2t_wnd activewnd = wnd;

    while (activewnd)
    {
        if (g2t_IsWndVisible(activewnd))
        {
            if (g2t_IsWndEnabled(activewnd))
            {
                if (g2t_strcmp(activewnd->clsname, G2T_STATIC) != 0)
                {
                    break; /* found the next control */
                }
            }
        }
        activewnd = activewnd->nextwnd;
    }
    return activewnd;
}

g2t_wnd     g2t_GetPrevActiveWnd(g2t_wnd wnd)
{
    g2t_wnd activewnd = wnd;
    while (activewnd)
    {
        if (g2t_IsWndVisible(activewnd))
        {
            if (g2t_IsWndEnabled(activewnd))
            {
                if (g2t_strcmp(activewnd->clsname, G2T_STATIC) != 0)
                {
                    break; /* found the next control */
                }
            }
        }
        activewnd = activewnd->prevwnd;
    }
    return activewnd;
}

g2t_wnd g2t_GetFirstActiveChildWnd(g2t_wnd wnd)
{
    if (wnd)
    {
        g2t_wnd activechild = wnd->activechild;
        g2t_wnd firstwnd = 0;

        wnd->activechild = wnd->lastchild;
        firstwnd = g2t_GetNextActiveChildWnd(wnd);

        wnd->activechild = activechild;
        return firstwnd;
    }
    return 0;
}

g2t_wnd g2t_GetLastActiveChildWnd(g2t_wnd wnd)
{
    if (wnd)
    {
        g2t_wnd activechild = wnd->activechild;
        g2t_wnd lastwnd = 0;

        wnd->activechild = wnd->firstchild;
        lastwnd = g2t_GetPrevActiveChildWnd(wnd);

        wnd->activechild = activechild;
        return lastwnd;
    }
    return 0;
}

g2t_wnd g2t_GetParent(g2t_wnd wnd)
{
    return (wnd ? wnd->parent : 0);
}

g2t_bool g2t_IsWndEnabled(g2t_wnd wnd)
{
    return (wnd ? wnd->enable : G2T_FALSE);
}

g2t_bool g2t_IsWndVisible(g2t_wnd wnd)
{
    return (wnd ? wnd->visible : G2T_FALSE);
}

g2t_wnd g2t_GetWndItem(g2t_wnd wnd, g2t_uint32 id)
{
    g2t_wnd child = wnd->firstchild;
    while (child)
    {
        if (child->id == id)
        {
            break;
        }
        child = child->nextwnd;
    }
    return child;
}

g2t_wnd g2t_GetPrevActiveChildWnd(g2t_wnd wnd)
{
    g2t_wnd activechild = (wnd ? wnd->activechild : 0);
    if (!activechild && wnd)
    {
        activechild = wnd->lastchild;
    }
    else if (activechild)
    {
        activechild = activechild->prevwnd;
        if (!activechild && wnd)
        {
            activechild = wnd->lastchild;
        }
    }
    while (activechild)
    {
        if (g2t_IsWndVisible(activechild))
        {
            if (g2t_IsWndEnabled(activechild))
            {
                if (g2t_strcmp(activechild->clsname, G2T_STATIC) != 0)
                {
                    break; /* found the next control */
                }
            }
        }
        activechild = activechild->prevwnd;
    }
    return activechild;
}

g2t_wnd g2t_GetActiveChildWnd(g2t_wnd wnd)
{
    g2t_wnd parent = (wnd ? wnd->parent : 0);
    return (wnd ? wnd->activechild : parent);
}

g2t_wnd g2t_GetNextActiveChildWnd(g2t_wnd wnd)
{
    g2t_wnd activechild = (wnd ? wnd->activechild : 0);
    if (!activechild && wnd)
    {
        activechild = wnd->firstchild;
    }
    else if (activechild)
    {
        activechild = activechild->nextwnd;
        if (!activechild && wnd)
        {
            activechild = wnd->firstchild;
        }
    }

    while (activechild)
    {
        if (g2t_IsWndVisible(activechild))
        {
            if (g2t_IsWndEnabled(activechild))
            {
                if (g2t_strcmp(activechild->clsname, G2T_STATIC) != 0)
                {
                    break; /* found the next control */
                }
            }
        }
        activechild = activechild->nextwnd;
    }
    return activechild;
}

g2t_status _g2t_InvalidateWnd(g2t_wnd wnd)
{
    g2t_wnd child = (wnd ? wnd->firstchild : 0);
    g2t_dc dc = g2t_GetDC();
    g2t_dc memdc = g2t_GetMemDC();
    g2t_int cx = GetSystemMetrics(SM_CXSCREEN);
    g2t_int cy = GetSystemMetrics(SM_CYSCREEN);

    /* draw itself */
    g2t_SendMsg(wnd, GWM_ERASEBK, (g2t_wparam) memdc, 0);
    g2t_SendMsg(wnd, GWM_PAINT, 0, 0);
    /* draw children */
    while (child)
    {
        g2t_SendMsg(child, GWM_ERASEBK, (g2t_wparam) memdc, 0);
        g2t_SendMsg(child, GWM_PAINT, 0, 0);
        child = child->nextwnd;
    }

    BitBlt(dc, 0, 0, cx, cy, memdc, 0, 0, SRCCOPY);

    return G2T_OK;
}

g2t_long g2t_IsWndValidate(g2t_wnd wnd, g2t_string text)
{
    g2t_long rc = G2T_CONTINUE;
    if (wnd->validateproc)
    {
        rc = wnd->validateproc(wnd, text);
    }
    return rc;
}

g2t_status g2t_InvalidateWnd(g2t_wnd wnd)
{
    g2t_status rc = _g2t_InvalidateWnd(wnd);
    g2t_wnd activechild = g2t_GetActiveChildWnd(wnd);
#if 0
    if (!activechild)
    {
        activechild = g2t_GetFirstActiveChildWnd(wnd);
    }
    if (activechild)
    {
        /*g2t_SetFocus(activechild);
        g2t_MoveXY(activechild->x, activechild->y);*/
        /* finally, paint the active window */
        g2t_SendMsg(activechild, GWM_ERASEBK, (g2t_wparam) g2t_GetMemDC(), 0);
        g2t_SendMsg(activechild, GWM_PAINT, 0, 0);
    }
#endif
    return rc;
}

g2t_wnd g2t_GetFocus(g2t_wnd wnd)
{
    return g2t_GetActiveChildWnd(wnd);
}

g2t_status g2t_SetFocus(g2t_wnd wnd)
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd parent = (wnd ? wnd->parent : env->activewnd);
    g2t_wnd activewnd = g2t_GetActiveChildWnd(wnd);
    g2t_status rc = G2T_CONTINUE;

    /* kill focus at the current control */
    if (activewnd)
    {
        rc = g2t_SendMsg(activewnd, GWM_KILLFOCUS, 0, (g2t_lparam) wnd);
        g2t_InvalidateWnd(activewnd);
    }
    if (rc != G2T_CONTINUE)
    {
        return rc;
    }
    if (wnd)
    {
        rc = g2t_SendMsg(wnd, GWM_SETFOCUS, 0, (g2t_lparam) activewnd);
        rc = g2t_InvalidateWnd(wnd);
        g2t_MoveXY(wnd->x, wnd->y);

        if (parent)
        {
            parent->activechild = wnd;
        }
    }
    return rc;
}

g2t_void _g2t_DefWndProc_OnDestroy(g2t_wnd wnd)
{
    g2t_wnd child = (wnd ? wnd->lastchild : 0);
    if (child)
    {
        g2t_SendMsg(child->firstchild, GWM_DESTROY, 0, 0);
        g2t_SendMsg(child->nextwnd, GWM_DESTROY, 0, 0);
        g2t_SendMsg(child, GWM_DESTROY, 0, 0);
    }
}

g2t_long _g2t_DefWndProc_OnGetCursor(g2t_wnd wnd, g2t_pos* pos)
{
    /*g2t_GetYX(g2t_GetDC(wnd), &pos->y, &pos->x);*/
    return G2T_OK;
}

g2t_long _g2t_DefWndProc_OnSetCursor(g2t_wnd wnd, g2t_pos* pos)
{
    /*g2t_MoveYX(g2t_GetDC(wnd), pos->y, pos->x);*/
    return G2T_OK;
}

g2t_long _g2t_DefWndProc_OnEraseBk(g2t_wnd wnd, g2t_dc dc)
{
    g2t_rect rc;
    g2t_pos pos;
    RECT rect;
    g2t_dword bgcolor = g2t_GetBgColor(wnd);
    HBRUSH hbr = 0;

    if (g2t_IsWndVisible(wnd))
    {
        g2t_GetWndRect(wnd, &rc);

        pos.x = rc.x;
        pos.y = rc.y;
        g2t_TextToScreen(&pos, &pos);
        rect.left = pos.x;
        rect.top = pos.y;

        pos.x = rc.x + rc.cols;
        pos.y = rc.y + rc.lines;
        g2t_TextToScreen(&pos, &pos);
        rect.right = pos.x;
        rect.bottom = pos.y;

        hbr = CreateSolidBrush(bgcolor);
        FillRect(dc, &rect, hbr);
        DeleteObject(hbr);
    }

    return G2T_OK;
}

g2t_void _g2t_DefWndProc_OnSetText(g2t_wnd wnd, g2t_string text)
{
    g2t_long len = 0;
    if (text)
    {
        len = g2t_strlen(text);
        if (len > G2T_MAX_WNDTEXT)
        {
            g2t_strncpy(wnd->wndname, text, G2T_MAX_WNDTEXT);
        }
        else
        {
            g2t_strcpy(wnd->wndname, text);
        }
    }
}

g2t_long _g2t_DefWndProc_OnGetText(g2t_wnd wnd, g2t_string text, g2t_long cb)
{
    g2t_long len = g2t_strlen(wnd->wndname);
    if (cb < 0 || !text)
    {
        return len;
    }

    if (cb > len + 1)
    {
        cb = len + 1;
    }
    g2t_memset(text, 0, cb);
    g2t_strncpy(text, wnd->wndname, cb);
    return g2t_strlen(text);
}

g2t_void _g2t_DefWndProc_OnSetInfoText(g2t_wnd wnd, g2t_string text)
{
    g2t_long len = 0;
    if (text)
    {
        len = g2t_strlen(text);
        if (len > G2T_MAX_WNDTEXT)
        {
            g2t_strncpy(wnd->infotext, text, G2T_MAX_WNDTEXT);
        }
        else
        {
            g2t_strcpy(wnd->infotext, text);
        }
    }
}

g2t_long _g2t_DefWndProc_OnGetInfoText(g2t_wnd wnd, g2t_string text, g2t_long cb)
{
    g2t_long len = g2t_strlen(wnd->infotext);
    if (cb < 0 || !text)
    {
        return len;
    }

    if (cb > len + 1)
    {
        cb = len + 1;
    }
    g2t_memset(text, 0, cb);
    g2t_strncpy(text, wnd->infotext, cb);
    return g2t_strlen(text);
}

g2t_void _g2t_DefWndProc_OnSetTextAlign(g2t_wnd wnd, g2t_int align)
{
    wnd->style &= ~(GWS_LEFT | GWS_CENTER | GWS_RIGHT);

    if (GALIGN_CENTER == align)
    {
        wnd->style |= GWS_CENTER;
    }
    else if (GALIGN_RIGHT == align)
    {
        wnd->style |= GWS_RIGHT;
    }
    else
    {
        wnd->style |= GWS_LEFT;
    }
}

g2t_dword g2t_GetWndTextAttrs(g2t_wnd wnd)
{
    return (wnd ? wnd->textcolor : G2T_RGB_WHITE);
}

g2t_long g2t_GetWndText(g2t_wnd wnd, g2t_string text, g2t_long cb)
{
    g2t_SendMsg(wnd, GWM_GETTEXT, cb, (g2t_lparam) text);
    return g2t_strlen(text);
}

g2t_dword _g2t_DefWndProc_OnSetTextAttrs(g2t_wnd wnd, g2t_dword textcolor, g2t_dword bgcolor)
{
    g2t_dword oldattrs = wnd->textcolor;
    wnd->textcolor = textcolor;
    wnd->bgcolor = bgcolor;
    return textcolor;
}

g2t_dword _g2t_DefWndProc_OnGetTextAttrs(g2t_wnd wnd)
{
    return wnd->textcolor;
}

g2t_uint32 g2t_GetWndID(g2t_wnd wnd)
{
    return (wnd ? wnd->id : 0);
}

g2t_status g2t_GetWndRect(g2t_wnd wnd, g2t_rect* rect)
{
    if (!wnd)
    {
        return G2T_ERROR;
    }
    rect->y = wnd->y;
    rect->x = wnd->x;
    rect->lines = wnd->lines;
    rect->cols = wnd->cols;
    return G2T_OK;
}


g2t_long _g2t_DefWndProc_OnKillFocus(g2t_wnd wnd)
{
    g2t_ShowCaret(GW_HIDE);
    return G2T_CONTINUE;
}

g2t_void _g2t_DefWndProc_OnSetFocus(g2t_wnd wnd)
{
    g2t_rect rcwnd;
    g2t_dispinfo di;
    di.hdr.id = g2t_GetWndID(wnd);
    di.hdr.ctl = wnd;
    di.hdr.code = GCN_DISPLAYINFO;
    g2t_strcpy(di.text, " ");

    if (wnd && g2t_strlen(wnd->infotext) > 0)
    {
        g2t_strcpy(di.text, wnd->infotext);
    }
    g2t_SendMsg(g2t_GetParent(wnd), GWM_NOTIFY, 0, (g2t_lparam) & di);

    g2t_ShowCaret(GW_SHOW);
    g2t_GetWndRect(wnd, &rcwnd);
    g2t_MoveXY(rcwnd.x, rcwnd.y);
}

g2t_long _g2t_DefWndProc_OnShow(g2t_wnd wnd, g2t_long show)
{
    g2t_long rc = G2T_OK;
    g2t_wnd activechild = g2t_GetActiveChildWnd(wnd);
    wnd->visible = show;

    if (!activechild)
    {
        activechild = g2t_GetFirstActiveChildWnd(wnd);
    }
    if (activechild)
    {
        /* finally, paint the active window */
        g2t_SendMsg(activechild, GWM_ERASEBK, (g2t_wparam) g2t_GetDC(), 0);
        g2t_SendMsg(activechild, GWM_PAINT, 0, 0);
    }
    rc = g2t_InvalidateWnd(wnd);
    if (activechild)
    {
        g2t_SendMsg(activechild, GWM_SETFOCUS, 0, 0);
    }

    return rc;
}

g2t_long _g2t_DefWndProc_OnEnable(g2t_wnd wnd, g2t_long enable)
{
    wnd->enable = enable;
    return G2T_OK;
}

g2t_int32 g2t_DefWndProc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
    case GWM_CREATE:
        return G2T_CONTINUE;

    case GWM_KILLFOCUS:
        return _g2t_DefWndProc_OnKillFocus(wnd);

    case GWM_SHOW:
    {
        return _g2t_DefWndProc_OnShow(wnd,
                                      (GW_HIDE == wparam ? GW_HIDE : GW_SHOW));
    }

    case GWM_ENABLE:
    {
        return _g2t_DefWndProc_OnEnable(wnd,
                                        (GW_DISABLE == wparam ? GW_DISABLE : GW_ENABLE));
    }

    case GWM_SETFOCUS:
    {
        _g2t_DefWndProc_OnSetFocus(wnd);
        break;
    }

    case GWM_ERASEBK:
    {
        return _g2t_DefWndProc_OnEraseBk(wnd, (HDC) wparam);
    }

    case GWM_SETINFOTEXT:
    {
        _g2t_DefWndProc_OnSetInfoText(wnd, (g2t_string) lparam);
        return 0;
    }
    case GWM_GETINFOTEXT:
    {
        return _g2t_DefWndProc_OnGetInfoText(wnd, (g2t_string) lparam, wparam);
    }

    case GWM_SETTEXT:
    {
        _g2t_DefWndProc_OnSetText(wnd, (g2t_string) lparam);
        return 0;
    }
    case GWM_GETTEXT:
    {
        return _g2t_DefWndProc_OnGetText(wnd, (g2t_string) lparam, wparam);
    }
    case GWM_SETTEXTALIGN:
    {
        _g2t_DefWndProc_OnSetTextAlign(wnd, wparam);
    }
    case GWM_SETTEXTATTRS:
    {
        return _g2t_DefWndProc_OnSetTextAttrs(wnd, wparam, lparam);
    }
    case GWM_GETTEXTATTRS:
    {
        _g2t_DefWndProc_OnGetTextAttrs(wnd);
        break;
    }
    case GWM_DESTROY:
    {
        _g2t_DefWndProc_OnDestroy(wnd);
        break;
    }
    }
    return G2T_OK;
}

g2t_status g2t_InitLockedObject(g2t_lockable* obj)
{
    g2t_status rc = G2T_OK;
    if (!InitializeCriticalSectionAndSpinCount(obj, 0x00000400))
    {
        rc = G2T_ERROR;
    }
    return rc;
}

g2t_void g2t_Lock(g2t_lockable* obj)
{
    EnterCriticalSection(obj);
}

g2t_void g2t_UnLock(g2t_lockable* obj)
{
    LeaveCriticalSection(obj);
}

g2t_void g2t_ReleaseLockedObject(g2t_lockable* obj)
{
    DeleteCriticalSection(obj);
}

g2t_dc g2t_GetMemDC()
{
    g2t_env env = g2t_GetEnv(); 
    return env->hmemdc;
}

g2t_dc g2t_GetDC()
{
    g2t_env env = g2t_GetEnv();
    return (env->hdc);
}

g2t_wnd _g2t_CreateWnd(
                       g2t_string clsname,
                       g2t_string wndname,
                       g2t_dword style,
                       g2t_dword exstyle,
                       g2t_int y,
                       g2t_int x,
                       g2t_int lines,
                       g2t_int cols,
                       g2t_wnd parent,
                       g2t_uint32 id,
                       g2t_string infotext,
                       g2t_void* param
                       )
{
    g2t_env env = g2t_GetEnv();
    g2t_wnd wnd = 0;
    g2t_long len = 0;
    g2t_wndproc_t* proc = _g2t_FindWndProc(env, clsname);

    if ((style & GWS_WINDOW) &&
            (style & GWS_CHILD))
    {
        /* not allow creating both styles at a time */
        return wnd;
    }
    else if (style & GWS_CHILD && !parent)
    {
        /* not allow child witwout parent */
        return wnd;
    }

    if (proc)
    {
        wnd = (g2t_wnd) g2t_malloc(sizeof (struct _g2t_wnd));
        if (wnd)
        {
            wnd->clsname = clsname;
            len = g2t_strlen(wndname);
            if (wndname && len > 0)
            {
                if (len > G2T_MAX_WNDTEXT)
                {
                    g2t_strncpy(wnd->wndname, wndname, G2T_MAX_WNDTEXT);
                }
                else
                {
                    g2t_strcpy(wnd->wndname, wndname);
                }
            }
            wnd->style = style;
            wnd->parent = parent;
            wnd->id = id;
            wnd->wndproc = proc->wndproc;
            wnd->enable = (style & GWS_DISABLED ? G2T_FALSE : G2T_TRUE);
            wnd->visible = (style & GWS_VISIBLE ? G2T_TRUE : G2T_FALSE);
            wnd->validateproc = 0;
            wnd->exstyle = exstyle;
            wnd->textcolor = G2T_RGB_WHITE;
            wnd->bgcolor = G2T_RGB_BLACK;
            /* info text */
            len = strlen(infotext);
            if (infotext && len > 0)
            {
                if (len > G2T_MAX_WNDTEXT)
                {
                    g2t_strncpy(wnd->infotext, infotext, G2T_MAX_WNDTEXT);
                }
                else
                {
                    g2t_strcpy(wnd->infotext, infotext);
                }
            }


            /* make link */
            if (style & GWS_WINDOW)
            {
                if (env->firstwnd)
                {
                    wnd->prevwnd = env->lastwnd;
                    env->lastwnd->nextwnd = wnd;
                    env->lastwnd = wnd;
                }
                else
                {
                    env->firstwnd = env->lastwnd = wnd;
                }
                env->activewnd = wnd;
                /*env->dlgmsgid  = MB_INVALID;*/
                wnd->x = x;
                wnd->y = y;
                wnd->lines = lines;
                wnd->cols = cols;
            }
            else if (style & GWS_CHILD)
            {
                if (parent->firstchild)
                {
                    wnd->prevwnd = parent->lastchild;
                    parent->lastchild->nextwnd = wnd;
                    parent->lastchild = wnd;
                }
                else
                {
                    parent->firstchild = parent->lastchild = wnd;
                    parent->activechild = wnd;
                }
                /* re-size window rectangle */
                wnd->y = (parent->y + y);
                wnd->x = (parent->x + x);
                wnd->lines = (lines > parent->lines ? parent->lines : lines);
                wnd->cols = (cols > parent->cols ? parent->cols : cols);
                if (g2t_GetWndStyle(parent) & GWS_BORDER)
                {
                    wnd->lines--;
                    wnd->cols--;
                }
                /* send message */
                g2t_SendMsg(wnd, GWM_ERASEBK, (g2t_wparam) g2t_GetDC(), 0);
                g2t_SendMsg(wnd, GWM_CREATE, 0, 0);
            }
        }
    }
    return wnd;
}

g2t_dword g2t_GetWndStyleEx(g2t_wnd wnd)
{
    return (wnd ? wnd->exstyle : 0);
}

g2t_dword g2t_GetWndStyle(g2t_wnd wnd)
{
    return (wnd ? wnd->style : 0);
}


g2t_dword g2t_SetWndStyleEx(g2t_wnd wnd, g2t_dword newstyle)
{
    g2t_dword oldstyle = (wnd ? wnd->exstyle : 0);
    if (wnd)
    {
        wnd->exstyle = newstyle;
    }
    return oldstyle;
}

g2t_dword g2t_SetWndStyle(TWND wnd, g2t_dword newstyle)
{
    g2t_dword oldstyle = (wnd ? wnd->style : 0);
    if (wnd)
    {
        wnd->style = newstyle;
    }
    return oldstyle;
}


g2t_long       g2t_MoveWnd(g2t_wnd wnd, g2t_int y, g2t_int x, g2t_int lines, g2t_int cols)
{
    TLONG rc = G2T_OK;
    wnd->y = y;
    wnd->x = x;
    wnd->lines = lines;
    wnd->cols = cols;
    return rc;
}

g2t_void _g2t_DestroyWnd(g2t_wnd wnd)
{
    g2t_wnd child = (wnd ? wnd->firstchild : 0);

    if (child)
    {
        _g2t_DestroyWnd(child->firstchild);
    }
    if (wnd)
    {
        _g2t_DestroyWnd(wnd->nextwnd);
    }

    if (wnd && wnd->prevwnd)
    {
        wnd->prevwnd->nextwnd = 0;
    }
    if (wnd)
    {
        wnd->nextwnd = wnd->prevwnd = 0;
        g2t_free(wnd);
    }
}

g2t_void g2t_DestroyWnd(g2t_wnd wnd)
{
    g2t_env env = g2t_GetEnv();
    /* remove the link if it is TWS_WINDOW */
    if (wnd && (wnd->style & GWS_WINDOW))
    {
        env->lastwnd = wnd->prevwnd;
        if (env->lastwnd)
        {
            env->lastwnd->nextwnd = 0;
        }
        env->activewnd = env->lastwnd;
    }
    /* save the last message id */
    g2t_SendMsg(wnd, GWM_DESTROY, 0, 0);
    _g2t_DestroyWnd(wnd);

    if (env->activewnd)
    {
        g2t_InvalidateWnd(env->activewnd);
    }
}

g2t_wnd g2t_CreateWnd(
                      g2t_string clsname,
                      g2t_string wndname,
                      g2t_dword style,
                      g2t_dword exstyle,
                      g2t_int y,
                      g2t_int x,
                      g2t_int lines,
                      g2t_int cols,
                      g2t_wnd parent,
                      g2t_uint32 id,
                      g2t_string infotext,
                      g2t_void* param
                      )
{
    g2t_status rc = G2T_CONTINUE;
    g2t_wnd child = 0;
    g2t_wnd wnd = _g2t_CreateWnd(
                                 clsname,
                                 wndname,
                                 style,
                                 exstyle,
                                 y,
                                 x,
                                 lines,
                                 cols,
                                 parent,
                                 id,
                                 infotext,
                                 param
                                 );
    if (wnd)
    {
        g2t_SendMsg(wnd, GWM_ERASEBK, (g2t_wparam) g2t_GetDC(), 0);
        rc = g2t_SendMsg(wnd, GWM_CREATE, 0, (g2t_lparam) param);
        if (rc != G2T_CONTINUE)
        {
            if (wnd)
            {
                _g2t_DestroyWnd(wnd);
            }
            wnd = 0;
        }
        child = g2t_GetFirstActiveChildWnd(wnd);
        if (child)
        {
            wnd->activechild = child;
            g2t_SetFocus(child);
        }
    }
    return wnd;
}

g2t_void g2t_SetWndText(g2t_wnd wnd, g2t_string text)
{
    if (text && wnd)
    {
        g2t_SendMsg(wnd, GWM_SETTEXT, 0, (g2t_lparam) text);
        g2t_InvalidateWnd(wnd);
    }
}

g2t_validateproc g2t_SetWndValidateProc(g2t_wnd wnd, g2t_validateproc validateproc)
{
    g2t_validateproc oldproc = wnd->validateproc;
    wnd->validateproc = validateproc;
    return oldproc;
}

g2t_wnd g2t_CreateFrameWnd(
                           g2t_string clsname,
                           g2t_string wndname,
                           g2t_dword style,
                           g2t_dword exstyle,
                           g2t_int y,
                           g2t_int x,
                           g2t_int lines,
                           g2t_int cols,
                           g2t_frmwndtemplate* templs,
                           g2t_void* param
                           )
{
    g2t_status rc = G2T_CONTINUE;
    g2t_wnd wnd = 0;
    g2t_wnd child = 0;
    g2t_int i = 0;
    g2t_dword childstyle = 0;

    wnd = g2t_CreateWnd(
                        clsname,
                        wndname,
                        style,
                        exstyle,
                        y,
                        x,
                        lines,
                        cols,
                        0,
                        0,
                        "",
                        param);
    if (wnd && templs)
    {
        for (i = 0; templs[i].clsname; ++i)
        {
            childstyle = templs[i].style;
            childstyle &= ~GWS_WINDOW; /* others must be child */
            childstyle |= GWS_CHILD; /* others must be child */
            child = _g2t_CreateWnd(
                                   templs[i].clsname,
                                   templs[i].text,
                                   childstyle,
                                   templs[i].exstyle,
                                   templs[i].y,
                                   templs[i].x,
                                   templs[i].lines,
                                   templs[i].cols,
                                   wnd,
                                   templs[i].id,
                                   templs[i].infotext,
                                   0
                                   );
            if (!child)
            {
                _g2t_DestroyWnd(wnd);
                wnd = 0;
            }
            else
            {
                g2t_SetWndValidateProc(child, (g2t_validateproc) templs[i].validateproc);
                g2t_SetWndText(child, templs[i].text);
            }
        } /* create children */

        if (wnd)
        {
            g2t_SendMsg(wnd, GWM_ERASEBK, (g2t_wparam) g2t_GetDC(), 0);
            rc = g2t_SendMsg(wnd, GWM_INITDIALOG, (g2t_wparam) 0, (g2t_lparam) param);
            if (rc != G2T_CONTINUE)
            {
                _g2t_DestroyWnd(wnd);
                wnd = 0;
            }
            child = g2t_GetFirstActiveChildWnd(wnd);
            if (child)
            {
                wnd->activechild = child;
                g2t_SetFocus(child);
            }
        }
    } /* templs is allocated */
    return wnd;
}

g2t_long _g2t_AlignmentPrint(g2t_string out, g2t_string in, g2t_long limit, g2t_long align)
{
    g2t_long len = 0;
    g2t_char text[G2T_MAX_WNDTEXT + 1];
    g2t_long firstlen = 0;
    g2t_long lastlen = 0;

    len = g2t_strlen(in);
    if (len > limit)
    {
        len = limit;
    }
    g2t_memset(text, 0, G2T_MAX_WNDTEXT);
    g2t_memset(out, 0, limit + 1);

    if ((GALIGN_CENTER == align) || (GWS_CENTER & align))
    {
        firstlen = (limit - len) / 2;
        lastlen = limit - (len + firstlen);
        if (firstlen > 0 && lastlen > 0)
        {
            sprintf(text, "%*s%s%*s",
                    firstlen, " ",
                    in,
                    lastlen, " ");
        }
        else if (lastlen > 0)
        {
            sprintf(text, "%s%*s",
                    in,
                    lastlen, " ");
        }
        else
        {
            sprintf(text, "%s", in);
        }
        g2t_strcpy(out, text);
    }
    else if ((GALIGN_RIGHT == align) || (GWS_RIGHT & align))
    {
        sprintf(text, "%*s",
                (g2t_int) (limit),
                in);
        g2t_strcpy(out, text);
    }
    else
    {
        sprintf(text, "%-*s",
                (g2t_int) (limit),
                in);
        g2t_strncpy(out, text, limit);
    }

    return g2t_strlen(out);
}

g2t_long g2t_PrintTextAlignment(g2t_string out, g2t_string in, g2t_long limit, g2t_long align)
{
    return _g2t_AlignmentPrint(out, in, limit, align);
}

g2t_dword g2t_GetTextColor(g2t_wnd wnd)
{
    return (wnd ? wnd->textcolor : G2T_RGB_WHITE);
}

g2t_dword g2t_GetBgColor(g2t_wnd wnd)
{
    return (wnd ? wnd->bgcolor : G2T_RGB_BLACK);
}

g2t_dword g2t_SetBgColor(g2t_wnd wnd, g2t_dword color)
{
    g2t_dword bgcolor = G2T_RGB_BLACK;
    if (wnd)
    {
        bgcolor = wnd->bgcolor;
        wnd->bgcolor = color;
    }
    return bgcolor;
}

g2t_dword g2t_SetTextColor(g2t_wnd wnd, g2t_dword color)
{
    g2t_dword textcolor = G2T_RGB_WHITE;
    if (wnd)
    {
        textcolor = wnd->textcolor;
        wnd->textcolor = color;
    }
    return textcolor;
}

g2t_lparam g2t_SetWndParam(g2t_wnd wnd, g2t_lparam lparam)
{
    g2t_lparam oldparam = (wnd ? wnd->lparam : 0);
    if (wnd)
    {
        wnd->lparam = lparam;
    }
    return oldparam;
}

g2t_bool   g2t_LockUpdate(g2t_wnd wnd, g2t_bool locked)
{
    g2t_bool oldlocked = (wnd ? wnd->lockedupdate : G2T_FALSE);
    wnd->lockedupdate = locked;
    return oldlocked;
}

g2t_long   g2t_GetClassName(g2t_wnd wnd, g2t_string clsname, g2t_long len)
{
    if (0 == wnd || 0 == clsname)
    {
        return -1;
    }
    if (len < 0)
    {
        g2t_strlen(wnd->clsname);
    }
    g2t_strcpy(clsname, wnd->clsname);
    return g2t_strlen(clsname);
}

g2t_lparam g2t_GetWndParam(g2t_wnd wnd)
{
    return (wnd ? wnd->lparam : 0);
}

g2t_status g2t_ShowWnd(g2t_wnd wnd, g2t_bool show)
{
    return g2t_SendMsg(wnd, GWM_SHOW, (g2t_wparam) show, 0);
}

g2t_status g2t_EnableWnd(g2t_wnd wnd, g2t_bool enable)
{
    return g2t_SendMsg(wnd, GWM_ENABLE, (g2t_wparam) enable, 0);
}

g2t_status g2t_DrawText(g2t_dc dc, g2t_int x, g2t_int y, g2t_string text, g2t_dword textcolor, g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    g2t_pos tpos = {y, x};
    g2t_pos spos;
    COLORREF oldtext = 0;
    COLORREF oldbg = 0;

    g2t_TextToScreen(&spos, &tpos);

    oldtext = SetTextColor(dc, textcolor);
    oldbg = SetBkColor(dc, bgcolor);
    SetTextAlign(dc, TA_LEFT | TA_TOP);

    TextOut(dc, spos.x, spos.y, text, g2t_strlen(text));

    SetTextColor(dc, oldtext);
    SetBkColor(dc, oldbg);
    return rc;
}

g2t_status g2t_MoveXY(g2t_int x, g2t_int y)
{
    g2t_status rc = G2T_OK;
    g2t_pos pos;
    pos.x = x;
    pos.y = y;
    g2t_TextToScreen(&pos, &pos);
    g2t_SetCaretPos(pos.x, pos.y);
    return rc;
}

g2t_status g2t_TextToScreen(g2t_pos* spos, g2t_pos* tpos)
{
    g2t_status rc = G2T_OK;
    g2t_env env = g2t_GetEnv();
    spos->x = tpos->x * env->cxCaps;
    spos->y = tpos->y * env->cyLine;
    return rc;
}

g2t_status g2t_ScreenToText(g2t_pos* tpos, g2t_pos* spos)
{
    g2t_status rc = G2T_OK;
    g2t_env env = g2t_GetEnv();
    tpos->x = (env->cxCaps ? spos->x / env->cxCaps : 0);
    tpos->y = (env->cyLine ? spos->y / env->cyLine : 0);
    return rc;
}

g2t_long g2t_PutChar(
                     g2t_dc dc,
                     g2t_int x,
                     g2t_int y,
                     g2t_char ch,
                     g2t_dword textcolor,
                     g2t_dword bgcolor)
{
    g2t_char text[2] = {ch, 0};
    return g2t_DrawText(dc, x, y, text, textcolor, bgcolor);
}

/*
g2t_DrawFrameChar()
    dc - device context
    x  - x-pos
    y  - y-pos
    ch is one of (G2T_UPPER_LEFT, G2T_UPPER_RIGHT, G2T_LOWER_LEFT, G2T_LOWER_RIGHT, G2T_VERTICAL, G2T_HORIZONTAL)
#define G2T_UPPER_LEFT      (1)
#define G2T_UPPER_RIGHT     (2)
#define G2T_LOWER_LEFT      (3)
#define G2T_LOWER_RIGHT     (4)
#define G2T_VERTICAL        (5)
#define G2T_HORIZONTAL      (6)
#define G2T_VERT_LEFT       (7)
#define G2T_VERT_RIGHT      (8)
#define G2T_HORZ_DOWN       (9)
#define G2T_HORZ_UP         (10)
#define G2T_VERT_HORZ       (11)
*/
g2t_status g2t_DrawFrameChar(g2t_dc dc, g2t_int x, g2t_int y, g2t_int ch, g2t_dword textcolor,
                     g2t_dword bgcolor)
{
    g2t_env env = g2t_GetEnv();
    g2t_status rc = G2T_OK;
    g2t_pos pos;
    pos.x = x;
    pos.y = y;
    g2t_TextToScreen(&pos, &pos);

    x = pos.x / 2;
    y = pos.y / 2;

    SetTextColor(dc, textcolor);
    SetBkColor(dc, bgcolor);
    switch (ch)
    {
        case G2T_UPPER_LEFT:
            MoveToEx(dc, pos.x + x, pos.y + env->cyChar, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + env->cxCaps, pos.y + y);
            break;
        case G2T_UPPER_RIGHT:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + x, pos.y + env->cyChar);
            break;
        case G2T_LOWER_LEFT:
            MoveToEx(dc, pos.x + x, pos.y, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + env->cxCaps, pos.y + y);
            break;
        case G2T_LOWER_RIGHT:
            MoveToEx(dc, pos.x + x, pos.y, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + x, pos.y + env->cyChar);
            break;
        case G2T_VERTICAL:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            break;
        case G2T_HORIZONTAL:
            MoveToEx(dc, pos.x + x, pos.y, NULL);
            LineTo(dc, pos.x + env->cxCaps, pos.y);
            break;
        case G2T_VERT_LEFT:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + x, pos.y + env->cyChar);
            LineTo(dc, pos.x + x, pos.y);
            break;
        case G2T_VERT_RIGHT:
            MoveToEx(dc, pos.x + x, pos.y + env->cyChar, NULL);
            LineTo(dc, pos.x + x, pos.y + y);
            LineTo(dc, pos.x + env->cxCaps, pos.y + y);
            LineTo(dc, pos.x, pos.y + y);
            break;
        case G2T_HORZ_DOWN:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            MoveToEx(dc, pos.x + x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            break;
        case G2T_HORZ_UP:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            MoveToEx(dc, pos.x + x, pos.y + y, NULL);
            LineTo(dc, pos.x + env->cxCaps, pos.y);
            break;
        case G2T_VERT_HORZ:
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            MoveToEx(dc, pos.x, pos.y + y, NULL);
            LineTo(dc, pos.x, pos.y + env->cyChar);
            break;
        default:
            rc = G2T_ERROR;
            break;
    }
    return rc;
}

g2t_status g2t_DrawVLine(g2t_dc dc, g2t_int x, g2t_int y, g2t_int blocks, g2t_dword textcolor,
                     g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    g2t_int i = 0;
    for (i = 0; i < blocks; ++i)
    {
        g2t_DrawFrameChar(dc, x, y+i, G2T_VERTICAL, textcolor, bgcolor);
    }
    return rc;
}
g2t_status g2t_DrawHLine(g2t_dc dc, g2t_int x, g2t_int y, g2t_int blocks, g2t_dword textcolor,
                     g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    g2t_int i = 0;
    for (i = 0; i < blocks; ++i)
    {
        g2t_DrawFrameChar(dc, x+i, y, G2T_HORIZONTAL, textcolor, bgcolor);
    }
    return rc;
}
g2t_status g2t_DrawFrame(g2t_dc dc, g2t_rect* rect, g2t_dword textcolor, g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    /* upper line */
    g2t_DrawHLine(dc, rect->x, rect->y, rect->cols, textcolor, bgcolor);
    /* lower line */
    g2t_DrawHLine(dc, rect->x, rect->y+rect->lines, rect->cols, textcolor, bgcolor);
    /* left line */
    g2t_DrawVLine(dc, rect->x, rect->y, rect->lines, textcolor, bgcolor);
    /* right line */
    g2t_DrawVLine(dc, rect->x+rect->cols, rect->y, rect->lines, textcolor, bgcolor);
    /* upper left*/
    g2t_DrawFrameChar(dc, rect->x, rect->y, G2T_UPPER_LEFT, textcolor, bgcolor);
    /* lower left*/
    g2t_DrawFrameChar(dc, rect->x, rect->y+rect->lines, G2T_LOWER_LEFT, textcolor, bgcolor);
    /* upper right*/
    g2t_DrawFrameChar(dc, rect->x+rect->cols, rect->y, G2T_UPPER_RIGHT, textcolor, bgcolor);
    /* lower right*/
    g2t_DrawFrameChar(dc, rect->x+rect->cols, rect->y+rect->lines, G2T_LOWER_RIGHT, textcolor, bgcolor);
    return rc;
}

g2t_status g2t_DrawHFrames(g2t_dc dc, g2t_rect* rect, g2t_int* heights, g2t_int frames, g2t_dword textcolor, g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    g2t_int i = 0;
    g2t_rect rcframe = *rect;
    for (i = 0; heights && i < frames; ++i)
    {
        rcframe.lines = rect->y + heights[i];
        g2t_DrawFrame(dc, &rcframe, textcolor, bgcolor);
    }
    return rc;
}
g2t_status g2t_DrawVFrames(g2t_dc dc, g2t_rect* rect, g2t_int* widths, g2t_int frames, g2t_dword textcolor, g2t_dword bgcolor)
{
    g2t_status rc = G2T_OK;
    g2t_int i = 0;
    g2t_rect rcframe = *rect;
    for (i = 0; widths && i < frames; ++i)
    {
        rcframe.cols = rect->x + widths[i];
        g2t_DrawFrame(dc, &rcframe, textcolor, bgcolor);
    }
    return rc;
}
