/// Copyright 2010-2017 - asm128
#include "nwol_display.h"
#include "nwol_log.h"

GDEFINE_ENUM_TYPE(WINDOWS_MESSAGE, uint32_t);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NULL							, 0x0000			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CREATE							, 0x0001			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DESTROY							, 0x0002			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOVE							, 0x0003			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SIZE							, 0x0005			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ACTIVATE						, 0x0006			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETFOCUS						, 0x0007			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KILLFOCUS						, 0x0008			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ENABLE							, 0x000A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETREDRAW						, 0x000B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETTEXT							, 0x000C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETTEXT							, 0x000D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETTEXTLENGTH					, 0x000E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PAINT							, 0x000F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CLOSE							, 0x0010			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUERYENDSESSION					, 0x0011			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUERYOPEN						, 0x0013			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ENDSESSION						, 0x0016			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUIT							, 0x0012			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ERASEBKGND						, 0x0014			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSCOLORCHANGE					, 0x0015			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SHOWWINDOW						, 0x0018			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, WININICHANGE					, 0x001A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DEVMODECHANGE					, 0x001B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ACTIVATEAPP						, 0x001C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, FONTCHANGE						, 0x001D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TIMECHANGE						, 0x001E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CANCELMODE						, 0x001F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETCURSOR						, 0x0020			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEACTIVATE					, 0x0021			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CHILDACTIVATE					, 0x0022			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUEUESYNC						, 0x0023			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETMINMAXINFO					, 0x0024			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PAINTICON						, 0x0026			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ICONERASEBKGND					, 0x0027			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NEXTDLGCTL						, 0x0028			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SPOOLERSTATUS					, 0x002A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DRAWITEM						, 0x002B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MEASUREITEM						, 0x002C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DELETEITEM						, 0x002D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, VKEYTOITEM						, 0x002E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CHARTOITEM						, 0x002F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETFONT							, 0x0030			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETFONT							, 0x0031			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETHOTKEY						, 0x0032			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETHOTKEY						, 0x0033			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUERYDRAGICON					, 0x0037			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COMPAREITEM						, 0x0039			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETOBJECT						, 0x003D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COMPACTING						, 0x0041			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COMMNOTIFY						, 0x0044			);/* no longer suported */
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, WINDOWPOSCHANGING				, 0x0046			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, WINDOWPOSCHANGED				, 0x0047			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POWER							, 0x0048			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COPYDATA						, 0x004A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CANCELJOURNAL					, 0x004B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NOTIFY							, 0x004E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INPUTLANGCHANGEREQUEST			, 0x0050			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INPUTLANGCHANGE					, 0x0051			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TCARD							, 0x0052			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HELP							, 0x0053			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, USERCHANGED						, 0x0054			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NOTIFYFORMAT					, 0x0055			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CONTEXTMENU						, 0x007B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, STYLECHANGING					, 0x007C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, STYLECHANGED					, 0x007D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DISPLAYCHANGE					, 0x007E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETICON							, 0x007F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SETICON							, 0x0080			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCCREATE						, 0x0081			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCDESTROY						, 0x0082			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCCALCSIZE						, 0x0083			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCHITTEST						, 0x0084			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCPAINT							, 0x0085			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCACTIVATE						, 0x0086			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETDLGCODE						, 0x0087			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYNCPAINT						, 0x0088			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHDESTROYWINDOW				, 0x0090			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHDRAWMENU						, 0x0091			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHDRAWMENUITEM					, 0x0092			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHINITMENU						, 0x0093			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHMEASUREMENUITEM				, 0x0094			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UAHNCPAINTMENUPOPUP				, 0x0095			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMOUSEMOVE						, 0x00A0			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCLBUTTONDOWN					, 0x00A1			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCLBUTTONUP						, 0x00A2			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCLBUTTONDBLCLK					, 0x00A3			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCRBUTTONDOWN					, 0x00A4			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCRBUTTONUP						, 0x00A5			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCRBUTTONDBLCLK					, 0x00A6			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMBUTTONDOWN					, 0x00A7			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMBUTTONUP						, 0x00A8			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMBUTTONDBLCLK					, 0x00A9			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCXBUTTONDOWN					, 0x00AB			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCXBUTTONUP						, 0x00AC			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCXBUTTONDBLCLK					, 0x00AD			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INPUT_DEVICE_CHANGE				, 0x00FE			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INPUT							, 0x00FF			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KEYFIRST						, 0x0100			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KEYDOWN							, 0x0100			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KEYUP							, 0x0101			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CHAR							, 0x0102			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DEADCHAR						, 0x0103			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSKEYDOWN						, 0x0104			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSKEYUP						, 0x0105			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSCHAR							, 0x0106			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSDEADCHAR						, 0x0107			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UNICHAR							, 0x0109			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KEYLAST							, 0x0109			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, KEYLAST_2						, 0x0108			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_STARTCOMPOSITION			, 0x010D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_ENDCOMPOSITION				, 0x010E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_COMPOSITION					, 0x010F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_KEYLAST						, 0x010F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INITDIALOG						, 0x0110			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COMMAND							, 0x0111			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SYSCOMMAND						, 0x0112			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TIMER							, 0x0113			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HSCROLL							, 0x0114			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, VSCROLL							, 0x0115			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INITMENU						, 0x0116			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, INITMENUPOPUP					, 0x0117			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GESTURE							, 0x0119			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GESTURENOTIFY					, 0x011A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENUSELECT						, 0x011F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENUCHAR						, 0x0120			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ENTERIDLE						, 0x0121			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENURBUTTONUP					, 0x0122			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENUDRAG						, 0x0123			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENUGETOBJECT					, 0x0124			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UNINITMENUPOPUP					, 0x0125			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MENUCOMMAND						, 0x0126			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CHANGEUISTATE					, 0x0127			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UPDATEUISTATE					, 0x0128			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUERYUISTATE					, 0x0129			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORMSGBOX					, 0x0132			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLOREDIT					, 0x0133			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORLISTBOX					, 0x0134			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORBTN						, 0x0135			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORDLG						, 0x0136			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORSCROLLBAR				, 0x0137			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CTLCOLORSTATIC					, 0x0138			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEFIRST						, 0x0200			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEMOVE						, 0x0200			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, LBUTTONDOWN						, 0x0201			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, LBUTTONUP						, 0x0202			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, LBUTTONDBLCLK					, 0x0203			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, RBUTTONDOWN						, 0x0204			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, RBUTTONUP						, 0x0205			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, RBUTTONDBLCLK					, 0x0206			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MBUTTONDOWN						, 0x0207			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MBUTTONUP						, 0x0208			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MBUTTONDBLCLK					, 0x0209			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEWHEEL						, 0x020A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, XBUTTONDOWN						, 0x020B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, XBUTTONUP						, 0x020C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, XBUTTONDBLCLK					, 0x020D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEHWHEEL						, 0x020E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSELAST_0						, 0x020E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSELAST_1						, 0x020D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSELAST_2						, 0x020A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSELAST_3						, 0x0209			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PARENTNOTIFY					, 0x0210			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ENTERMENULOOP					, 0x0211			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, EXITMENULOOP					, 0x0212			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NEXTMENU						, 0x0213			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SIZING							, 0x0214			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CAPTURECHANGED					, 0x0215			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOVING							, 0x0216			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POWERBROADCAST					, 0x0218			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DEVICECHANGE					, 0x0219			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDICREATE						, 0x0220			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIDESTROY						, 0x0221			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIACTIVATE						, 0x0222			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIRESTORE						, 0x0223			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDINEXT							, 0x0224			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIMAXIMIZE						, 0x0225			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDITILE							, 0x0226			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDICASCADE						, 0x0227			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIICONARRANGE					, 0x0228			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIGETACTIVE					, 0x0229			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDISETMENU						, 0x0230			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ENTERSIZEMOVE					, 0x0231			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, EXITSIZEMOVE					, 0x0232			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DROPFILES						, 0x0233			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MDIREFRESHMENU					, 0x0234			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERDEVICECHANGE				, 0x0238			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERDEVICEINRANGE			, 0x0239			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERDEVICEOUTOFRANGE			, 0x023A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TOUCH							, 0x0240			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCPOINTERUPDATE					, 0x0241			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCPOINTERDOWN					, 0x0242			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCPOINTERUP						, 0x0243			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERUPDATE					, 0x0245			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERDOWN						, 0x0246			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERUP						, 0x0247			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERENTER					, 0x0249			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERLEAVE					, 0x024A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERACTIVATE					, 0x024B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERCAPTURECHANGED			, 0x024C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TOUCHHITTESTING					, 0x024D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERWHEEL					, 0x024E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERHWHEEL					, 0x024F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERROUTEDTO					, 0x0251			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERROUTEDAWAY				, 0x0252			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, POINTERROUTEDRELEASED			, 0x0253			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_SETCONTEXT					, 0x0281			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_NOTIFY						, 0x0282			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_CONTROL						, 0x0283			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_COMPOSITIONFULL				, 0x0284			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_SELECT						, 0x0285			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_CHAR						, 0x0286			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_REQUEST						, 0x0288			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_KEYDOWN						, 0x0290			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, IME_KEYUP						, 0x0291			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSEHOVER						, 0x02A1			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, MOUSELEAVE						, 0x02A3			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMOUSEHOVER					, 0x02A0			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, NCMOUSELEAVE					, 0x02A2			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, WTSSESSION_CHANGE				, 0x02B1			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TABLET_FIRST					, 0x02c0			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, TABLET_LAST						, 0x02df			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DPICHANGED						, 0x02E0			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DPICHANGED_BEFOREPARENT			, 0x02E2			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DPICHANGED_AFTERPARENT			, 0x02E3			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETDPISCALEDSIZE				, 0x02E4			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CUT								, 0x0300			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, COPY							, 0x0301			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PASTE							, 0x0302			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CLEAR							, 0x0303			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, UNDO							, 0x0304			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, RENDERFORMAT					, 0x0305			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, RENDERALLFORMATS				, 0x0306			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DESTROYCLIPBOARD				, 0x0307			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DRAWCLIPBOARD					, 0x0308			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PAINTCLIPBOARD					, 0x0309			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, VSCROLLCLIPBOARD				, 0x030A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, SIZECLIPBOARD					, 0x030B			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, ASKCBFORMATNAME					, 0x030C			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CHANGECBCHAIN					, 0x030D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HSCROLLCLIPBOARD				, 0x030E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, QUERYNEWPALETTE					, 0x030F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PALETTEISCHANGING				, 0x0310			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PALETTECHANGED					, 0x0311			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HOTKEY							, 0x0312			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PRINT							, 0x0317			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PRINTCLIENT						, 0x0318			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, APPCOMMAND						, 0x0319			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, THEMECHANGED					, 0x031A			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, CLIPBOARDUPDATE					, 0x031D			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMCOMPOSITIONCHANGED			, 0x031E			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMNCRENDERINGCHANGED			, 0x031F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMCOLORIZATIONCOLORCHANGED		, 0x0320			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMWINDOWMAXIMIZEDCHANGE		, 0x0321			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMSENDICONICTHUMBNAIL			, 0x0323			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, DWMSENDICONICLIVEPREVIEWBITMAP	, 0x0326			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, GETTITLEBARINFOEX				, 0x033F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HANDHELDFIRST					, 0x0358			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, HANDHELDLAST					, 0x035F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, AFXFIRST						, 0x0360			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, AFXLAST							, 0x037F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PENWINFIRST						, 0x0380			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, PENWINLAST						, 0x038F			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, APP								, 0x8000			);
GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE, USER							, 0x0400			);
//GDEFINE_ENUM_VALUE(WINDOWS_MESSAGE,  WM_SETTINGCHANGE					, WM_WININICHANGE	);


#if defined(__WINDOWS__)
LRESULT											nwol::SDisplay::WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	verbose_printf("Received window message. hWnd = %p, uMsg = 0x%X, wParam = 0x%p, lParam = 0x%p. %s.", hWnd, uMsg, wParam, lParam, ::nwol::get_value_label((::WINDOWS_MESSAGE)uMsg).c_str());
	error_if(errored(PlatformDetail.Messages.push_back({hWnd, uMsg, wParam, lParam})), "Out of memory?");
	switch( uMsg ) {
	case WM_MENUCHAR		: return MAKELRESULT(0, MNC_CLOSE); // The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. Don't beep when we alt-enter.
	case WM_SYSKEYDOWN		: info_printf("Handling WM_SYSKEYDOWN."								); return 0;
	case WM_DESTROY			: info_printf("Handling WM_DESTROY. Doing nothing..."				); return 0;
	case WM_QUIT			: info_printf("Handling WM_QUIT."									); break;
	case WM_GETMINMAXINFO	: info_printf("Handling WM_GETMINMAXINFO."							); ((MINMAXINFO*)lParam)->ptMinTrackSize = {320, 200}; return 0;	// Catch this message so to prevent the window from becoming too small.
	case WM_CLOSE			: info_printf("Handling WM_CLOSE."									); State.NoDraw = State.Closed		= true; return 0;
	case WM_ENTERSIZEMOVE	: info_printf("Handling WM_ENTERSIZEMOVE."							); State.NoDraw = State.Resizing	= true; break;
	case WM_SHOWWINDOW		: info_printf("Handling WM_SHOWWINDOW. Fallback to WM_CREATE."		);
	case WM_CREATE			: info_printf("Handling WM_CREATE. Fallback to WM_EXITSIZEMOVE."	);
	case WM_EXITSIZEMOVE	: info_printf("Handling WM_EXITSIZEMOVE."							); State.Repaint = State.Resized = !(State.NoDraw = State.Resizing = false); break;
	case WM_PAINT			: info_printf("Handling WM_PAINT."									); State.Repaint = true; break;
	case WM_SIZE			:
		if(lParam) {
			::nwol::SCoord2<int16_t>							newMetrics									= ::nwol::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<int16_t>();
			if(newMetrics != Metrics.Size) {
				Metrics.Size									= newMetrics;
				State.Resized									= true;
				State.Repaint									= true; 
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			info_printf("SIZE_MINIMIZED.");
			State.MinOrMaxed = State.NoDraw					= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			info_printf("SIZE_MAXIMIZED.");
			State.Resized = State.MinOrMaxed				= true;
			State.NoDraw									= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			info_printf("SIZE_RESTORED.");
			State.Resized = State.MinOrMaxed				= true;
			State.NoDraw									= false;
		}
		else {
			info_printf("Handling unknown WM_SIZE message: 0x%p.", wParam);
			//State.Resized									= true;	? 
			State.MinOrMaxed = State.NoDraw					= false;
		}
		break;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
#endif
