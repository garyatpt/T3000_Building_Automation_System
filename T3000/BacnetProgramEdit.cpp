// BacnetProgramEdit.cpp : implementation file
// This File coded by Fance used to encode and decode programming.20130820

#include "stdafx.h"
#include "T3000.h"
#include "BacnetProgramEdit.h"
#include "BacnetProgram.h"
#include "afxdialogex.h"

#include "CM5/ud_str.h"
#include "Bacnet_Include.h"
#include "globle_function.h"
#include "gloab_define.h"
#include "BacnetProgramSetting.h"
extern int error;
extern char *pmes;
//extern int Station_NUM;
//extern int program_list_line;
vector <Str_char_pos_color> m_prg_char_color;	//用于highlight 关键字用;
vector <Str_char_pos_color> buffer_prg_char_color; //用于防止频繁更新界面引起的闪烁问题;
// CBacnetProgramEdit dialog
CString program_string;
CString AnalysisString;
 char editbuf[25000];
 extern char my_display[10240];
 extern int Encode_Program();
 extern int my_lengthcode;
 extern char mycode[2000];

extern void  init_info_table( void );
extern void Init_table_bank();
extern char mesbuf[1024];
extern int renumvar;
extern char *desassembler_program();
extern void copy_data_to_ptrpanel(int Data_type);
extern void check_high_light();
CString high_light_string;
HWND mParent_Hwnd;

bool show_upper;
DWORD prg_text_color;
DWORD prg_label_color;
DWORD prg_command_color;
DWORD prg_function_color;
CString prg_character_font;
bool prg_color_change;

IMPLEMENT_DYNAMIC(CBacnetProgramEdit, CDialogEx)

CBacnetProgramEdit::CBacnetProgramEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBacnetProgramEdit::IDD, pParent)
{
	Run_once_mutex = false;
}

CBacnetProgramEdit::~CBacnetProgramEdit()
{
}

void CBacnetProgramEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFORMATION_WINDOW, m_information_window);
	DDX_Control(pDX, IDC_STATIC_POOL_SIZE, m_pool_size);
	DDX_Control(pDX, IDC_STATIC_SIZE, m_program_size);
	DDX_Control(pDX, IDC_STATIC_FREE_MEMORY, m_free_memory);
}


BEGIN_MESSAGE_MAP(CBacnetProgramEdit, CDialogEx)
	ON_MESSAGE(WM_HOTKEY,&CBacnetProgramEdit::OnHotKey)//快捷键消息映射手动加入
	ON_MESSAGE(WM_REFRESH_BAC_PROGRAM_RICHEDIT,Fresh_Program_RichEdit)
	ON_MESSAGE(MY_RESUME_DATA, ProgramResumeMessageCallBack)
	ON_COMMAND(ID_SEND, &CBacnetProgramEdit::OnSend)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CLEAR, &CBacnetProgramEdit::OnClear)
	ON_COMMAND(ID_LOADFILE, &CBacnetProgramEdit::OnLoadfile)
	ON_COMMAND(ID_SAVEFILE, &CBacnetProgramEdit::OnSavefile)
	ON_EN_SETFOCUS(IDC_RICHEDIT2_PROGRAM, &CBacnetProgramEdit::OnEnSetfocusRichedit2Program)
	ON_COMMAND(ID_REFRESH, &CBacnetProgramEdit::OnRefresh)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_PROGRAM_IDE_SETTINGS, &CBacnetProgramEdit::OnProgramIdeSettings)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBacnetProgramEdit message handlers
LRESULT CBacnetProgramEdit::OnHotKey(WPARAM wParam,LPARAM lParam)
{
	if(Run_once_mutex == true)	//If already exist some function run,it can't show another one;
		return -1;
	if (wParam==KEY_F2)
	{
		Run_once_mutex = true;
		OnSend();
		Run_once_mutex = false;
	}
	else if(wParam == KEY_F3)
	{
		Run_once_mutex = true;
		OnClear();
		Run_once_mutex = false;
	}
	else if(wParam == KEY_F7)
	{
		Run_once_mutex = true;
		OnLoadfile();
		Run_once_mutex = false;
	}
	else if(wParam == KEY_F6)
	{
		Run_once_mutex = true;
		OnSavefile();
		Run_once_mutex = false;
	}
	else if(wParam == KEY_F8)
	{
		Run_once_mutex = true;
		OnRefresh();
		Run_once_mutex = false;
	}
	return 0;
}

void CBacnetProgramEdit::Initial_static()
{
	CString temp_cs_size,temp_cs_free;
	m_pool_size.SetWindowTextW(_T("2000"));
	m_pool_size.textColor(RGB(255,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_pool_size.setFont(15,10,NULL,_T("Arial"));

	temp_cs_size.Format(_T("%d"),bac_program_size);
	m_program_size.SetWindowTextW(temp_cs_size);
	m_program_size.textColor(RGB(255,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_program_size.setFont(15,10,NULL,_T("Arial"));

	temp_cs_free.Format(_T("%d"),bac_free_memory);
	m_free_memory.SetWindowTextW(temp_cs_free);
	m_free_memory.textColor(RGB(255,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_free_memory.setFont(15,10,NULL,_T("Arial"));
}
extern char my_panel;



void CBacnetProgramEdit::SetRicheditFont(long nStartchar,long nEndchar,DWORD nColor)
{
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask|=CFM_BOLD;

	cf.dwEffects&=~CFE_BOLD;
	//cf.dwEffects|=~CFE_BOLD; //粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwMask|=CFM_ITALIC;
	cf.dwEffects&=~CFE_ITALIC;
	//cf.dwEffects|=~CFE_ITALIC; //斜体，取消用cf.dwEffects&=~CFE_ITALIC;
	cf.dwMask|=CFM_UNDERLINE;
	cf.dwEffects&=~CFE_UNDERLINE;
	//cf.dwEffects|=~CFE_UNDERLINE; //斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
	cf.dwMask|=CFM_COLOR;
	cf.crTextColor = nColor;//RGB(0,0,255); //设置颜色
	cf.dwMask|=CFM_SIZE;
	cf.yHeight =250; //设置高度
	cf.dwMask|=CFM_FACE;
	//_tcscpy(cf.szFaceName ,_T("SimSun-ExtB"));
	//_tcscpy(cf.szFaceName ,_T("Times New Roman"));
	//	strcpy(cf.szFaceName ,_T("隶书")); //设置字体
	_tcscpy(cf.szFaceName , prg_character_font);
	//_tcscpy(cf.szFaceName ,_T("NSimSun"));
	//
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(nStartchar,nEndchar);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSelectionCharFormat(cf);

}


void CBacnetProgramEdit::GetColor()
{
	prg_text_color = (DWORD)GetPrivateProfileInt(_T("Program_IDE_Color"),_T("Text Color"),DEFAULT_PRG_TEXT_COLOR,g_cstring_ini_path);
	prg_label_color = (DWORD)GetPrivateProfileInt(_T("Program_IDE_Color"),_T("Label Color"),DEFAULT_PRG_LABEL_COLOR,g_cstring_ini_path);
	prg_command_color = (DWORD)GetPrivateProfileInt(_T("Program_IDE_Color"),_T("Command Color"),DEFAULT_PRG_COMMAND_COLOR,g_cstring_ini_path);
	prg_function_color = (DWORD)GetPrivateProfileInt(_T("Program_IDE_Color"),_T("Function Color"),DEFAULT_PRG_FUNCTION_COLOR,g_cstring_ini_path);
	show_upper = (DWORD)GetPrivateProfileInt(_T("Program_IDE_Color"),_T("Upper Case"),1,g_cstring_ini_path);
	GetPrivateProfileString(_T("Program_IDE_Color"),_T("Text Font"),_T("Arial"),prg_character_font.GetBuffer(MAX_PATH),MAX_PATH,g_cstring_ini_path);
	prg_character_font.ReleaseBuffer();
	
	bool found_font = false;
	for (int i=0;i< (sizeof(Program_Fonts) / sizeof(Program_Fonts[0])) ; i++ )
	{
		if(prg_character_font.CompareNoCase(Program_Fonts[i]) == 0 )
		{
			found_font = true;
			break;
		}
	}

	if(!found_font)
	{
		prg_character_font.Format(_T("Arial"));
		WritePrivateProfileString(_T("Program_IDE_Color"),_T("Text Font"),_T("Arial"),g_cstring_ini_path);
	}
}

BOOL CBacnetProgramEdit::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	CString ShowProgramText;
	CString temp_label;
	MultiByteToWideChar( CP_ACP, 0, (char *)m_Program_data.at(program_list_line).label,(int)strlen((char *)m_Program_data.at(program_list_line).label)+1, 
		temp_label.GetBuffer(MAX_PATH), MAX_PATH );
	temp_label.ReleaseBuffer();	
	if(temp_label.IsEmpty())
	{
		temp_label.Format(_T("PRG%d"),program_list_line + 1);
	}

	ShowProgramText.Format(_T("Panel :  %u      Program  :  %u      Name  :  "),Station_NUM,program_list_line + 1 );
	ShowProgramText = ShowProgramText + temp_label;
	SetWindowText(ShowProgramText);
	((CBacnetProgram*)pDialog[WINDOW_PROGRAM])->Unreg_Hotkey();
	prg_character_font.Empty();
	// TODO:  Add extra initialization here
	m_edit_changed = false;
	my_panel = bac_gloab_panel; //Set the panel number
	AnalysisString.Empty();
	GetDlgItem(IDC_RICHEDIT2_PROGRAM)->SetFocus();
	GetColor();
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask|=CFM_BOLD;

	cf.dwEffects&=~CFE_BOLD;
	//cf.dwEffects|=~CFE_BOLD; //粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwMask|=CFM_ITALIC;
	cf.dwEffects&=~CFE_ITALIC;
	//cf.dwEffects|=~CFE_ITALIC; //斜体，取消用cf.dwEffects&=~CFE_ITALIC;
	cf.dwMask|=CFM_UNDERLINE;
	cf.dwEffects&=~CFE_UNDERLINE;
	//cf.dwEffects|=~CFE_UNDERLINE; //斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
	cf.dwMask|=CFM_COLOR;
	cf.crTextColor = prg_text_color;//RGB(0,0,255); //设置颜色
	cf.dwMask|=CFM_SIZE;
	cf.yHeight =250; //设置高度
	cf.dwMask|=CFM_FACE;
	//_tcscpy(cf.szFaceName ,_T("SimSun-ExtB"));
	//_tcscpy(cf.szFaceName ,_T("Times New Roman"));
	//	strcpy(cf.szFaceName ,_T("隶书")); //设置字体
	_tcscpy(cf.szFaceName , prg_character_font);
	//_tcscpy(cf.szFaceName ,_T("NSimSun"));
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetDefaultCharFormat(cf); 


	//((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->PasteSpecial(CF_TEXT);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->PostMessage(WM_VSCROLL, SB_BOTTOM,0);


	RegisterHotKey(GetSafeHwnd(),KEY_F2,NULL,VK_F2);//F2键
	RegisterHotKey(GetSafeHwnd(),KEY_F3,NULL,VK_F3);
	RegisterHotKey(GetSafeHwnd(),KEY_F7,NULL,VK_F7);
	RegisterHotKey(GetSafeHwnd(),KEY_F6,NULL,VK_F6);
	RegisterHotKey(GetSafeHwnd(),KEY_F8,NULL,VK_F8);
	Initial_static();

	init_info_table();
	Init_table_bank();

	mParent_Hwnd = g_hwnd_now;
	prg_color_change = false;

	m_program_edit_hwnd = this->m_hWnd;
	g_hwnd_now = m_program_edit_hwnd;

	copy_data_to_ptrpanel(TYPE_ALL);
	memset(my_display,0,sizeof(my_display));
	PostMessage(WM_REFRESH_BAC_PROGRAM_RICHEDIT,NULL,NULL);

	//SetTimer(1,60000,NULL);
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CBacnetProgramEdit::Fresh_Program_RichEdit(WPARAM wParam,LPARAM lParam)
{
	//return 0;
	//	CString temp2;
	char * temp_point;
 	temp_point = desassembler_program();
	if(temp_point == NULL)
	{
		SetPaneString(BAC_SHOW_MISSION_RESULTS,_T("Decode Error!"));
		return 1;
	}
	SetPaneString(BAC_SHOW_MISSION_RESULTS,_T("Decode success!"));
	Initial_static();	
	CString temp;


	int  len = 0;
	len =  strlen(my_display); //str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,0, my_display,-1,NULL,0 );  
	::MultiByteToWideChar( CP_ACP,  0,my_display,-1,temp.GetBuffer(unicodeLen),unicodeLen ); 
	temp.ReleaseBuffer();
	CString temp1 = temp;
	if(show_upper)
	{
		temp1.MakeUpper();
	}
	else
		temp1.MakeLower();

	temp1.Replace(_T("\r\n"),_T("  \r\n"));
	temp1.Replace(_T("\("),_T(" \( "));
	temp1.Replace(_T("\)"),_T(" \) "));

	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetWindowTextW(temp1);
	m_edit_changed = false;
	program_string = temp1;
	UpdateDataProgramText();
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(-1,-1);
	return 0;
}

void CBacnetProgramEdit::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetFocus();

	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(-1,-1);
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->ReplaceSel(_T("\n"));
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetFocus();

	/*CDialogEx::OnOK();*/
}


LRESULT  CBacnetProgramEdit::ProgramResumeMessageCallBack(WPARAM wParam, LPARAM lParam)
{
	_MessageInvokeIDInfo *pInvoke =(_MessageInvokeIDInfo *)lParam;
	CString temp_cs = pInvoke->task_info;
	bool msg_result=WRITE_FAIL;
	msg_result = MKBOOL(wParam);
	CString Show_Results;
	if(msg_result)
	{
		//CString temp_ok;
		//temp_ok = _T("Bacnet operation success!   Request ID:") +  temp_cs;
		//m_Program_data.at(program_list_line).bytes =	program_code_length[program_list_line] - PRIVATE_HEAD_LENGTH;

		Show_Results = temp_cs + _T("Success!");
		SetPaneString(BAC_SHOW_MISSION_RESULTS,Show_Results);


		CString Edit_Buffer;
		GetDlgItemText(IDC_RICHEDIT2_PROGRAM,Edit_Buffer);
		program_string = Edit_Buffer;
		MessageBox(Show_Results);
//#endif
	}
	else
	{
		Show_Results = temp_cs + _T("Fail!");
		SetPaneString(BAC_SHOW_MISSION_RESULTS,Show_Results);
//#ifdef SHOW_ERROR_MESSAGE
		MessageBox(Show_Results);
//#endif
	}
	if(pInvoke)
		delete pInvoke;
	return 0;
}


void CBacnetProgramEdit::OnSend()
{
	// TODO: Add your command handler code here
	memset(program_code[program_list_line],0,2000);

	renumvar = 1;
	error = -1; //Default no error;
	CString tempcs;
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->GetWindowTextW(tempcs);
	tempcs.MakeUpper();
//	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_ACP,0,tempcs,-1,NULL,0,NULL,NULL );
	memset( ( void* )editbuf, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_ACP,0,tempcs,-1,editbuf,iTextLen,NULL,NULL );

	Encode_Program();
	if(error == -1)
	{
	
		TRACE(_T("Encode_Program length is %d ,copy length is %d\r\n"),program_code_length[program_list_line],my_lengthcode );
#if 0
		if(my_lengthcode <400)
			memcpy_s(program_code[program_list_line],my_lengthcode,mycode,my_lengthcode);
		else
		{
			CString temp_mycs;//如果npdu的长度过长，大于 400了，目前是通过换一个page来存;
			temp_mycs.Format(_T("Encode Program Code Length is %d,Please Try to code in another page."),my_lengthcode);
			MessageBox(temp_mycs);
			memset(program_code[program_list_line],0,400);
			return;
		}
#endif
		if(my_lengthcode > 2000)
		{
			MessageBox(_T("Encode Program Code Length is too large"));
			return;
		}
		memset(program_code[program_list_line],0,2000);
		memcpy_s(program_code[program_list_line],my_lengthcode,mycode,my_lengthcode);
		program_code_length[program_list_line] = program_code[program_list_line][1] *256 + (unsigned char)program_code[program_list_line][0];
		bac_program_size = program_code_length[program_list_line];// my_lengthcode;
		bac_free_memory = 2000 - bac_program_size;


		int npart = (my_lengthcode / 401) + 1;

		bool b_program_status = true;

		if(g_protocol == PROTOCOL_BIP_TO_MSTP)
		{
			for (int j=0;j<npart;j++)
			{
			   int n_ret = 0;
			   n_ret = WriteProgramData_Blocking(g_bac_instance,WRITEPROGRAMCODE_T3000,program_list_line,program_list_line,j);
			   if(n_ret< 0)
			   {
				   MessageBox(_T("Write Program Code Timeout!"));
				   return;
			   }
			}
			b_program_status = true;
		}
		else
		{
			for (int j=0;j<npart;j++)
			{
				int send_status = true;
				int resend_count = 0;
				int temp_invoke_id = -1;
				do 
				{
					resend_count ++;
					if(resend_count>5)
					{
						send_status = false;
						b_program_status = false;
						MessageBox(_T("Write Program Code Timeout!"));
						return;
					}
					temp_invoke_id =  WriteProgramData(g_bac_instance,WRITEPROGRAMCODE_T3000,program_list_line,program_list_line,j);

					Sleep(SEND_COMMAND_DELAY_TIME);
				} while (temp_invoke_id<0);

				if(send_status)
				{
					for (int i=0;i<3000;i++)
					{
						Sleep(1);
						if(tsm_invoke_id_free(temp_invoke_id))
						{
							//MessageBox(_T("Operation success!"),_T("Information"),MB_OK);
							//return;
							goto	program_part_success;
						}
					}
					b_program_status = false;
					MessageBox(_T("Write Program Code Timeout!"));
					return;

program_part_success:
					continue;
				}
			}
		}

		if(b_program_status)
		{
			CString temp_string;
			temp_string.Format(_T("Resource Compile succeeded.\r\nTotal size 2000 bytes.\r\nAlready used %d"),bac_program_size);
			MessageBox(temp_string);
		}

		m_information_window.ResetContent();
		m_information_window.InsertString(0,_T("Resource Compile succeeded."));
		m_Program_data.at(program_list_line).bytes = bac_program_size;
		Initial_static();
		CString temp1;
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->GetWindowTextW(temp1);
		m_edit_changed = false;
		program_string = temp1;
	}
	else
	{
		m_information_window.ResetContent();
		CString cstring_error;

		int len = strlen(mesbuf);
		int  unicodeLen = ::MultiByteToWideChar( CP_ACP,0, mesbuf,-1,NULL,0 ); 
		::MultiByteToWideChar( CP_ACP,  0,mesbuf,-1,cstring_error.GetBuffer(2000),unicodeLen );  
		cstring_error.ReleaseBuffer();


//		MessageBox(cstring_error);
		CStringArray  Error_info;  
		SplitCStringA(Error_info,cstring_error,_T("\r\n"));//Split the CString with "\r\n" and then add to the list.(Fance)
		Sleep(1);
		m_information_window.ResetContent();
		for (int i=0;i<(int)Error_info.GetSize();i++)
		{
			m_information_window.InsertString(i,Error_info.GetAt(i));
		}

		MessageBox(_T("Errors,program NOT Sent!"));
	}
	UpdateDataProgramText();
}





void CBacnetProgramEdit::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	UnregisterHotKey(GetSafeHwnd(),KEY_F2);//注销F2键
	UnregisterHotKey(GetSafeHwnd(),KEY_F3);
	UnregisterHotKey(GetSafeHwnd(),KEY_F7);
	UnregisterHotKey(GetSafeHwnd(),KEY_F6);
	UnregisterHotKey(GetSafeHwnd(),KEY_F8);

	g_hwnd_now = mParent_Hwnd;
	if(m_pragram_dlg_hwnd!=NULL)
		::PostMessage(m_pragram_dlg_hwnd,WM_REFRESH_BAC_PROGRAM_LIST,NULL,NULL);
	CDialogEx::OnClose();
}


void CBacnetProgramEdit::OnClear()
{
	// TODO: Add your command handler code here
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetWindowTextW(_T(""));
}


void CBacnetProgramEdit::OnLoadfile()
{
	// TODO: Add your command handler code here


	CString FilePath;
	CString ReadBuffer;

	CFileDialog dlg(true,_T("*.txt"),_T(" "),OFN_HIDEREADONLY ,_T("txt files (*.txt)|*.txt|All Files (*.*)|*.*||"),NULL,0);
	if(IDOK==dlg.DoModal())
	{
		FilePath=dlg.GetPathName();

		//Write_Position = pParent->myapp_path + cs_file_time;
		char *pBuf;
		DWORD dwFileLen;
		
		
		CFile file(FilePath,CFile::modeCreate |CFile::modeReadWrite |CFile::modeNoTruncate);

		dwFileLen=(DWORD)file.GetLength();
		pBuf= new char[dwFileLen+1];
		pBuf[dwFileLen]=0;

		file.SeekToBegin();
		file.Read(pBuf,dwFileLen);
		file.Close();

		CString ReadBuffer;

		int  len = 0;
		len = strlen(pBuf);
		int  unicodeLen = ::MultiByteToWideChar( CP_ACP,0, pBuf,-1,NULL,0 );  
		::MultiByteToWideChar( CP_ACP,  0,pBuf,-1,ReadBuffer.GetBuffer(unicodeLen),unicodeLen );  
		ReadBuffer.ReleaseBuffer();
		
		delete[] pBuf;
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetWindowTextW(ReadBuffer);
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(unicodeLen,unicodeLen);

	}
}


void CBacnetProgramEdit::OnSavefile()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(false,_T("*.txt"),_T(" "),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("txt files (*.txt)|*.txt|All Files (*.*)|*.*||"),NULL,0);
	if(IDOK==dlg.DoModal())
	{
		CString Write_Buffer;
		CString FilePath;
		FilePath=dlg.GetPathName();
		CFileFind temp_find;
		if(temp_find.FindFile(FilePath))
		{
			DeleteFile(FilePath);
		}



		GetDlgItemText(IDC_RICHEDIT2_PROGRAM,Write_Buffer);
		//char *readytowrite = 
		char*     readytowrite;
		int    iTextLen;
		iTextLen = WideCharToMultiByte( CP_ACP,0,Write_Buffer,-1,NULL,0,NULL,NULL );
		readytowrite = new char[iTextLen + 1];
		memset( ( void* )readytowrite, 0, sizeof( char ) * ( iTextLen + 1 ) );
		::WideCharToMultiByte( CP_ACP,0,Write_Buffer,-1,readytowrite,iTextLen,NULL,NULL );

		

		CFile file(FilePath,CFile::modeCreate |CFile::modeReadWrite |CFile::modeNoTruncate);
		file.SeekToEnd();
		int write_length = strlen(readytowrite);
		file.Write(readytowrite,write_length + 1);
		file.Flush();
		file.Close();
		delete[] readytowrite;
	}
	
}


void CBacnetProgramEdit::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	
	CString Edit_Buffer;
	GetDlgItemText(IDC_RICHEDIT2_PROGRAM,Edit_Buffer);
	if(program_string.CompareNoCase(Edit_Buffer) == 0)
	{
		((CBacnetProgram*)pDialog[WINDOW_PROGRAM])->Reg_Hotkey();
		CDialogEx::OnCancel();
	}
	else
	{
		if(MessageBox(_T("Do you want to exit the programming without saving?"),_T("Prompting"),MB_ICONINFORMATION | MB_YESNO) == IDYES)
		{
			((CBacnetProgram*)pDialog[WINDOW_PROGRAM])->Reg_Hotkey();
			CDialogEx::OnCancel();
		}
	}

}

//Fance 
//This part of code is use to make sure when the rich edit loss focus,and get focus later,it not select none of the character
//Move the mouse point to the end of the text.
void CBacnetProgramEdit::OnEnSetfocusRichedit2Program()
{
	// TODO: Add your control notification handler code here
	CString temp;
	GetDlgItemText(IDC_RICHEDIT2_PROGRAM,temp);
	int length = temp.GetLength();
	((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(length,length);
}


void CBacnetProgramEdit::OnRefresh()
{
	// TODO: Add your command handler code here

	memset(mycode,0,2000);



	for (int x=0;x<5;x++)
	{
		int send_status = true;
		int resend_count = 0;
		int temp_invoke_id = -1;
		do 
		{
			resend_count ++;
			if(resend_count>RESEND_COUNT)
				return;
			temp_invoke_id = GetProgramData(g_bac_instance,program_list_line,program_list_line,x);
			Sleep(SEND_COMMAND_DELAY_TIME);
		} while (temp_invoke_id<0);

		Sleep(SEND_COMMAND_DELAY_TIME);
		if(send_status)
		{
			for (int i=0;i<2000;i++)
			{
				Sleep(1);
				if(tsm_invoke_id_free(temp_invoke_id))
				{
					goto	dlg_part_success;
				}
			}
			return;

dlg_part_success:
			continue;
		}
	}
	Sleep(100);
	PostMessage(WM_REFRESH_BAC_PROGRAM_RICHEDIT,NULL,NULL);


}



BOOL CBacnetProgramEdit::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default
	if (g_protocol==PROTOCOL_BACNET_IP){
		HWND hWnd;

		if(pHelpInfo->dwContextId > 0) hWnd = ::HtmlHelp((HWND)pHelpInfo->hItemHandle,theApp.m_szHelpFile, HH_HELP_CONTEXT, pHelpInfo->dwContextId);
		else
			hWnd =  ::HtmlHelp((HWND)pHelpInfo->hItemHandle, theApp.m_szHelpFile,HH_HELP_CONTEXT, IDH_TOPIC_EXPORT_TO_BITMAPS);
		return (hWnd != NULL);
	}
	else{
		::HtmlHelp(NULL, theApp.m_szHelpFile, HH_HELP_CONTEXT, IDH_TOPIC_OVERVIEW);
	}
	return CDialogEx::OnHelpInfo(pHelpInfo);
}

void CBacnetProgramEdit::OnProgramIdeSettings()
{
	// TODO: Add your command handler code here
	CBacnetProgramSetting ProgramSettingdlg;
	ProgramSettingdlg.DoModal();
	if(prg_color_change)
	{
		AnalysisString.Empty();
		UpdateDataProgramText();
		prg_color_change = false;
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(0, 0); //操作完成后还原现场;
	}

}


void CBacnetProgramEdit::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	UpdateDataProgramText();
	CDialogEx::OnTimer(nIDEvent);
}

void CBacnetProgramEdit::UpdateDataProgramText()
{
	CString Edit_Buffer;
	GetDlgItemText(IDC_RICHEDIT2_PROGRAM,Edit_Buffer);
	if(AnalysisString.CompareNoCase(Edit_Buffer) != 0)
	{
		AnalysisString = Edit_Buffer;
		TRACE(_T("AnalysisString\r\n"));
		CString tempcs;
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->GetWindowTextW(tempcs);
		tempcs.MakeUpper();
		high_light_string = tempcs;
		check_high_light();


		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->HideSelection(TRUE,FALSE);
		SetRicheditFont(0,-1,prg_text_color);

		long temp_sel_str = 0;
		long temp_sel_end = 0;
		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->GetSel(temp_sel_str, temp_sel_end); //记住刷新前用户选择的部分;

		for (int i=0;i<m_prg_char_color.size();i++)
		{
			SetRicheditFont(m_prg_char_color.at(i).startpos,m_prg_char_color.at(i).endpos,m_prg_char_color.at(i).ncolor);
		}
		//if(temp_sel_str!= temp_sel_end)
			((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSel(temp_sel_end, temp_sel_end); //操作完成后还原现场;

		((CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->HideSelection(FALSE,FALSE);
	}
	else
	{
		TRACE(_T("The same\r\n"));
	}


	SetBackFont();
	return;
}

void CBacnetProgramEdit::SetBackFont()
{
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask|=CFM_BOLD;

	cf.dwEffects&=~CFE_BOLD;
	//cf.dwEffects|=~CFE_BOLD; //粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwMask|=CFM_ITALIC;
	cf.dwEffects&=~CFE_ITALIC;
	//cf.dwEffects|=~CFE_ITALIC; //斜体，取消用cf.dwEffects&=~CFE_ITALIC;
	cf.dwMask|=CFM_UNDERLINE;
	cf.dwEffects&=~CFE_UNDERLINE;
	//cf.dwEffects|=~CFE_UNDERLINE; //斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
	cf.dwMask|=CFM_COLOR;
	cf.crTextColor = prg_text_color;//RGB(0,0,255); //设置颜色
	cf.dwMask|=CFM_SIZE;
	cf.yHeight =250; //设置高度
	cf.dwMask|=CFM_FACE;
	//_tcscpy(cf.szFaceName ,_T("SimSun-ExtB"));
	//_tcscpy(cf.szFaceName ,_T("Times New Roman"));
	//	strcpy(cf.szFaceName ,_T("隶书")); //设置字体
	_tcscpy(cf.szFaceName , prg_character_font);
	//_tcscpy(cf.szFaceName ,_T("NSimSun"));
	//((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetSelectionCharFormat(cf);
	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT2_PROGRAM))->SetDefaultCharFormat(cf); 

};
