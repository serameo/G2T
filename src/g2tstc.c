/*-------------------------------------------------------------------
 * File name: g2tstc.c
 * Author: Seree Rakwong
 * Date: 24-SEP-19
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"

/*-------------------------------------------------------------------
 * STATIC functions
 *-----------------------------------------------------------------*/
g2t_void _TSTC_OnPaint(g2t_wnd wnd, g2t_dc dc);
g2t_long _TSTC_OnCreate(g2t_wnd wnd);
g2t_long G2TSTATICPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);

g2t_long _TSTC_OnCreate(g2t_wnd wnd)
{
    g2t_EnableWnd(wnd, GW_DISABLE);
    return G2T_CONTINUE;
}

g2t_void _TSTC_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_char text[G2T_MAX_WNDTEXT + 1];
    g2t_rect rc;
    g2t_string pos = 0;
    g2t_char   nextch = 0;

    if (g2t_IsWndVisible(wnd))
    {
        g2t_GetWndRect(wnd, &rc);

        g2t_GetWndText(wnd, text, G2T_MAX_WNDTEXT);
        pos = g2t_strchr(text, '&');

        if (!pos)
        {
            g2t_PrintTextAlignment(buf, text, rc.cols, g2t_GetWndStyle(wnd));
            g2t_DrawText(dc, rc.x, rc.y, buf, g2t_GetTextColor(wnd), g2t_GetBgColor(wnd));
        }
        else
        {
            ++pos;
            nextch = *pos;

            g2t_strcpywo(buf, text, '&');
            g2t_strcpy(text, buf);
            g2t_PrintTextAlignment(buf, text, rc.cols, g2t_GetWndStyle(wnd));
            g2t_DrawText(dc, rc.x, rc.y, buf, g2t_GetTextColor(wnd), g2t_GetBgColor(wnd));

            if (nextch)
            {
                pos = g2t_strchr(buf, nextch);
                if (pos)
                {
                    text[0] = nextch;
                    text[1] = 0;
                    g2t_DrawText(dc, rc.x+(g2t_long)(pos-buf), rc.y, text, g2t_GetBgColor(wnd), g2t_GetTextColor(wnd));
                }
            }
        }
    }
}

g2t_long G2TSTATICPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
    case GWM_PAINT:
    {
        _TSTC_OnPaint(wnd, g2t_GetMemDC());
        return 0;
    }
    case GWM_CREATE:
    {
        return _TSTC_OnCreate(wnd);
    }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

