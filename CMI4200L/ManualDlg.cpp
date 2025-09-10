// ManualDlg.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CMI4200L.h"
#include "ManualDlg.h"
#include "afxdialogex.h"

#include "CMI4200LDlg.h"
#include "LogFile.h"

// CManualDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CManualDlg, CDialogEx)

CManualDlg *CManualDlg::m_pInstance = NULL;

CManualDlg *CManualDlg::Get_Instance(CWnd *pParent)
{
	if (!m_pInstance) {
		m_pInstance = new CManualDlg(pParent);
		if (!m_pInstance->m_hWnd) {
			m_pInstance->Create(IDD_MANUAL_DLG, pParent);
		}
	}
	return m_pInstance;
}

void CManualDlg::Delete_Instance()
{
	if (m_pInstance->m_hWnd) m_pInstance->DestroyWindow();
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CManualDlg::CManualDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManualDlg::IDD, pParent)
{
}

CManualDlg::~CManualDlg()
{
}

void CManualDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_MANUAL_BACK, m_picManualBack);
	DDX_Control(pDX, IDC_RDO_MANUAL_LOADING, m_rdoManualLoading);
	DDX_Control(pDX, IDC_RDO_MANUAL_LOADPICKER, m_rdoManualLoadPicker);
	DDX_Control(pDX, IDC_RDO_MANUAL_GRIPPER, m_rdoManual_Inspector);
	DDX_Control(pDX, IDC_RDO_MANUAL_PICKER, m_rdoManual_NgGoodPicker);
	DDX_Control(pDX, IDC_RDO_MANUAL_UNLOADING, m_rdoManual_Unloading);
	DDX_Control(pDX, IDC_RDO_MANUAL_UNLOADPICKER, m_rdoManual_UnloadPicker);
	DDX_Control(pDX, IDC_RDO_MANUAL_DOOR_LOCK, m_rdoManualDoorLock);
	DDX_Control(pDX, IDC_RDO_MANUAL_DOOR_UNLOCK, m_rdoManualDoorUnlock);

}

BEGIN_MESSAGE_MAP(CManualDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RDO_MANUAL_LOADING, &CManualDlg::OnBnClickedRdoManualLoading)
	ON_BN_CLICKED(IDC_RDO_MANUAL_LOADPICKER, &CManualDlg::OnBnClickedRdoManualLoadPicker)
	ON_BN_CLICKED(IDC_RDO_MANUAL_GRIPPER, &CManualDlg::OnBnClickedRdoManualInspector)
	ON_BN_CLICKED(IDC_RDO_MANUAL_PICKER, &CManualDlg::OnBnClickedRdoManualNgGoodPicker)
	ON_BN_CLICKED(IDC_RDO_MANUAL_UNLOADING, &CManualDlg::OnBnClickedRdoManualUnloading)
	ON_BN_CLICKED(IDC_RDO_MANUAL_DOOR_LOCK, &CManualDlg::OnBnClickedRdoManualDoorLock)
	ON_BN_CLICKED(IDC_RDO_MANUAL_DOOR_UNLOCK, &CManualDlg::OnBnClickedRdoManualDoorUnlock)

	ON_BN_CLICKED(IDC_RDO_MANUAL_UNLOADPICKER, &CManualDlg::OnBnClickedRdoManualUnloadPicker)
END_MESSAGE_MAP()

// CManualDlg 메시지 처리기입니다.

BOOL CManualDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(this, 0, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	Initial_Controls();	

	m_pManual_InspectorDlg = new CManual_InspectorDlg(this);
	m_pManual_InspectorDlg->Create(IDD_MANUAL_INSPECTOR_DLG, this);

	m_pManual_NGGoodPickerDlg = new CManual_NgGoodPickerDlg(this);
	m_pManual_NGGoodPickerDlg->Create(IDD_MANUAL_NG_GOOD_PICKER_DLG, this);

	m_pManual_UnloadingDlg = new CManual_UnloadingDlg(this);
	m_pManual_UnloadingDlg->Create(IDD_MANUAL_UNLOADING_DLG, this);

	m_pManual_LoadPickerDlg = new CManual_LoadPickerDlg(this);
	m_pManual_LoadPickerDlg->Create(IDD_MANUAL_LOADPICKER_DLG, this);

	m_pManual_UnloadPickerDlg = new CManual_UnloadPickerDlg(this);
	m_pManual_UnloadPickerDlg->Create(IDD_MANUAL_UNLOADPICKER_DLG, this);
	
	m_pManual_LoadingDlg = new CManual_LoadingDlg(this);
	m_pManual_LoadingDlg->Create(IDD_MANUAL_LOADING_DLG, this);

	// Inspector Dlg Visible
	m_rdoManual_Unloading.SetCheck(TRUE);
	m_rdoManual_Unloading.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CManualDlg::OnDestroy() 
{
	CDialogEx::OnDestroy();

	m_pManual_UnloadingDlg->DestroyWindow();
	m_pManual_NGGoodPickerDlg->DestroyWindow();
	m_pManual_InspectorDlg->DestroyWindow();	
	m_pManual_LoadingDlg->DestroyWindow();
	m_pManual_LoadPickerDlg->DestroyWindow();
	m_pManual_UnloadPickerDlg->DestroyWindow();

	if (m_pManual_UnloadingDlg) delete m_pManual_UnloadingDlg;
	if (m_pManual_NGGoodPickerDlg) delete m_pManual_NGGoodPickerDlg;
	if (m_pManual_InspectorDlg) delete m_pManual_InspectorDlg;
	if (m_pManual_LoadPickerDlg) delete m_pManual_LoadPickerDlg;
	if (m_pManual_UnloadPickerDlg) delete m_pManual_UnloadPickerDlg;
	
	m_pManual_UnloadingDlg = NULL;
	m_pManual_NGGoodPickerDlg = NULL;
	m_pManual_InspectorDlg = NULL;
	m_pManual_LoadPickerDlg = NULL;
	m_pManual_UnloadPickerDlg = NULL;	
	m_pManual_LoadingDlg = NULL;
}

BOOL CManualDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CManualDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		if (m_rdoManualLoading.GetCheck()) m_pManual_LoadingDlg->ShowWindow(SW_SHOW);
		if (m_rdoManualLoadPicker.GetCheck()) m_pManual_LoadPickerDlg->ShowWindow(SW_SHOW);
		if (m_rdoManual_UnloadPicker.GetCheck()) m_pManual_UnloadPickerDlg->ShowWindow(SW_SHOW);
		if (m_rdoManual_Inspector.GetCheck()) m_pManual_InspectorDlg->ShowWindow(SW_SHOW);
		if (m_rdoManual_NgGoodPicker.GetCheck()) m_pManual_NGGoodPickerDlg->ShowWindow(SW_SHOW);
		if (m_rdoManual_Unloading.GetCheck()) m_pManual_UnloadingDlg->ShowWindow(SW_SHOW);
	
//		m_rdoManualDoorLock.SetCheck(TRUE);
//		m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
//		m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);

		SetTimer(0, 100, NULL);

	} else {
		KillTimer(0);

//		m_rdoManualDoorUnlock.SetCheck(TRUE);
//		m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
//		m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);

		if (m_rdoManualLoading.GetCheck()) m_pManual_LoadingDlg->ShowWindow(SW_HIDE);		
		if (m_rdoManualLoadPicker.GetCheck()) m_pManual_LoadPickerDlg->ShowWindow(SW_HIDE);
		if (m_rdoManual_UnloadPicker.GetCheck()) m_pManual_UnloadPickerDlg->ShowWindow(SW_HIDE);
		if (m_rdoManual_Inspector.GetCheck()) m_pManual_InspectorDlg->ShowWindow(SW_HIDE);
		if (m_rdoManual_NgGoodPicker.GetCheck()) m_pManual_NGGoodPickerDlg->ShowWindow(SW_HIDE);
		if (m_rdoManual_Unloading.GetCheck()) m_pManual_UnloadingDlg->ShowWindow(SW_HIDE);
		
	}
}

void CManualDlg::OnTimer(UINT nIDEvent) 
{
	KillTimer(0);

	CCommon *pCommon = CCommon::Get_Instance();
	if (!gData.m_bErrorShow) {
		pCommon->Check_MainEmgAir();	// Emg & Main Air
	}

	if (m_pManual_InspectorDlg->IsWindowVisible()) {
		m_pManual_InspectorDlg->Display_Status();
	} else if (m_pManual_NGGoodPickerDlg->IsWindowVisible()) {
		m_pManual_NGGoodPickerDlg->Display_Status();
	} else if (m_pManual_LoadPickerDlg->IsWindowVisible()) {
		m_pManual_LoadPickerDlg->Display_Status();
	} else if (m_pManual_UnloadPickerDlg->IsWindowVisible()) {
		m_pManual_UnloadPickerDlg->Display_Status();
	} else if (m_pManual_UnloadingDlg->IsWindowVisible()) {
		m_pManual_UnloadingDlg->Display_Status();
	} 
	else if (m_pManual_LoadingDlg->IsWindowVisible()) {
		m_pManual_LoadingDlg->Display_Status();
	}

	SetTimer(0, 100, NULL);
	CDialogEx::OnTimer(nIDEvent);
}

void CManualDlg::OnBnClickedRdoManualLoading()
{
	if (m_pManual_LoadingDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Loading] Start");
	m_rdoManualLoading.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_LoadingDlg->ShowWindow(SW_SHOW);
}

void CManualDlg::OnBnClickedRdoManualLoadPicker()
{
	if (m_pManual_LoadPickerDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Load Picker] Start");
	m_rdoManualLoadPicker.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_LoadPickerDlg->ShowWindow(SW_SHOW);

}

void CManualDlg::OnBnClickedRdoManualInspector()
{
	if (m_pManual_InspectorDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Inspector] Start");
	m_rdoManual_Inspector.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_InspectorDlg->ShowWindow(SW_SHOW);
}

void CManualDlg::OnBnClickedRdoManualNgGoodPicker()
{
	if (m_pManual_NGGoodPickerDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Ng Good Picker] Start");
	m_rdoManual_NgGoodPicker.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_NGGoodPickerDlg->ShowWindow(SW_SHOW);
}

void CManualDlg::OnBnClickedRdoManualUnloading()
{
	if (m_pManual_UnloadingDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Unloading] Start");
	m_rdoManual_Unloading.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_UnloadingDlg->ShowWindow(SW_SHOW);
}


void CManualDlg::OnBnClickedRdoManualUnloadPicker()
{
	if (m_pManual_UnloadPickerDlg->IsWindowVisible()) return;
	Hide_Windows();
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_HandlerLog("[Manual - Unload Picker] Start");
	m_rdoManual_UnloadPicker.Set_Color(RGB(0xFF, 0x00, 0x00), COLOR_DEFAULT);
	m_pManual_UnloadPickerDlg->ShowWindow(SW_SHOW);

}

void CManualDlg::OnBnClickedRdoManualDoorLock()
{
/*
	if (m_rdoManualDoorLock.GetCheck()) {
		CLogFile *pLogFile = CLogFile::Get_Instance();
		pLogFile->Save_HandlerLog("[Manual] Door Lock button push");
//		CCommon *pCommon = CCommon::Get_Instance();
//		pCommon->Locking_MainDoor(TRUE);
		m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);
	}
*/
}

void CManualDlg::OnBnClickedRdoManualDoorUnlock()
{
/*
	if (m_rdoManualDoorUnlock.GetCheck()) {
		CLogFile *pLogFile = CLogFile::Get_Instance();
		pLogFile->Save_HandlerLog("[Manual] Door Unlock button push");
		m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
		m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);
//		CCommon *pCommon = CCommon::Get_Instance();
//		pCommon->Locking_MainDoor(FALSE);
	}
*/
}

///////////////////////////////////////////////////////////////////////////////
// User Functions

void CManualDlg::Initial_Controls() 
{
	m_picManualBack.Init_Ctrl(RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xFF, 0xE0));
	m_rdoManualLoading.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
	m_rdoManualLoadPicker.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
	m_rdoManual_Inspector.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
	m_rdoManual_NgGoodPicker.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
	m_rdoManual_Unloading.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
	m_rdoManual_UnloadPicker.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);	
//	m_rdoManualDoorLock.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);
//	m_rdoManualDoorUnlock.Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), COLOR_DEFAULT, 0, 0);

}

void CManualDlg::Hide_Windows()
{
	m_pManual_InspectorDlg->ShowWindow(SW_HIDE);
	m_pManual_NGGoodPickerDlg->ShowWindow(SW_HIDE);
	m_pManual_LoadPickerDlg->ShowWindow(SW_HIDE);
	m_pManual_UnloadPickerDlg->ShowWindow(SW_HIDE);
	m_pManual_UnloadingDlg->ShowWindow(SW_HIDE);	
	m_pManual_LoadingDlg->ShowWindow(SW_HIDE);

	m_rdoManualLoading.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_rdoManualLoadPicker.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_rdoManual_Inspector.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_rdoManual_NgGoodPicker.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
	m_rdoManual_UnloadPicker.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);	
	m_rdoManual_Unloading.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);

}

void CManualDlg::MainDoor_Lock()
{
//	CLogFile *pLogFile = CLogFile::Get_Instance();
//	pLogFile->Save_HandlerLog("[Manual] Door Lock cmd exec");
//	m_rdoManualDoorLock.SetCheck(TRUE);
//	m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
//	m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);
}

void CManualDlg::MainDoor_Unlock()
{
//	CLogFile *pLogFile = CLogFile::Get_Instance();
//	pLogFile->Save_HandlerLog("[Manual] Door Unlock cmd exec");
//	m_rdoManualDoorUnlock.SetCheck(TRUE);
//	m_rdoManualDoorLock.Set_Color(RGB(0x00, 0x00, 0x00), COLOR_DEFAULT);
//	m_rdoManualDoorUnlock.Set_Color(RGB(0x00, 0x00, 0xFF), COLOR_DEFAULT);
}

///////////////////////////////////////////////////////////////////////////////
