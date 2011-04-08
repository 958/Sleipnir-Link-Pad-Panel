/*-----------------------------------------------------------------------------
	Sleipnir Link Pad Panel Plugin by 958
 -----------------------------------------------------------------------------*/
#include <stdio.h>
#define _INC_OLE
#include <windows.h>
#undef	_INC_OLE
#include <shlobj.h>
#include <commctrl.h>
#include <Shlwapi.h>

#include "SPlugin.h"
#include "OleDragDrop.h"

#ifdef __cplusplus
#define DLLExport extern "C" __declspec (dllexport)
#else
#define DLLExport __declspec (dllexport)
#endif

typedef struct {
	BOOL bNewWindow;
	BOOL bRemove;
	BOOL bRestore;
	BOOL bAutoSave;
} ST_SETTING;

#define BUF_MAX			1024

#define WM_DRAGDROP		(WM_APP + 100)

// メニューアイテムID
#define ID_NEWWINDOW		100
#define ID_REMOVE			101
#define ID_RESTORE			102
#define ID_AUTO_SAVE		103
#define ID_SAVE				104

#define DROP_NONE			(-2)

/*---------------------------------------------------------------------------
	グローバル変数
 ---------------------------------------------------------------------------*/
HINSTANCE		g_hInstance	= NULL;
HWND			g_hWndParent	= NULL;
HWND			g_hWnd		= NULL;
HWND			g_hChildWnd	= NULL;
HFONT			g_hFont		= NULL;
SPX_ISleipnir	*g_ipPnir	= NULL;
HIMAGELIST		g_hImage	= NULL;
HMENU			g_hMenu		= NULL;
char			g_szIniPath[MAX_PATH] = "";
char			g_szTempPath[MAX_PATH] = "";
char			g_szTitle[MAX_PATH] = "\0";
ST_SETTING		g_stSetting;
BOOL			g_bDrop = FALSE;
WNDPROC			g_pDefWndProc	= NULL;

// D&D用
//HIMAGELIST	g_hDragImage	= NULL;	// ドラッグイメージを格納するポインタ
BOOL		g_bDrag			= FALSE;// ドラッグフラグ
int			g_nDragItem;			// ドラッグ中アイテム
int			g_nDropItem;			// ドロップ先アイテム

static BOOL GetPrivateProfileBool(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szDefault, LPCTSTR szPath);
static void WritePrivateProfileBool(LPCTSTR szSession, LPCTSTR szKey, BOOL bValue, LPCTSTR szPath);
static void InitListView(void);
static void LoadSetting(void);
static void SaveSetting(void);
static void AppendIniToList(LPCTSTR szFileName);
static void AppendTempToList(LPCTSTR szFileName);
static BOOL ReadLineFile(HANDLE hFile, LPTSTR pBuf, DWORD dwSize);
static void AddListViewItem(LPTSTR szTitle, LPTSTR szURL, int nIndex);

/*---------------------------------------------------------------------------
	エントリーポイント
 ---------------------------------------------------------------------------*/
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		g_hInstance = hinstDLL;
	}
	
	return TRUE;
}

/*---------------------------------------------------------------------------
	プラグイン情報取得
 ---------------------------------------------------------------------------*/
DLLExport SPX_USHORT SPX_CALL SPX_GetPluginInfo()
{
	return MAKEWORD(SPX_PLUGIN_EXTENDED_PANEL, SPX_EXTENDED_PANEL_VERSION);
}

/*---------------------------------------------------------------------------
	WM_SIZE
 ---------------------------------------------------------------------------*/
static void OnSize(int cx, int cy)
{
	if (IsWindow(g_hChildWnd)) {
		MoveWindow(g_hChildWnd, 0, 0, cx, cy, TRUE);
	}
}

static void ShowItem(int nItem)
{
	char szBuf[BUF_MAX];
	BOOL bResult = TRUE;

	BOOL bNewWindow = ((GetKeyState(VK_SHIFT) & 0x8000) == 0) ? g_stSetting.bNewWindow : (g_stSetting.bNewWindow == TRUE) ? FALSE : TRUE;

	ListView_GetItemText(g_hChildWnd, nItem, 1, szBuf, BUF_MAX - 1);
	if (bNewWindow == TRUE) {
		SPX_ULONG lResult = g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_NEWWINDOW, (SPX_ULONG) (((GetKeyState(VK_CONTROL) & 0x8000) == 0) ? FALSE : TRUE), (SPX_ULONG) szBuf);
//		if (g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_NEWWINDOW, (SPX_ULONG) (((GetKeyState(VK_CONTROL) & 0x8000) == 0) ? FALSE : TRUE), (SPX_ULONG) szBuf) != SPX_ERR_CODE_SUCCESS) {
//		if (lResult != SPX_ERR_CODE_SUCCESS) {
//			sprintf(szBuf, "%d", lResult);
//MessageBox(g_hWnd, szBuf, "Error", MB_OK | MB_ICONERROR);
//			bResult = FALSE;
//		}
	} else {
		SPX_ULONG	ulActive;
		g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_GETACTIVEPAGE, (SPX_ULONG) &ulActive, 0);
		g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_NAVIGATE, (SPX_ULONG) ulActive, (SPX_ULONG) szBuf);
	}
	if ((g_stSetting.bRemove == TRUE) && (bResult == TRUE)) {
		ListView_DeleteItem(g_hChildWnd, nItem);
		ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);

		if (g_stSetting.bAutoSave == TRUE) {
			SaveSetting();
		}
	}
}

static void DeleteItem(int nItem)
{
	ListView_DeleteItem(g_hChildWnd, nItem);
	ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);

	if (g_stSetting.bAutoSave == TRUE) {
		SaveSetting();
	}
}

/*---------------------------------------------------------------------------
	WM_NOTIFY
 ---------------------------------------------------------------------------*/
static void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR phdr = (LPNMHDR) lParam;

	switch (phdr->code) {
	case NM_CLICK:
		{
			LPNMLISTVIEW pnmlv = (LPNMLISTVIEW) lParam;
			if (pnmlv->iItem != -1) {
				ShowItem(pnmlv->iItem);
			}
		}
		break;
	case NM_RCLICK:
		{
			LPNMLISTVIEW pnmlv = (LPNMLISTVIEW) lParam;
			if (pnmlv->iItem != -1) {
				DeleteItem(pnmlv->iItem);
			} else {
				// 設定メニュー表示
				POINT pos;
				SetForegroundWindow(g_hWnd);
				GetCursorPos(&pos);
				TrackPopupMenu(g_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
					pos.x, pos.y, 0, g_hWnd, NULL);
			}
		}
		break;
	case LVN_KEYDOWN:
		{
			switch (((LPNMLVKEYDOWN) lParam)->wVKey) {
			case VK_RETURN:
				{
					int nCnt;
					int nMax;
					for (nCnt = 0, nMax = ListView_GetItemCount(g_hChildWnd); nCnt < nMax; nCnt++) {
						if (ListView_GetItemState(g_hChildWnd, nCnt, LVIS_FOCUSED) != 0) {
							ShowItem(nCnt);
							break;
						}
					}
				}
				break;
			case VK_DELETE:
				{
					int nCnt;
					int nMax;
					for (nCnt = 0, nMax = ListView_GetItemCount(g_hChildWnd); nCnt < nMax; nCnt++) {
						if (ListView_GetItemState(g_hChildWnd, nCnt, LVIS_FOCUSED) != 0) {
							DeleteItem(nCnt);
							break;
						}
					}
				}
				break;
			case VK_APPS:
				{
					// 設定メニュー表示
					RECT rc;
					SetForegroundWindow(g_hWnd);
					GetWindowRect(g_hChildWnd, &rc);
					TrackPopupMenu(g_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
						rc.left, rc.top, 0, g_hWnd, NULL);
				}
				break;
			}
		}
		break;
	case LVN_GETINFOTIP:
		{
			static char szToolTip[BUF_MAX*2];
			char szTemp[BUF_MAX];
			LPNMLVGETINFOTIP pnmit = (LPNMLVGETINFOTIP) lParam;
			ListView_GetItemText(pnmit->hdr.hwndFrom, pnmit->iItem, 0, szToolTip, BUF_MAX - 1);
			ListView_GetItemText(pnmit->hdr.hwndFrom, pnmit->iItem, 1, szTemp, BUF_MAX - 1);
			if (StrCmpN(szTemp, szToolTip, lstrlen(szToolTip)) != 0) {
				lstrcat(szToolTip, "\n");
				lstrcat(szToolTip, szTemp);
			}
			pnmit->pszText = szToolTip;
			pnmit->cchTextMax = BUF_MAX;
		}
		break;
	case LVN_BEGINDRAG:
		{
			LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)lParam;
#if 0
			POINT ptPos;
			POINT ptCursor;
			long lX;
			long lY;

			g_hDragImage = ListView_CreateDragImage(pnmlv->hdr.hwndFrom, pnmlv->iItem, &ptPos);
			// ドラッグ開始
			GetCursorPos(&ptCursor);
			ScreenToClient(g_hWnd, &ptCursor);

			lX = ptCursor.x - ptPos.x;
			lY = ptCursor.y - ptPos.y;

			ImageList_BeginDrag(g_hDragImage,0,lX,lY);
			ImageList_DragEnter(g_hWnd, 0, 0);
#endif
#if 0
			POINT pt = pnmlv->ptAction;

			ImageList_BeginDrag(g_hImage, 0, 0, 0);						// ドラッグ開始
			ClientToScreen(GetParent(g_hChildWnd), &pt);
			ImageList_DragEnter(NULL, pt.x, pt.y);						// ウィンドウをロック
#endif

			g_bDrag = TRUE;												// ドラッグ中フラグをON
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			SetCapture(GetParent(g_hChildWnd));
			g_nDragItem = pnmlv->iItem;
			g_nDropItem = DROP_NONE;
		}
		break;
	}
}

/*---------------------------------------------------------------------------
	WM_MOUSEMOVE
 ---------------------------------------------------------------------------*/
static void OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (g_bDrag == TRUE) {
#if 0
		POINT Point={LOWORD(lParam), HIWORD(lParam)};
		RECT Rect;

		ClientToScreen(g_hWnd, &Point);
		GetWindowRect(g_hWnd, &Rect);
		ImageList_DragMove(Point.x-Rect.left, Point.y-Rect.top);
#endif
		POINT pt;
		GetCursorPos(&pt);											// カーソル位置取得
#if 0
		ImageList_DragMove(pt.x, pt.y);								// ドラッグイメージ移動
#endif
		{
			LVHITTESTINFO lvhi;
			lvhi.pt = pt;
			ScreenToClient(g_hChildWnd, &lvhi.pt);
			ListView_HitTest(g_hChildWnd, &lvhi);
			if (g_nDropItem != lvhi.iItem) {
				if (g_nDropItem != -1) {
					ListView_SetItemState(g_hChildWnd, g_nDropItem, ~LVIS_DROPHILITED, LVIS_DROPHILITED);
				}
				if (lvhi.flags & LVHT_ONITEM) {
					if (lvhi.iItem != -1) {
						ListView_SetItemState(g_hChildWnd, lvhi.iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
					}
					if (lvhi.iItem == g_nDragItem) {
						SetCursor(LoadCursor(NULL, IDC_NO));
					} else {
						SetCursor(LoadCursor(NULL, IDC_ARROW));
					}
				} else {
					lvhi.iItem = DROP_NONE;
					SetCursor(LoadCursor(NULL, IDC_NO));
				}
				g_nDropItem = lvhi.iItem;
			}
		}
	}
}

/*---------------------------------------------------------------------------
	WM_LBUTTONUP
 ---------------------------------------------------------------------------*/
static void OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (g_bDrag == TRUE) {											// ドラッグ中 ?
		g_bDrag = FALSE;											// ドラッグ中フラグをOFF

		SetCursor(NULL);

		ReleaseCapture();											// マウスキャプチャーを解放

#if 0
		ImageList_DragLeave(NULL);									// ウィンドウのロックを解除
		ImageList_EndDrag();
#endif
#if 0
		if (g_hDragImage) {											// ドラッグイメージの破棄
			ImageList_Destroy(g_hDragImage);
			g_hDragImage = NULL;
		}
#endif
		if ((g_nDropItem != DROP_NONE) && (g_nDragItem != g_nDropItem)) {
			char szTitle[BUF_MAX];
			char szURL[BUF_MAX];
			ListView_GetItemText(g_hChildWnd, g_nDragItem, 0, szTitle, BUF_MAX - 1);
			ListView_GetItemText(g_hChildWnd, g_nDragItem, 1, szURL, BUF_MAX - 1);

			ListView_SetItemState(g_hChildWnd, g_nDropItem, ~LVIS_DROPHILITED, LVIS_DROPHILITED);

			AddListViewItem(szTitle, szURL, g_nDropItem + 1);
			if (g_nDragItem > g_nDropItem) {
				g_nDragItem += 1;
			}
			ListView_DeleteItem(g_hChildWnd, g_nDragItem);

			if (g_stSetting.bAutoSave == TRUE) {
				SaveSetting();
			}
		}
	}
}

/*---------------------------------------------------------------------------
	WM_COMMAND
 ---------------------------------------------------------------------------*/
static void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MENUITEMINFO mii;

	mii.cbSize = sizeof (MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	switch (LOWORD(wParam)) {
	case ID_NEWWINDOW:
		if (g_stSetting.bNewWindow == TRUE) {
			g_stSetting.bNewWindow = FALSE;
			mii.fState = MFS_ENABLED;
			SetMenuItemInfo(g_hMenu, ID_NEWWINDOW, FALSE, &mii);
		} else {
			g_stSetting.bNewWindow = TRUE;
			mii.fState = MFS_ENABLED | MFS_CHECKED;
			SetMenuItemInfo(g_hMenu, ID_NEWWINDOW, FALSE, &mii);
		}
		break;
	case ID_REMOVE:
		if (g_stSetting.bRemove == TRUE) {
			g_stSetting.bRemove = FALSE;
			mii.fState = MFS_ENABLED;
			SetMenuItemInfo(g_hMenu, ID_REMOVE, FALSE, &mii);
		} else {
			g_stSetting.bRemove = TRUE;
			mii.fState = MFS_ENABLED | MFS_CHECKED;
			SetMenuItemInfo(g_hMenu, ID_REMOVE, FALSE, &mii);
		}
		break;
	case ID_RESTORE:
		if (g_stSetting.bRestore == TRUE) {
			g_stSetting.bRestore = FALSE;
			mii.fState = MFS_ENABLED;
			SetMenuItemInfo(g_hMenu, ID_RESTORE, FALSE, &mii);

			g_stSetting.bAutoSave = FALSE;
			mii.fState = MFS_DISABLED;
			SetMenuItemInfo(g_hMenu, ID_AUTO_SAVE, FALSE, &mii);
		} else {
			g_stSetting.bRestore = TRUE;
			mii.fState = MFS_ENABLED | MFS_CHECKED;
			SetMenuItemInfo(g_hMenu, ID_RESTORE, FALSE, &mii);

			mii.fState = MFS_ENABLED;
			SetMenuItemInfo(g_hMenu, ID_AUTO_SAVE, FALSE, &mii);
		}
		break;
	case ID_AUTO_SAVE:
		if (g_stSetting.bAutoSave == TRUE) {
			g_stSetting.bAutoSave = FALSE;
			mii.fState = MFS_ENABLED;
			SetMenuItemInfo(g_hMenu, ID_AUTO_SAVE, FALSE, &mii);
		} else {
			g_stSetting.bAutoSave = TRUE;
			mii.fState = MFS_ENABLED | MFS_CHECKED;
			SetMenuItemInfo(g_hMenu, ID_AUTO_SAVE, FALSE, &mii);
		}
		break;
	case ID_SAVE:
		SaveSetting();
		break;
	}
}

static void GetDnDFileName(LPTSTR szFileName, LPCTSTR szDropFile)
{
	LPTSTR szExt = PathFindExtension(szDropFile);
	if (StrCmpNI(szExt, ".url", 4) == 0) {
		LPTSTR szTemp = PathFindFileName(szDropFile);
		StrCpyN(szFileName, szTemp, lstrlen(szTemp) - 4 + 1);
	} else {
		szFileName[0] = '\0';
	}
}

/*---------------------------------------------------------------------------
	WM_DRAGDROP
 ---------------------------------------------------------------------------*/
static void OnDragDrop(WPARAM wParam, LPIDROPTARGET_NOTIFY pdtn)
{
	switch(wParam){
	case IDROPTARGET_NOTIFY_DRAGOVER:	/* マウスがウィンドウ内で移動したとき */
		{
			pdtn->dwEffect = DROPEFFECT_LINK;
		}
		break;

	case IDROPTARGET_NOTIFY_DROP:		/* ドロップされたとき */
		switch(pdtn->cfFormat) {
		case CF_TEXT:			/* テキスト */
			{
				int nCnt = ListView_GetItemCount(g_hChildWnd);
				LPTSTR pTitle;
				char *p = GlobalLock(pdtn->hMem);

				if(p == NULL){
					break;
				}
				/* 行追加 */
				pTitle = (g_szTitle[0] == '\0') ? p : g_szTitle;
				AddListViewItem(pTitle, p, nCnt);

				ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);

				g_szTitle[0] = '\0';

				GlobalUnlock(pdtn->hMem);

				if (g_stSetting.bAutoSave == TRUE) {
					SaveSetting();
				}
			}
			break;

		case CF_HDROP:			/* ファイル */
			{
				int i;
				char szFileName[MAX_PATH];

				for(i = 0;i < (int) DragQueryFile(pdtn->hMem,0xFFFFFFFF, NULL, 0);i++){
					DragQueryFile(pdtn->hMem,i, szFileName, sizeof(szFileName));

					GetDnDFileName(g_szTitle, szFileName);
					break;
				}
			}
			break;

		default:
			if (pdtn->cfFormat == RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR)) {
				/* file descriptor */
				int i;

				LPFILEGROUPDESCRIPTOR pp = GlobalLock(pdtn->hMem);
				if(pp == NULL){
					break;
				}
				for(i = 0;i < (int) pp->cItems; i++){
					GetDnDFileName(g_szTitle, pp->fgd[i].cFileName);
					break;
				}
				GlobalUnlock(pdtn->hMem);
			}
			break;
		}
	default:
		break;
	}
}

/*---------------------------------------------------------------------------
	親ウィンドウコールバック関数
 ---------------------------------------------------------------------------*/
static LRESULT CALLBACK WndCallbackProc(HWND hWnd, UINT unMsg, WPARAM wParam, LPARAM lParam)
{
	switch (unMsg) {
	case WM_SETFOCUS:
		SetForegroundWindow(g_hChildWnd);
		SetFocus(g_hChildWnd);
		break;

	case WM_SIZE:
		OnSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);
		break;
		
	case WM_NOTIFY:
		OnNotify(hWnd, wParam, lParam);
		break;

	case WM_COMMAND:
		OnCommand(hWnd, wParam, lParam);
		break;

	case WM_DRAGDROP:
		OnDragDrop(wParam, (LPIDROPTARGET_NOTIFY) lParam);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(hWnd, wParam, lParam);
		break;
	}

	return DefWindowProc(hWnd, unMsg, wParam, lParam);
}

/*---------------------------------------------------------------------------
	子ウィンドウコールバック関数
 ---------------------------------------------------------------------------*/
static LRESULT CALLBACK WndCallbackProcListView(HWND hWnd, UINT unMsg, WPARAM wParam, LPARAM lParam)
{
	switch (unMsg) {
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	}

	return CallWindowProc(g_pDefWndProc, hWnd, unMsg, wParam, lParam);
}

/*---------------------------------------------------------------------------
	SPX_Create
 ---------------------------------------------------------------------------*/
static SPX_HWND SPX_CALL SPX_CreateProc(SPX_ISleipnir* ipSleipnir, SPX_HWND hWndParent)
{
	const char	szClassName[] = "Link Pad Plugin";

	g_ipPnir = ipSleipnir;
	g_hWndParent = hWndParent;

	/* クラスの登録 */
	{
		WNDCLASS	tThisClass;
		tThisClass.style			= CS_HREDRAW | CS_VREDRAW;
		tThisClass.lpfnWndProc		= WndCallbackProc;
		tThisClass.cbClsExtra		= 0;
		tThisClass.cbWndExtra		= 0;
		tThisClass.hInstance		= g_hInstance;
		tThisClass.hIcon			= NULL;
		tThisClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
		tThisClass.hbrBackground	= GetStockObject(WHITE_BRUSH);
		tThisClass.lpszMenuName		= NULL;
		tThisClass.lpszClassName	= szClassName;

		RegisterClass(&tThisClass);
	}

	/* 親ウィンドウを作成する */
	g_hWnd = CreateWindow(
		szClassName,
		"",
		WS_CHILD|WS_VISIBLE,
		0,
		0,
		200,
		200,
		hWndParent,
		NULL,
		g_hInstance,
		NULL);
 
 	if (g_hWnd == NULL)
 		return NULL;

	/* 子ウィンドウを作成する */
	g_hChildWnd = CreateWindowEx(
		0,
		WC_LISTVIEW,
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL,
		0,
		0,
		200,
		200,
		g_hWnd,
		NULL,
		g_hInstance,
		0);

	if (g_hChildWnd == NULL) {
		DestroyWindow(g_hWnd);
		return NULL;
	}
	/* OLE D&D 登録 */
	{
		UINT		cf[2];

		cf[0] = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);	/* FileDiscriptor */
		cf[1] = CF_HDROP;	/* ファイル */
		cf[2] = CF_TEXT;	/* テキスト */

		if(OLE_IDropTarget_RegisterDragDrop(g_hWnd, g_hWnd, WM_DRAGDROP, cf, 3) == FALSE){
			MessageBox(g_hWnd,"ドロップターゲットの登録に失敗しました。","Error",MB_OK | MB_ICONERROR);
			DestroyWindow(g_hChildWnd);
			DestroyWindow(g_hWnd);
			return NULL;
		}
	}

	/* リストビュー初期化 */
	InitListView();

	/* iniから設定を取得 */
	{
		SPX_CHAR *pBuf;
		g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_GETSPECIALFOLDER, (SPX_ULONG) SPX_SF_INI, (SPX_ULONG) &pBuf);
		PathCombine(g_szIniPath, pBuf, "linkpad.ini");
		g_ipPnir->Free(pBuf);

		LoadSetting();
	}

	/* フォントを割り当てる */
	{
		LOGFONT		logfont;
		// アイコンフォント
		SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
		g_hFont = CreateFontIndirect(&logfont);
		SendMessage(g_hChildWnd, WM_SETFONT, (WPARAM) g_hFont, (LPARAM) FALSE);
	}

	/* メニュー作成 */
	{
		MENUITEMINFO mii;

		g_hMenu = CreatePopupMenu();

		mii.cbSize = sizeof (MENUITEMINFO);

		mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED | ((g_stSetting.bNewWindow == TRUE) ? MFS_CHECKED : 0);
		mii.wID = ID_NEWWINDOW;
		mii.dwTypeData = "新規ウィンドウで開く(&N)";
		InsertMenuItem(g_hMenu, 0, TRUE, &mii);

		mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED | ((g_stSetting.bRemove == TRUE) ? MFS_CHECKED : 0);
		mii.wID = ID_REMOVE;
		mii.dwTypeData = "リンクを開いたらリストから削除する(&D)";
		InsertMenuItem(g_hMenu, 1, TRUE, &mii);

		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		InsertMenuItem(g_hMenu, 2, TRUE, &mii);

		mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED | ((g_stSetting.bRestore == TRUE) ? MFS_CHECKED : 0);
		mii.wID = ID_RESTORE;
		mii.dwTypeData = "起動時に前回の状態を復元する(&R)";
		InsertMenuItem(g_hMenu, 3, TRUE, &mii);

		mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.fState = ((g_stSetting.bRestore == TRUE) ? MFS_ENABLED : MFS_DISABLED) |
					((g_stSetting.bAutoSave == TRUE) ? MFS_CHECKED : 0);
		mii.wID = ID_AUTO_SAVE;
		mii.dwTypeData = "随時保存する(&A)";
		InsertMenuItem(g_hMenu, 4, TRUE, &mii);

		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		InsertMenuItem(g_hMenu, 5, TRUE, &mii);

		mii.fMask = MIIM_STATE | MIIM_ID | MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.wID = ID_SAVE;
		mii.dwTypeData = "保存(&S)";
		InsertMenuItem(g_hMenu, 6, TRUE, &mii);
	}

	{
		SPX_CHAR *pBuf;
		g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_GETSPECIALFOLDER, (SPX_ULONG) SPX_SF_INI, (SPX_ULONG) &pBuf);
		PathCombine(g_szTempPath, pBuf, "linkpad.tmp");
		g_ipPnir->Free(pBuf);
	}
	AppendTempToList(g_szTempPath);

	/* 親ウィンドウを返す */
	return g_hWnd;
}

/*---------------------------------------------------------------------------
	SPX_Destroy
---------------------------------------------------------------------------*/
static SPX_LONG SPX_CALL SPX_DestroyProc(SPX_ISleipnir* ipSleipnir, SPX_HWND hWnd)
{
	SaveSetting();

	DestroyMenu(g_hMenu);
	DeleteObject(g_hFont);
	ImageList_Destroy(g_hImage);
	OLE_IDropTarget_RevokeDragDrop(g_hWnd);
	DestroyWindow(g_hChildWnd);
	DestroyWindow(g_hWnd);

	return SPX_ERR_CODE_SUCCESS;
}

/*---------------------------------------------------------------------------
	SPX_Idle
---------------------------------------------------------------------------*/
static SPX_LONG SPX_CALL SPX_IdleProc(SPX_ISleipnir* ipSleipnir, SPX_HWND hWnd)
{
	AppendTempToList(g_szTempPath);

	return SPX_ERR_CODE_SUCCESS;
}

/*---------------------------------------------------------------------------
	プラグイン初期化
 ---------------------------------------------------------------------------*/
DLLExport SPX_LONG SPX_CALL SPX_Initialize(SPX_IBrowserEventListener* ipEvent)
{
	ipEvent->OnCreate	= SPX_CreateProc;
	ipEvent->OnDestroy	= SPX_DestroyProc;
	ipEvent->OnIdle		= SPX_IdleProc;

	return SPX_ERR_CODE_SUCCESS;
}

static BOOL GetPrivateProfileBool(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szDefault, LPCTSTR szPath)
{
	BOOL bResult = FALSE;
	char szTemp[BUF_MAX];

	GetPrivateProfileString(szSession, szKey, szDefault, szTemp, BUF_MAX - 1, szPath);

	if (StrCmpNI(szTemp, "True", 4) == 0) {
		bResult = TRUE;
	}
	return bResult;
}

static void WritePrivateProfileBool(LPCTSTR szSession, LPCTSTR szKey, BOOL bValue, LPCTSTR szPath)
{
	char szTemp[BUF_MAX];

	if (bValue == TRUE) {
		StrCpyN(szTemp, "True", 4 + 1);
	} else {
		StrCpyN(szTemp, "False", 5 + 1);
	}
	WritePrivateProfileString(szSession, szKey, szTemp, szPath);
}

static void InitListView(void)
{
	g_pDefWndProc = (WNDPROC) SetWindowLong(g_hChildWnd, GWL_WNDPROC, (LONG) WndCallbackProcListView);

	/* アイコン設定 */
	{
		char szSkinName[BUF_MAX];
		char szIco[MAX_PATH];
		/* スキンのパスを取得 */
		{
			char szIni[MAX_PATH];
			SPX_CHAR *pBuf;
			g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_GETSPECIALFOLDER, (SPX_ULONG) SPX_SF_INI, (SPX_ULONG) &pBuf);
			PathCombine(szIni, pBuf, "user.ini");
			g_ipPnir->Free(pBuf);
			GetPrivateProfileString("General", "Skin", "default", szSkinName, BUF_MAX - 1, szIni);
		}
		/* アイコンのパスを取得 */
		{
			char szTemp[MAX_PATH];
			SPX_CHAR *pBuf;
			g_ipPnir->SendMessage((SPX_ULONG) SPX_SM_GETSPECIALFOLDER, (SPX_ULONG) SPX_SF_ACTIVESKIN, (SPX_ULONG) &pBuf);
			PathCombine(szTemp, pBuf, szSkinName);
			PathCombine(szIco, szTemp, ".\\page.ico");
			if (PathFileExists(szIco) == FALSE) {
				PathCombine(szIco, pBuf, "default\\page.ico");
			}
			g_ipPnir->Free(pBuf);
		}
		/* イメージリスト作成 */
		{
			HICON hIco = LoadImage(g_hInstance, szIco, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if (hIco != NULL) {
				g_hImage = ImageList_Create(16, 16, ILC_COLORDDB, 1, 0);
				ListView_SetImageList(g_hChildWnd, g_hImage, LVSIL_SMALL);
				ImageList_AddIcon(g_hImage, hIco);
				DeleteObject(hIco);
			}
		}
	}

	/* リストビュースタイルと初期化 */
	{
		LV_COLUMN	lvcol;
		ListView_SetExtendedListViewStyle(
			g_hChildWnd,
			LVS_EX_ONECLICKACTIVATE | LVS_EX_INFOTIP | LVS_EX_UNDERLINEHOT);

		lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.cx = 500;
		lvcol.pszText = "タイトル";
		lvcol.iSubItem = 0;
		ListView_InsertColumn(g_hChildWnd, 0, &lvcol);

		lvcol.cx = 0;
		lvcol.pszText = "URL";
		lvcol.iSubItem = 1;
		ListView_InsertColumn(g_hChildWnd, 1, &lvcol);

		ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);
	}
}

static void LoadSetting(void)
{
	g_stSetting.bNewWindow = GetPrivateProfileBool("General", "NewWindow", "True", g_szIniPath);;
	g_stSetting.bRemove = GetPrivateProfileBool("General", "Remove", "True", g_szIniPath);;
	g_stSetting.bRestore = GetPrivateProfileBool("General", "Restore", "True", g_szIniPath);;
	g_stSetting.bAutoSave = GetPrivateProfileBool("General", "AutoSave", "True", g_szIniPath);;

	/* リストビューへデータ読み込み */
	if (g_stSetting.bRestore == TRUE) {
		AppendIniToList(g_szIniPath);
	}
}

static void SaveSetting(void)
{
	/* 設定をiniに保存 */
	WritePrivateProfileBool("General", "NewWindow", g_stSetting.bNewWindow, g_szIniPath);
	WritePrivateProfileBool("General", "Remove", g_stSetting.bRemove, g_szIniPath);
	WritePrivateProfileBool("General", "Restore", g_stSetting.bRestore, g_szIniPath);
	WritePrivateProfileBool("General", "AutoSave", g_stSetting.bAutoSave, g_szIniPath);

	/* linkリストを保存 */
	if (g_stSetting.bRestore == TRUE) {
		char szKey[BUF_MAX];
		char szTitle[BUF_MAX];
		char szURL[BUF_MAX];
		int nCnt;
		int nMax = ListView_GetItemCount(g_hChildWnd);

		WritePrivateProfileSection("LinkList", "", g_szIniPath);

		for (nCnt = 0; nCnt < nMax; nCnt++) {
			ListView_GetItemText(g_hChildWnd, nCnt, 0, szTitle, BUF_MAX - 1);
			sprintf(szKey, "Title_%d", nCnt);
			WritePrivateProfileString("LinkList", szKey, szTitle, g_szIniPath);

			ListView_GetItemText(g_hChildWnd, nCnt, 1, szURL, BUF_MAX - 1);
			sprintf(szKey, "URL_%d", nCnt);
			WritePrivateProfileString("LinkList", szKey, szURL, g_szIniPath);
		}
	}
}

static void AddListViewItem(LPTSTR szTitle, LPTSTR szURL, int nIndex)
{
	LV_ITEM item;

	/* 行追加 */
	item.mask = LVIF_TEXT;
	item.pszText = szTitle;
	item.iItem = nIndex;
	item.iSubItem = 0;
	item.iImage = 0;
	ListView_InsertItem(g_hChildWnd, &item);

	item.pszText = szURL;
	item.iItem = nIndex;
	item.iSubItem = 1;
	ListView_SetItem(g_hChildWnd, &item);
}

static void AppendIniToList(LPCTSTR szFileName)
{
	char szKey[BUF_MAX];
	char szTitle[BUF_MAX];
	char szURL[BUF_MAX];
	int nCnt;

	for (nCnt = 0; ; nCnt++) {
		sprintf(szKey, "Title_%d", nCnt);
		GetPrivateProfileString("LinkList", szKey, "", szTitle, BUF_MAX - 1, szFileName);
		if (szTitle[0] == '\0') {
			break;
		}

		sprintf(szKey, "URL_%d", nCnt);
		GetPrivateProfileString("LinkList", szKey, "", szURL, BUF_MAX - 1, szFileName);
		if (szURL[0] == '\0') {
			break;
		}

		AddListViewItem(szTitle, szURL, nCnt);
	}

	ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);
}

static BOOL ReadLineFile(HANDLE hFile, LPTSTR pBuf, DWORD dwSize)
{
	BOOL	bResult = TRUE;
	DWORD	dwCnt;
	char	cPrev = '\0';

	memset(pBuf, '\0', dwSize);
	for (dwCnt = 0; dwCnt < dwSize ; dwCnt++, pBuf++) {
		BOOL	bRet;
		DWORD	dwRead;
		bRet = ReadFile(hFile, pBuf, 1, &dwRead, NULL);
		if ((bRet == FALSE) || (dwRead < 1)) {
			bResult = FALSE;
			break;
		} else if ((*pBuf == '\n') && (*(pBuf - 1) == '\r')) {
			*(pBuf - 1) = '\0';
			break;
		}
	}

	return bResult;
}

static void AppendTempToList(LPCTSTR szFileName)
{
	HANDLE hFile = CreateFile(
		szFileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	while (1) {
		char szTitle[BUF_MAX];
		char szURL[BUF_MAX];
		ReadLineFile(hFile, szTitle, BUF_MAX);
		ReadLineFile(hFile, szURL, BUF_MAX);
		if (szTitle[0] == '\0') {
			strcpy(szTitle, szURL);
		}
		if (szURL[0] == '\0') {
			break;
		}
		{
			int nMax = ListView_GetItemCount(g_hChildWnd);
			AddListViewItem(szTitle, szURL, nMax);
		}
	}
	CloseHandle(hFile);
	DeleteFile(szFileName);

	ListView_SetColumnWidth(g_hChildWnd, 0, LVSCW_AUTOSIZE);

	if (g_stSetting.bAutoSave == TRUE) {
		SaveSetting();
	}
}

/*---------------------------------------------------------------------------
	Visual C++ 用 エクスポート名の変換
 ---------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:SPX_GetPluginInfo=_SPX_GetPluginInfo@0")
#pragma comment(linker, "/EXPORT:SPX_Initialize=_SPX_Initialize@4")
#endif

