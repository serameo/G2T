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
    g2t_char* pos = 0;
    g2t_char subbuf[G2T_MAX_WNDTEXT + 1];
    g2t_long len = 0;
    g2t_long len2 = 0;

    if (g2t_IsWndVisible(wnd))
    {
        g2t_GetWndRect(wnd, &rc);

        g2t_GetWndText(wnd, text, G2T_MAX_WNDTEXT);
        g2t_PrintTextAlignment(buf, text, rc.cols, g2t_GetWndStyle(wnd));

        pos = strchr(buf, '&');
        if (0 == pos)
        {
            g2t_DrawText(dc, rc.x, rc.y, buf, g2t_GetTextColor(wnd), g2t_GetBgColor(wnd));
        }
        else
        {
            len = (g2t_long)(size_t)(pos - buf);
            if (len > 0)
            {
                g2t_memset(subbuf, 0, sizeof(subbuf));
                g2t_memcpy(subbuf, buf, len);
                g2t_DrawText(dc, rc.x, rc.y, subbuf, g2t_GetTextColor(wnd), g2t_GetBgColor(wnd));
            }

            g2t_memset(subbuf, 0, sizeof(subbuf));
            g2t_memcpy(subbuf, &buf[(size_t)(pos - buf)]+1, 1);
            g2t_DrawText(dc, rc.x+len, rc.y, subbuf,
                g2t_GetSysColor(STATIC_MNEMONIC_TEXTCOLOR),
                g2t_GetSysColor(STATIC_MNEMONIC_BGCOLOR));

            len2 = strlen(buf) - (size_t)(pos - buf) + 1;
            g2t_memset(subbuf, 0, sizeof(subbuf));
            g2t_memcpy(subbuf, &buf[(size_t)(pos - buf)]+2, len2);
            g2t_DrawText(dc, rc.x+len+1, rc.y, subbuf, g2t_GetTextColor(wnd), g2t_GetBgColor(wnd));
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

