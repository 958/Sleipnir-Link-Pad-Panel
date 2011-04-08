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


#define SPX_PLUGIN_UNKNOWN			0x00000000	/* �s���ȃv���O�C�� */
#define SPX_PLUGIN_EXTENDED_PANEL	0x00000001	/* �g���p�l���p�v���O�C�� */
#define SPX_PLUGIN_INTELLI_SEARCH	0x00000002	/* IntelliSearch �p�v���O�C�� */

#define SPX_EXTENDED_PANEL_VERSION	  0			/* �g���p�l���v���O�C���o�[�W���� */
#define SPX_INTELLI_SEARCH_VERSION	  0			/* IntelliSearch �v���O�C���o�[�W���� */



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


#define SPX_ERR_CODE_SUCCESS		0	/* ���� */
#define SPX_ERR_CODE_FAILED			1	/* ���s */
#define SPX_ERR_CODE_MEMORY			2	/* �������s�� */
#define SPX_ERR_CODE_NOT_SUPPORT	3	/* ���T�|�[�g�@�\ */



/*---------------------------------------------------------------------------*/
/* Sleipnir Messages                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------
 * ����  : Sleipnir �̃o�[�W�������擾
 * First : SPX_ULONG*
 *         �o�[�W�������(1.60 => 160)
 * Second: �Ȃ�
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETVERSION			0x00000001


/*---------------------------------------------------------
 * ����  : �J����Ă���y�[�W�����擾
 * First : SPX_ULONG*
 *         �y�[�W��
 * Second: �Ȃ�
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETPAGECOUNT			0x00000002


/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�� IWebBrowser2 ���擾
 * First : IWebBrowser2 ���擾����y�[�W�ԍ�
 * Second: SPX_VOID**
 *         IWebBrowser2 �ւ̃|�C���^
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETBROWSERINTERFACE	0x00000003


/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�Ńi�r�Q�[�g�J�n
 * First : SPX_ULONG
 *         �i�r�Q�[�g���s���y�[�W�ԍ�
 * Second: SPX_CHAR*
 *         URI ������������
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_NAVIGATE				0x00000004


/*---------------------------------------------------------
 * ����  : �C�ӂ� URI �ŐV�����y�[�W���쐬
 * First : SPX_BOOL
 *         �A�N�e�B�u�ɂ��邩
 * Second: SPX_CHAR*
 *         URI ������������
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_NEWWINDOW			0x00000005


/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�����
 * First : SPX_ULONG
 *         ����y�[�W�ԍ�
 * Second: �Ȃ�
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_CLOSEPAGE			0x00000006


/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�� URL ���擾����
 * First : SPX_ULONG
 *         �y�[�W�ԍ�
 * Second: SRX_CHAR**
 *         URL ���i�[���ꂽ�A�h���X
 *         ISleipnir::Free �ŊJ�����邱��
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETURL				0x00000007


/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�̃^�C�g�����擾����
 * First : SPX_ULONG
 *         �y�[�W�ԍ�
 * Second: SRX_CHAR**
 *         �^�C�g�����i�[���ꂽ�A�h���X
 *         ISleipnir::Free �ŊJ�����邱��
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETTITLE				0x00000008



/*---------------------------------------------------------
 * ����  : �A�N�e�B�u�ȃy�[�W���擾
 * First : SPX_ULONG*
 *         �y�[�W�ԍ�
 * Second: �Ȃ�
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETACTIVEPAGE		0x00000009



/*---------------------------------------------------------
 * ����  : �A�N�e�B�u�ȃy�[�W��ݒ�
 * First : SPX_ULONG
 *         �y�[�W�ԍ�
 * Second: �Ȃ�
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_SETACTIVEPAGE		0x00000010



/*---------------------------------------------------------
 * ����  : �C�ӂ� URI �ŐV�����y�[�W���쐬
 * First : SPX_ULONG*
 *         �쐬�����y�[�W�̔ԍ�
 * Second: SPX_CHAR*
 *         URI ������������
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_NEWWINDOW2			0x00000011



/*---------------------------------------------------------
 * ����  : �w��̃y�[�W�̃A�v���P�[�V�����I�u�W�F�N�g�� 
 *         LPDISPATCH ���擾
 * First : �擾����y�[�W�ԍ�
 * Second: SPX_VOID**
 *         LPDISPATCH
 * �߂�l: �G���[�R�[�h
---------------------------------------------------------*/

#define SPX_SM_GETAPPINTERFACE		0x00000012



/*---------------------------------------------------------
 * ����  : ����̃t�H���_���擾����
 * First : SPX_ULONG
 *         �擾�������t�H���_ ID
 * Second: SPX_CHAR**
 *         �t�H���_��
 *         ISleipnir::Free �ŊJ�����邱��
 * �߂�l: �G���[�R�[�h
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


/* Sleipnir �C�x���g */

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
