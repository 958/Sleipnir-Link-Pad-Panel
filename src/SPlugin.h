/*-----------------------------------------------------------------------------
 *                                                                           *
 * Sleipnir Extended Plugin                                                  *
 *                                                                           *
 * Version 1.02                                                              *
 *                                                                           *
 * Copyright (C) 2004 by Yasuyuki Kashiwagi. All Rights Reserved.            *
 *                                                                           *
-----------------------------------------------------------------------------*/

#ifndef SPX_PLUGIN_H
#define SPX_PLUGIN_H


#ifdef __cplusplus
extern "C" {
#endif


#include <windows.h>


/*---------------------------------------------------------------------------*/
/* Plugin Information                                                        */
/*---------------------------------------------------------------------------*/


#define SPX_PLUGIN_UNKNOWN			0x00000000	/* 不明なプラグイン */
#define SPX_PLUGIN_EXTENDED_PANEL	0x00000001	/* 拡張パネル用プラグイン */
#define SPX_PLUGIN_INTELLI_SEARCH	0x00000002	/* IntelliSearch 用プラグイン */

#define SPX_EXTENDED_PANEL_VERSION	  0			/* 拡張パネルプラグインバージョン */
#define SPX_INTELLI_SEARCH_VERSION	  0			/* IntelliSearch プラグインバージョン */



/*---------------------------------------------------------------------------*/
/* Sleipnir Types                                                            */
/*---------------------------------------------------------------------------*/


#define SPX_CALL	WINAPI
#define SPX_ULONG	DWORD
#define SPX_UINT 	unsigned int
#define SPX_USHORT	WORD
#define SPX_UCHAR	BYTE
#define SPX_LONG	long
#define SPX_INT 	int
#define SPX_SHORT	short int
#define SPX_CHAR	char
#define SPX_VOID	void
#define SPX_BOOL	BOOL
#define SPX_VARIANT	VARIANT
#define SPX_HWND	HWND



/*---------------------------------------------------------------------------*/
/* Sleipnir Error Values                                                     */
/*---------------------------------------------------------------------------*/


#define SPX_ERR_CODE_SUCCESS		0	/* 成功 */
#define SPX_ERR_CODE_FAILED			1	/* 失敗 */
#define SPX_ERR_CODE_MEMORY			2	/* メモリ不足 */
#define SPX_ERR_CODE_NOT_SUPPORT	3	/* 未サポート機能 */



/*---------------------------------------------------------------------------*/
/* Sleipnir Messages                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------
 * 説明  : Sleipnir のバージョンを取得
 * First : SPX_ULONG*
 *         バージョン情報(1.60 => 160)
 * Second: なし
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETVERSION			0x00000001


/*---------------------------------------------------------
 * 説明  : 開かれているページ数を取得
 * First : SPX_ULONG*
 *         ページ数
 * Second: なし
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETPAGECOUNT			0x00000002


/*---------------------------------------------------------
 * 説明  : 指定のページの IWebBrowser2 を取得
 * First : IWebBrowser2 を取得するページ番号
 * Second: SPX_VOID**
 *         IWebBrowser2 へのポインタ
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETBROWSERINTERFACE	0x00000003


/*---------------------------------------------------------
 * 説明  : 指定のページでナビゲート開始
 * First : SPX_ULONG
 *         ナビゲートを行うページ番号
 * Second: SPX_CHAR*
 *         URI を示す文字列
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_NAVIGATE				0x00000004


/*---------------------------------------------------------
 * 説明  : 任意の URI で新しいページを作成
 * First : SPX_BOOL
 *         アクティブにするか
 * Second: SPX_CHAR*
 *         URI を示す文字列
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_NEWWINDOW			0x00000005


/*---------------------------------------------------------
 * 説明  : 指定のページを閉じる
 * First : SPX_ULONG
 *         閉じるページ番号
 * Second: なし
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_CLOSEPAGE			0x00000006


/*---------------------------------------------------------
 * 説明  : 指定のページの URL を取得する
 * First : SPX_ULONG
 *         ページ番号
 * Second: SRX_CHAR**
 *         URL が格納されたアドレス
 *         ISleipnir::Free で開放すること
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETURL				0x00000007


/*---------------------------------------------------------
 * 説明  : 指定のページのタイトルを取得する
 * First : SPX_ULONG
 *         ページ番号
 * Second: SRX_CHAR**
 *         タイトルが格納されたアドレス
 *         ISleipnir::Free で開放すること
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETTITLE				0x00000008



/*---------------------------------------------------------
 * 説明  : アクティブなページを取得
 * First : SPX_ULONG*
 *         ページ番号
 * Second: なし
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETACTIVEPAGE		0x00000009



/*---------------------------------------------------------
 * 説明  : アクティブなページを設定
 * First : SPX_ULONG
 *         ページ番号
 * Second: なし
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_SETACTIVEPAGE		0x00000010



/*---------------------------------------------------------
 * 説明  : 任意の URI で新しいページを作成
 * First : SPX_ULONG*
 *         作成したページの番号
 * Second: SPX_CHAR*
 *         URI を示す文字列
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_NEWWINDOW2			0x00000011



/*---------------------------------------------------------
 * 説明  : 指定のページのアプリケーションオブジェクトの 
 *         LPDISPATCH を取得
 * First : 取得するページ番号
 * Second: SPX_VOID**
 *         LPDISPATCH
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETAPPINTERFACE		0x00000012



/*---------------------------------------------------------
 * 説明  : 特定のフォルダを取得する
 * First : SPX_ULONG
 *         取得したいフォルダ ID
 * Second: SPX_CHAR**
 *         フォルダ名
 *         ISleipnir::Free で開放すること
 * 戻り値: エラーコード
---------------------------------------------------------*/

#define SPX_SM_GETSPECIALFOLDER		0x00000013

#define SPX_SF_APP					0x00000000
#define SPX_SF_INI					0x00000001
#define SPX_SF_ACTIVESKIN			0x00000002



/*---------------------------------------------------------------------------*/
/* Sleipnir Privided Interface                                               */
/*---------------------------------------------------------------------------*/


typedef SPX_ULONG (SPX_CALL* SPX_SendMessage)(SPX_ULONG qMsg, SPX_ULONG qFirst, SPX_ULONG qSecond);
typedef SPX_ULONG (SPX_CALL* SPX_PostMessage)(SPX_ULONG qMsg, SPX_ULONG qFirst, SPX_ULONG qSecond);
typedef SPX_VOID* (SPX_CALL* SPX_Malloc     )(SPX_ULONG qSize);
typedef SPX_LONG  (SPX_CALL* SPX_Free       )(SPX_VOID* vpMemory);
typedef SPX_INT   (SPX_CALL* SPX_MessageBox )(const SPX_CHAR* cpPrompt, SPX_UINT unFlag);


#pragma pack(1)

typedef struct SPX_ISleipnir_Tag
{
	SPX_ULONG qSize;

	SPX_SendMessage			SendMessage;
	SPX_PostMessage			PostMessage;
	SPX_Malloc				Malloc;
	SPX_Free				Free;
	SPX_MessageBox			MessageBox;

	SPX_UCHAR bPad[124];

} SPX_ISleipnir;

#pragma pack()



/*---------------------------------------------------------------------------*/
/* Browser Event Listener                                                    */
/*---------------------------------------------------------------------------*/


/* Sleipnir イベント */

typedef SPX_HWND (SPX_CALL* SPX_Create				)(SPX_ISleipnir* ipSleipnir, SPX_HWND hWndParent);
typedef SPX_LONG (SPX_CALL* SPX_Destroy				)(SPX_ISleipnir* ipSleipnir, SPX_HWND hWnd);
typedef SPX_LONG (SPX_CALL* SPX_Idle				)(SPX_ISleipnir* ipSleipnir, SPX_HWND hWnd);
typedef SPX_LONG (SPX_CALL* SPX_Property			)(SPX_ISleipnir* ipSleipnir, SPX_HWND hWnd, SPX_ULONG x, SPX_ULONG y);
typedef SPX_BOOL (SPX_CALL* SPX_PreTranslateMessage	)(MSG* pMsg);
typedef SPX_VOID (SPX_CALL* SPX_ProcessMessage		)();


#pragma pack(1)

typedef struct SPX_IBrowserEventListener_Tag
{
	SPX_ULONG					qSize;

	SPX_Create					OnCreate;
	SPX_Destroy					OnDestroy;
	SPX_Idle					OnIdle;
	SPX_Property				OnProperty;
	SPX_PreTranslateMessage		OnPreTranslateMessage;
	SPX_ProcessMessage			OnProcessMessage;

	SPX_UCHAR bPad[116];
	
} SPX_IBrowserEventListener;

#pragma pack()





#ifdef __cplusplus
}
#endif


#endif /* SPX_PLUGIN_H */
