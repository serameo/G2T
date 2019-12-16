/*-------------------------------------------------------------------
 * File name: g2tedt.c
 * Author: Seree Rakwong
 * Date: 24-SEP-18
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"

/*-------------------------------------------------------------------
 * EDIT functions
 *-----------------------------------------------------------------*/

struct _TEDITSTRUCT
{
    g2t_char editbuf[G2T_MAX_WNDTEXT + 1];
    g2t_int firstvisit;
    g2t_char passchar;
    g2t_int showpass;
    g2t_int firstchar;
    g2t_int limitchars;
    g2t_int editing;
    g2t_int decwidth;
    g2t_char validstr[G2T_MAX_WNDTEXT + 1];
    g2t_int min;
    g2t_int max;
    g2t_int onminmax;
    g2t_void* exparam;
};
typedef struct _TEDITSTRUCT _TEDIT;
typedef struct _TEDITSTRUCT *TEDIT;

#define KILO      1000
#define MEGA      1000000
#define GIGA      1000000000
#define TERA      1000000000000
#define PETA      1000000000000000
#define EXA       1000000000000000000
/*
#define ZETA      1000000000000000000000
#define YOTTA     1000000000000000000000000
 */
g2t_long _TEDT_AddDecimalFormat(TEDIT edit);
g2t_long _TEDT_RemoveDecimalFormat(TEDIT edit);
g2t_long _TEDT_AddSuffix(TEDIT edit, g2t_int cols);
g2t_long _TEDT_RemoveSuffix(TEDIT edit);

g2t_void _TEDT_OnSetValidMinMax(g2t_wnd wnd, g2t_int on, g2t_validminmax* vmm);
g2t_long _TEDT_OnCreate(g2t_wnd wnd);
g2t_void _TEDT_OnDestroy(g2t_wnd wnd);
g2t_void _TEDT_OnSetFocus(g2t_wnd wnd);
g2t_long _TEDT_OnKillFocus(g2t_wnd wnd);
g2t_void _TEDT_OnChar(g2t_wnd wnd, g2t_long ch);
g2t_void _TEDT_OnPaint(g2t_wnd wnd, g2t_dc dc);
g2t_void _TEDT_OnLimitText(g2t_wnd wnd, g2t_int limit);
g2t_void _TEDT_OnSetPasswdChar(g2t_wnd wnd, g2t_char passchar);
g2t_void _TEDT_OnShowPasswdChar(g2t_wnd wnd, g2t_int show);
g2t_void _TEDT_OnSetDecWidth(g2t_wnd wnd, g2t_int width);
g2t_long _TEDT_ValidateNumberStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch);
g2t_long _TEDT_ValidateDecimalStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch);
g2t_long _TEDT_ValidateA2ZStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch);
/*g2t_void _TEDT_OnKeyDown(g2t_wnd wnd, g2t_long ch);*/

g2t_long G2TEDITBOXPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);

g2t_long _TEDT_AddDecimalFormat(TEDIT edit)
{
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_int cnt = 1;
    g2t_char* pbuf;
    g2t_long len = g2t_strlen(edit->editbuf) - 1;
    g2t_char* psz = &edit->editbuf[len];

    g2t_memset(buf, 0, sizeof (buf));
    pbuf = buf;

    while (*psz != 0 && *psz != '.')
    {
        *pbuf = *psz;
        ++pbuf;
        --psz;
        --len;
    }
    if ('.' == *psz)
    {
        *pbuf = *psz;
        ++pbuf;
        --psz;
        --len;
    }

    while (len >= 0)
    {
        if (cnt % 4 == 0 && *psz != '-')
        {
            *pbuf = ',';
            ++pbuf;
            ++cnt;
        }
        *pbuf = *psz;
        ++pbuf;
        --psz;
        --len;
        ++cnt;
    }
    /* save */
    g2t_memset(edit->editbuf, 0, sizeof (edit->editbuf));
    /* reverse copy */
    len = g2t_strlen(buf) - 1;
    pbuf = &buf[len];
    psz = edit->editbuf;
    while (len >= 0)
    {
        *psz = *pbuf;
        ++psz;
        --pbuf;
        --len;
    }
    return G2T_OK;
}

g2t_long _TEDT_RemoveDecimalFormat(TEDIT edit)
{
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_char* psz = edit->editbuf;
    g2t_char* pbuf = buf;

    if (strchr(edit->editbuf, ','))
    {
        g2t_memset(buf, 0, sizeof (buf));
        while (*psz != 0 && *psz != '.')
        {
            if (*psz != ',')
            {
                *pbuf = *psz;
                ++pbuf;
            }
            ++psz;
        }
        while (*psz != 0)
        {
            *pbuf = *psz;
            ++pbuf;
            ++psz;
        }
        /* save */
        g2t_strcpy(edit->editbuf, buf);
    }
    return G2T_OK;
}

g2t_long _TEDT_AddSuffix(TEDIT edit, g2t_int cols)
{
    g2t_double dbl = atof(edit->editbuf);
    g2t_long len = g2t_strlen(edit->editbuf);
    g2t_char suffix[3] = {' ', 0, 0};
    g2t_int cnt = 0;

    while (len > 0)
    {
        len -= 3;
        ++cnt;
    }
    len = g2t_strlen(edit->editbuf) + cnt;
    cnt = 0;

    while (cols < len)
    {
        len -= 3;
        ++cnt;
        dbl = dbl / KILO;
    }
    switch (cnt)
    {
    case 1:
    {
        suffix[0] = 'K';
        break;
    }
    case 2:
    {
        suffix[0] = 'M';
        break;
    }
    case 3:
    {
        suffix[0] = 'G';
        break;
    }
    case 4:
    {
        suffix[0] = 'T';
        break;
    }
    case 5:
    {
        suffix[0] = 'P';
        break;
    }
    case 6:
    {
        suffix[0] = 'E';
        break;
    }
    }

    if (suffix[0] != ' ')
    {
        wsprintf((char*) edit->editbuf, "%.*f", (g2t_int) edit->decwidth, dbl);
        g2t_strcat(edit->editbuf, suffix);
    }
    else
    {
        /* not support */
        return G2T_ERROR;
    }
    return G2T_OK;
}

g2t_long _TEDT_RemoveSuffix(TEDIT edit)
{
    g2t_double dbl = atof(edit->editbuf);
    g2t_char suffix = edit->editbuf[strlen(edit->editbuf) - 1];
    switch (suffix)
    {
    case 'K':
    {
        dbl = dbl * KILO;
        break;
    }
    case 'M':
    {
        dbl = dbl * MEGA;
        break;
    }
    case 'G':
    {
        dbl = dbl * GIGA;
        break;
    }
    case 'T':
    {
        dbl = dbl * TERA;
        break;
    }
    case 'P':
    {
        dbl = dbl * PETA;
        break;
    }
    case 'E':
    {
        dbl = dbl * EXA;
        break;
    }
    }
    wsprintf((g2t_char*) edit->editbuf, "%.*f", (g2t_int) edit->decwidth, dbl);
    return G2T_OK;
}

g2t_long _TEDT_OnCreate(g2t_wnd wnd)
{
    TEDIT edit = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_rect rc;

    /* initial memory for edit control */
    edit = (TEDIT) malloc(sizeof (_TEDIT));
    if (!edit)
    {
        return G2T_ENOMEM;
    }

    memset(edit, 0, sizeof (_TEDIT));
    g2t_GetWndRect(wnd, &rc);
    g2t_GetWndText(wnd, edit->editbuf, G2T_MAX_WNDTEXT);

    edit->firstvisit = 1;
    edit->firstchar = 0;
    edit->passchar = '*';
    edit->showpass = GW_SHOW;
    edit->editing = 0;
    edit->decwidth = 6;
    edit->limitchars = (GES_AUTOHSCROLL & style ? G2T_MAX_WNDTEXT : rc.cols);

    g2t_SetWndParam(wnd, (g2t_lparam) edit);
    g2t_SetTextColor(wnd, G2T_RGB_WHITE);
    g2t_SetBgColor(wnd, G2T_RGB_BLUE);

    /* validate and verify text */
    /*_TEDT_OnKillFocus(wnd);
    _TEDT_OnSetFocus(wnd);*/
    return G2T_CONTINUE;
}

g2t_void _TEDT_OnDestroy(g2t_wnd wnd)
{
    TEDIT edit = 0;
    /* release memory of edit control */
    edit = (TEDIT) g2t_GetWndParam(wnd);
    free(edit);
}

g2t_void _TEDT_OnSetFocus(g2t_wnd wnd)
{
    TEDIT edit = 0;
    g2t_nhdr nmhdr;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_pos pos;
    g2t_rect rect;
    g2t_char buf[G2T_MAX_WNDTEXT+1];
    /* save edited buffer */
    edit = (TEDIT) g2t_GetWndParam(wnd);
    g2t_GetWndText(wnd, buf, G2T_MAX_WNDTEXT);

    if (0 == g2t_strlen(edit->editbuf))
    {
        g2t_strcpy(edit->editbuf, buf);
    }
    edit->firstvisit = 1;
    edit->firstchar = 0;
    edit->editing = 0;

    if (GES_AUTODECIMALCOMMA & style)
    {
        _TEDT_RemoveDecimalFormat(edit);
        if (GES_AUTOSUFFIX & style)
        {
            _TEDT_RemoveSuffix(edit);
        }
    }

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = GEN_SETFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);

    /*g2t_InvalidateWnd(wnd);*/

    g2t_GetWndRect(wnd, &rect);
    pos.x = rect.x + g2t_strlen(edit->editbuf);
    pos.y = rect.y;
    g2t_TextToScreen(&pos, &pos);

    g2t_ShowCaret(GW_SHOW);
}

g2t_long _TEDT_OnKillFocus(g2t_wnd wnd)
{
    TEDIT edit = 0;
    g2t_nhdr nmhdr;
    g2t_long rc = G2T_CONTINUE;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_double decimal = 0.0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_long rcminmax = G2T_CONTINUE;
    g2t_int number = 0;
    g2t_rect rcwnd;
    g2t_pos pos;

    g2t_GetWndRect(wnd, &rcwnd);
    edit = (TEDIT) g2t_GetWndParam(wnd);
    edit->firstvisit = 1;

    /* check if style is GES_DECIMAL */
    if (GES_DECIMAL & style || GES_AUTODECIMALCOMMA & style)
    {
        decimal = atof(edit->editbuf);
        sprintf((char*) buf, "%.*f", (g2t_int) edit->decwidth, decimal);
        g2t_strcpy(edit->editbuf, buf);
        if (GW_ENABLE == edit->onminmax)
        {
            if (decimal < (g2t_double) edit->min &&
                    decimal > (g2t_double) edit->max)
            {
                rcminmax = G2T_ERROR;
            }
        }
    }
    else if ((GES_NUMBER & style))
    {
        number = atol(edit->editbuf);
        sprintf((char*) buf, "%ld", number);
        g2t_strcpy(edit->editbuf, buf);
        if (GW_ENABLE == edit->onminmax)
        {
            if (number < edit->min &&
                    number > edit->max)
            {
                rcminmax = G2T_ERROR;
            }
        }
    }
    else
    {
        g2t_strcpy(buf, edit->editbuf);
    }
    /* validate min & max */
    if (rcminmax != G2T_CONTINUE)
    {
        return rcminmax;
    }

    /* validate */
    rc = g2t_IsWndValidate(wnd, buf);
    if (rc != G2T_CONTINUE)
    {
        return rc;
    }

    if (GES_AUTODECIMALCOMMA & style)
    {
        _TEDT_AddDecimalFormat(edit);
        if (GES_AUTOSUFFIX & style)
        {
            if (rcwnd.cols < (g2t_int) g2t_strlen(edit->editbuf))
            {
                _TEDT_RemoveDecimalFormat(edit);
                _TEDT_AddSuffix(edit, rcwnd.cols);
                _TEDT_AddDecimalFormat(edit);
            }
        }
    }
    /* update text */
    edit->firstchar = 0;
    edit->editing = 0;
    g2t_SetWndText(wnd, edit->editbuf);

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = GEN_KILLFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);

    pos.y = rcwnd.y;
    pos.x = rcwnd.x;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_SETCURSOR, 0, (g2t_lparam) & pos);

    g2t_ShowCaret(GW_HIDE);
    g2t_InvalidateWnd(wnd);

    return rc;
}

g2t_long _TEDT_ValidateA2ZStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch)
{
    g2t_long rc = G2T_ERROR;
    if (ch >= 'a' && ch <= 'z')
    {
        rc = G2T_CONTINUE;
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
        rc = G2T_CONTINUE;
    }
    return rc;
}

g2t_long _TEDT_ValidateNumberStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch)
{
    g2t_long rc = G2T_CONTINUE;
    g2t_long len = g2t_strlen(edit->editbuf);

    if (ch < '0' || ch > '9')
    {
        if ((rc == G2T_CONTINUE) && (len == 0 || (edit->firstvisit == 1)) && ch == '-')
        {
            /* ok */
            rc = G2T_CONTINUE;
        }
        /* not allowed '-' in the string */
        else if ((len > 0 && (edit->firstvisit == 0)) && ch == '-')
        {
            rc = G2T_ERROR;
        }
        else
        {
            rc = G2T_ERROR;
        }
    }
    return rc;
}

g2t_long _TEDT_ValidateDecimalStyle(g2t_wnd wnd, TEDIT edit, g2t_long ch)
{
    g2t_long rc = G2T_CONTINUE;
    g2t_char* decimal = strchr(edit->editbuf, '.');
    g2t_long len = g2t_strlen(edit->editbuf);

    /* not allowed '-' in the string */
    if ((len > 0 && (edit->firstvisit == 0)) && ch == '-')
    {
        rc = G2T_ERROR;
    }
    if ((rc == G2T_CONTINUE) && (len == 0 || (edit->firstvisit == 1)) && ch == '-')
    {
        /* ok */
    }
    else if (ch == '.')
    {
        if (decimal)
        {
            /* not allowed having '.' more than one */
            rc = G2T_ERROR;
        }
    }
    else if (ch < '0' || ch > '9')
    {
        rc = G2T_ERROR;
    }
    return rc;
}

g2t_void _TEDT_OnChar(g2t_wnd wnd, g2t_long ch)
{
    TEDIT edit = 0;
    /*g2t_nhdr nmhdr;*/
    g2t_int changed = 0;
    g2t_dc dc = g2t_GetDC();
    g2t_long len = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_dword attrs = g2t_GetWndTextAttrs(wnd);
    g2t_char text[G2T_MAX_WNDTEXT + 1];
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_rect rc;
    g2t_char* psz;
    g2t_long ret = G2T_CONTINUE;
    g2t_nhdr nmhdr;
    g2t_dword textcolor = g2t_GetTextColor(wnd);
    g2t_dword bgcolor = g2t_GetBgColor(wnd);

    edit = (TEDIT) g2t_GetWndParam(wnd);

    if (g2t_IsWndEnabled(wnd))
    {
        if (style & GES_UNDERLINE)
        {
            /*attrs = g2t_UnderlineText(attrs);*/
        }
    }
    else
    {
        return;
    }

    g2t_GetWndText(wnd, text, G2T_MAX_WNDTEXT);
    g2t_GetWndRect(wnd, &rc);
    /* cancel editing */
    if (ch == VK_ESCAPE)
    {
        if (edit->firstvisit == 0)
        {
            edit->firstvisit = 1;
            g2t_strcpy(edit->editbuf, text);
            edit->firstchar = 0;
            edit->editing = 0;
            /*g2t_ShowCaret(GW_HIDE);*/
            g2t_InvalidateWnd(wnd);
            /*g2t_ShowCaret(GW_SHOW);*/
            return;
        }
    }
    /* alphablet */
    if (ch >= 0x20 && ch < 0x7f)
    {
        /* add char */
        len = g2t_strlen(edit->editbuf);
        /* is the first typing? */
        if (edit->firstvisit == 1)
        {
            edit->firstvisit = 0;
            if (!(GES_APPENDMODE & style) || (GES_PASSWORD & style)) /* replace mode */
            {
                edit->editbuf[0] = 0;
                g2t_memset(buf, ' ', rc.cols);
                buf[rc.cols] = 0;
                /*g2t_ShowCaret(GW_HIDE);*/
                g2t_DrawText(dc, rc.x, rc.y, buf, textcolor, bgcolor);
                g2t_MoveXY(rc.x, rc.y);
                /*g2t_ShowCaret(GW_SHOW);*/

                len = 0;
            }
            else if (GES_AUTOHSCROLL & style)
            {
                /* show first char at */
                if (len > rc.cols)
                {
                    edit->firstchar = len - rc.cols - 1;
                }
            }
        }

        if (len + 1 <= edit->limitchars)
        {
            if (GES_NUMBER & style)
            {
                /* require only number input */
                ret = _TEDT_ValidateNumberStyle(wnd, edit, ch);
                if (ret != G2T_CONTINUE)
                {
                    return;
                }
            }
            else if (GES_DECIMAL & style || GES_AUTODECIMALCOMMA & style)
            {
                /* require only decimal input */
                ret = _TEDT_ValidateDecimalStyle(wnd, edit, ch);
                if (ret != G2T_CONTINUE)
                {
                    return;
                }
            }
            else if (GES_UPPERCASE & style)
            {
                /* require changing from small to CAPITAL */
                if (ch >= 'a' && ch <= 'z')
                {
                    ch = ch - 'a' + 'A';
                }
            }
            else if (GES_LOWERCASE & style)
            {
                if (ch >= 'A' && ch <= 'Z')
                {
                    ch = ch - 'A' + 'a';
                }
            }

            if (GES_A2Z & style)
            {
                /* require only A-Z input */
                ret = _TEDT_ValidateA2ZStyle(wnd, edit, ch);
                if (ret != G2T_CONTINUE)
                {
                    return;
                }
            }
            /* valid char if it is in valid string */
            if (edit->validstr[0] != 0)
            {
                psz = strchr(edit->validstr, ch);
                if (!psz)
                {
                    return;
                }
            }

            /* append a new char */
            len = g2t_strlen(edit->editbuf);
            if (len < edit->limitchars)
            {
                edit->editbuf[len] = (g2t_char) (ch & 0xff);
                edit->editbuf[len + 1] = 0;
                if (len < rc.cols)
                {
                    if (GES_PASSWORD & style)
                    {
                        if (edit->showpass == GW_SHOW)
                        {
                            /*g2t_ShowCaret(GW_HIDE);*/
                            g2t_InvalidateWnd(wnd);
                            g2t_MoveXY(rc.x + g2t_strlen(edit->editbuf), rc.y);
                            /*g2t_ShowCaret(GW_SHOW);*/
                        }
                    }
                    else
                    {
                        /*g2t_ShowCaret(GW_HIDE);*/
                        g2t_InvalidateWnd(wnd);
                        g2t_MoveXY(rc.x + g2t_strlen(edit->editbuf), rc.y);
                        /*g2t_ShowCaret(GW_SHOW);*/
                    }
                }
                changed = 1;
            } /* len < limit */

            if (GES_AUTOHSCROLL & style)
            {
                len = strlen(edit->editbuf);
                if (len <= edit->limitchars && len > rc.cols && changed)
                {
                    ++edit->firstchar;
                    if (edit->firstchar > edit->limitchars - rc.cols)
                    {
                        edit->firstchar = edit->limitchars - rc.cols - 1;
                    }

                    /*g2t_ShowCaret(GW_HIDE);*/
                    g2t_InvalidateWnd(wnd);

                    len = g2t_strlen(edit->editbuf);
                    len = (len > rc.cols ? rc.cols : len);
                    g2t_MoveXY(rc.x + len, rc.y);
                    /*g2t_ShowCaret(GW_SHOW);*/
                }
            }

            /* editing */
            edit->editing = 1;

        } /*G2T_MAX_WNDTEXT*/
    }
    /* delete or backspace */
    else if (0x7f == ch || VK_BACK == ch)
    {
        len = strlen(edit->editbuf);
        if (edit->firstvisit == 1)
        {
            edit->firstvisit = 0;
            if (!(GES_APPENDMODE & style) || GES_PASSWORD & style) /* replace mode */
            {
                edit->editbuf[0] = 0;
                /*g2t_ShowCaret(GW_HIDE);*/
                g2t_InvalidateWnd(wnd);
                g2t_MoveXY(rc.x, rc.y);
                /*g2t_ShowCaret(GW_SHOW);*/
            }
            else if (GES_AUTOHSCROLL & style)
            {
                /* show first char at */
                if (len > rc.cols)
                {
                    edit->firstchar = len - rc.cols - 1;
                }
            }
        }

        if (len > 0)
        {
            edit->editbuf[len - 1] = 0;
            /*g2t_ShowCaret(GW_HIDE);*/
            g2t_InvalidateWnd(wnd);
            g2t_MoveXY(rc.x + len -1, rc.y);
            /*g2t_ShowCaret(GW_SHOW);*/

            changed = 1;

            if (GES_AUTOHSCROLL & style)
            {
                len = g2t_strlen(edit->editbuf);
                if (len >= rc.cols)
                {
                    edit->firstchar = len - rc.cols;
                    if (edit->firstchar < 0)
                    {
                        edit->firstchar = 0;
                    }
                    len = rc.cols;
                }
                else
                {
                    edit->firstchar = 0;
                }
                /*g2t_ShowCaret(GW_HIDE);*/
                g2t_InvalidateWnd(wnd);
                g2t_MoveXY(rc.x + len, rc.y);
                /*g2t_ShowCaret(GW_SHOW);*/
            }
        }

        /* editing */
        edit->editing = 1;
    }
        /* moving key pressed */
    else if (GES_NOTIFYKEYPRESSED & style)
    {
        switch (ch)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_NEXT:
        case VK_PRIOR:
            /*case VK_RETURN:*/
        case VK_INSERT:
        case VK_DELETE:
        case VK_END:
        case VK_HOME:
        {
            /*
            TNMHDRKEYPRESSEDSTRUCT keyhdr;
             keyhdr.hdr.id = g2t_GetWndID(wnd);
             keyhdr.hdr.ctl = wnd;
             keyhdr.hdr.code = GEN_KEYPRESSED;
             keyhdr.vkey = ch;
             g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam)&keyhdr);
             */
        }
        }
    }
    if (changed)
    {
        nmhdr.id = g2t_GetWndID(wnd);
        nmhdr.ctl = wnd;
        nmhdr.code = GEN_CHANGED;
        g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);
    }
}

g2t_void _TEDT_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
    TEDIT edit = 0;
    g2t_long len = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_char text[G2T_MAX_WNDTEXT + 1];
    g2t_rect rc;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_dword textcolor = g2t_GetSysColor(EDITBOX_TEXTCOLOR);
    g2t_dword bgcolor = g2t_GetSysColor(EDITBOX_BGCOLOR);

    edit = (TEDIT) g2t_GetWndParam(wnd);

    if (g2t_IsWndVisible(wnd))
    {
        if (g2t_IsWndEnabled(wnd))
        {
            if (style & GES_UNDERLINE)
            {
                /*attrs = g2t_UnderlineText(attrs);*/
            }
        }

        g2t_GetWndRect(wnd, &rc);
        len = rc.cols;
        if (GES_PASSWORD & style)
        {
            if (edit->showpass == GW_SHOW)
            {
                g2t_memset(buf, edit->passchar, len);
            }
            else
            {
                g2t_memset(buf, ' ', len);
            }
            buf[len] = 0;

            g2t_PrintTextAlignment(text, buf, rc.cols, GALIGN_LEFT);
            g2t_DrawText(dc, rc.x, rc.y, text, textcolor, bgcolor);
            g2t_MoveXY(rc.x + g2t_strlen(buf), rc.y);
            return;
        }

        len = G2T_MIN((g2t_int) g2t_strlen(edit->editbuf), rc.cols);
        g2t_memset(buf, 0, G2T_MAX_WNDTEXT);
        g2t_memcpy(buf, &edit->editbuf[edit->firstchar], len);

        if (edit->editing)
        {
            g2t_PrintTextAlignment(text, buf, rc.cols, GALIGN_LEFT);
        }
        else
        {
            g2t_PrintTextAlignment(text, buf, rc.cols, style);
        }

        g2t_DrawText(dc, rc.x, rc.y, text, textcolor, bgcolor);
        g2t_MoveXY(rc.x + g2t_strlen(buf), rc.y);
    }
}

g2t_void _TEDT_OnLimitText(g2t_wnd wnd, g2t_int limit)
{
    TEDIT edit = 0;
    g2t_char text[G2T_MAX_WNDTEXT + 1];

    if (limit > 0 || limit <= G2T_MAX_WNDTEXT)
    {
        edit = (TEDIT) g2t_GetWndParam(wnd);
        edit->limitchars = limit;

        g2t_GetWndText(wnd, text, G2T_MAX_WNDTEXT);
        if ((g2t_int) g2t_strlen(text) > limit)
        {
            text[limit] = 0;
            g2t_SetWndText(wnd, text);
            g2t_strcpy(edit->editbuf, text);
        }
    }
}

g2t_void _TEDT_OnSetPasswdChar(g2t_wnd wnd, g2t_char passchar)
{
    TEDIT edit = 0;

    edit = (TEDIT) g2t_GetWndParam(wnd);
    edit->passchar = passchar;
}

g2t_void _TEDT_OnShowPasswdChar(g2t_wnd wnd, g2t_int show)
{
    TEDIT edit = 0;

    edit = (TEDIT) g2t_GetWndParam(wnd);
    edit->showpass = (show == GW_SHOW ? GW_SHOW : GW_HIDE);
}

g2t_void _TEDT_OnSetDecWidth(g2t_wnd wnd, g2t_int width)
{
    TEDIT edit = 0;

    edit = (TEDIT) g2t_GetWndParam(wnd);
    edit->decwidth = width;
}

g2t_void _TEDT_OnSetText(g2t_wnd wnd, g2t_string text)
{
    TEDIT edit = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_double decimal = 0.0;
    g2t_char buf[G2T_MAX_WNDTEXT+1];
    g2t_long number = 0;

    edit = (TEDIT) g2t_GetWndParam(wnd);
    g2t_GetWndText(wnd, edit->editbuf, G2T_MAX_WNDTEXT);
    edit->firstvisit = 1;

    /* check if style is GES_DECIMAL */
    if (GES_DECIMAL & style || GES_AUTODECIMALCOMMA & style)
    {
        decimal = atof(edit->editbuf);
        if (GW_ENABLE == edit->onminmax)
        {
            if (decimal < (g2t_double) edit->min)
            {
                decimal = edit->min;
            }
            else if (decimal > (g2t_double) edit->max)
            {
                decimal = edit->max;
            }
        }
        sprintf((char*) buf, "%.*f", (g2t_int) edit->decwidth, decimal);
        g2t_strcpy(edit->editbuf, buf);
    }
    else if ((GES_NUMBER & style))
    {
        number = atol(edit->editbuf);
        if (GW_ENABLE == edit->onminmax)
        {
            if (number < edit->min)
            {
                number = edit->min;
            }
            else if (number > edit->max)
            {
                number = edit->max;
            }
        }
        sprintf((char*) buf, "%ld", number);
        g2t_strcpy(edit->editbuf, buf);
    }
    if (GES_AUTODECIMALCOMMA & style)
    {
        _TEDT_RemoveDecimalFormat(edit);
        _TEDT_AddDecimalFormat(edit);
        if (GES_AUTOSUFFIX & style)
        {
            g2t_rect rcwnd;
            g2t_GetWndRect(wnd, &rcwnd);
            if (rcwnd.cols < (g2t_int) g2t_strlen(edit->editbuf))
            {
                _TEDT_RemoveDecimalFormat(edit);
                _TEDT_AddSuffix(edit, rcwnd.cols);
                _TEDT_AddDecimalFormat(edit);
            }
        }
    }
    /*g2t_SetWndText(wnd, edit->editbuf);*/
    /*g2t_strcpy(wnd->text, edit->editbuf);*/
}

g2t_void _TEDT_OnSetValidString(g2t_wnd wnd, g2t_string validstr)
{
    TEDIT edit = 0;

    edit = (TEDIT) g2t_GetWndParam(wnd);
    if (validstr)
    {
        /* set the valid string */
        g2t_strcpy(edit->validstr, validstr);
    }
    else
    {
        /* remove valid string */
        g2t_memset(edit->validstr, 0, sizeof (edit->validstr));
    }
}

g2t_void _TEDT_OnSetValidMinMax(g2t_wnd wnd, g2t_int on, g2t_validminmax* vmm)
{
    TEDIT edit = 0;
    edit = (TEDIT) g2t_GetWndParam(wnd);
    edit->min = vmm->min;
    edit->max = vmm->max;
    edit->onminmax = (on == GW_ENABLE ? GW_ENABLE : GW_DISABLE);
}

g2t_long G2TEDITBOXPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
    case GWM_CREATE:
    {
        return _TEDT_OnCreate(wnd);
    }
    case GWM_DESTROY:
    {
        _TEDT_OnDestroy(wnd);
        return 0;
    }
    case GWM_SETTEXT:
    {
        g2t_DefWndProc(wnd, msg, wparam, lparam);
        _TEDT_OnSetText(wnd, (g2t_string) lparam);
        return 0;
    }
    case GWM_SETFOCUS:
    {
        _TEDT_OnSetFocus(wnd);
        break;
    }
    case GWM_KILLFOCUS:
    {
        return _TEDT_OnKillFocus(wnd);
    }
        /*case GWM_KEYDOWN:
        {
                            _TEDT_OnKeyDown(wnd, (g2t_long)wparam);
                            break;
        }*/
    case GWM_CHAR:
    {
        _TEDT_OnChar(wnd, (g2t_long) wparam);
        break;
    }
    case GWM_PAINT:
    {
        _TEDT_OnPaint(wnd, g2t_GetMemDC());
        return 0;
    }
    case GEM_LIMITTEXT:
    {
        _TEDT_OnLimitText(wnd, (g2t_int) wparam);
        return 0;
    }
    case GEM_SETPASSWDCHAR:
    {
        _TEDT_OnSetPasswdChar(wnd, (g2t_char) wparam);
        return 0;
    }
    case GEM_SETDECWIDTH:
    {
        _TEDT_OnSetDecWidth(wnd, (g2t_int) wparam);
        return 0;
    }
    case GEM_SHOWPASSWDCHAR:
    {
        _TEDT_OnShowPasswdChar(wnd, (g2t_int) wparam);
        return 0;
    }

    case GEM_SETVALIDSTRING:
    {
        _TEDT_OnSetValidString(wnd, (g2t_string) lparam);
        return 0;
    }

    case GEM_SETVALIDMINMAX:
    {
        _TEDT_OnSetValidMinMax(wnd, (g2t_int) wparam, (g2t_validminmax*) lparam);
        return 0;
    }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}