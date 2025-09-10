// ManualDlg.h : 헤더 파일
//
#pragma once


#include "Manual_InspectorDlg.h"
#include "Manual_NgGoodPickerDlg.h"
#include "Manual_UnloadingDlg.h"
#include "Manual_LoadPickerDlg.h"
#include "Manual_UnloadPickerDlg.h"
#include "Manual_LoadingDlg.h"

// CManualDlg 대화 상자입니다.

class CManualDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CManualDlg)

private:
	static CManualDlg *m_pInstance;

public:
	static CManualDlg *Get_Instance(CWnd *pParent = NULL);
	void Delete_Instance();

public:
	CManualDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CManualDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_MANUAL_DLG };
	CPictureCS	m_picManualBack;
	CRadioCS	m_rdoManualLoading;
	CRadioCS	m_rdoManualLoadPicker;
	CRadioCS	m_rdoManual_Inspector;
	CRadioCS	m_rdoManual_NgGoodPicker;
	CRadioCS	m_rdoManual_Unloading;
	CRadioCS	m_rdoManual_UnloadPicker;
	CRadioCS	m_rdoManualDoorLock;
	CRadioCS	m_rdoManualDoorUnlock;

	
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRdoManualLoading();
	afx_msg void OnBnClickedRdoManualLoadPicker();
	afx_msg void OnBnClickedRdoManualInspector();
	afx_msg void OnBnClickedRdoManualNgGoodPicker();
	afx_msg void OnBnClickedRdoManualUnloading();
	afx_msg void OnBnClickedRdoManualUnloadPicker();
	afx_msg void OnBnClickedRdoManualDoorLock();
	afx_msg void OnBnClickedRdoManualDoorUnlock();



public:	
	CManual_InspectorDlg	*m_pManual_InspectorDlg;
	CManual_NgGoodPickerDlg	*m_pManual_NGGoodPickerDlg;
	CManual_UnloadingDlg	*m_pManual_UnloadingDlg;
	CManual_LoadPickerDlg	*m_pManual_LoadPickerDlg;
	CManual_UnloadPickerDlg	*m_pManual_UnloadPickerDlg;
	CManual_LoadingDlg		*m_pManual_LoadingDlg;

private:
	void Initial_Controls();
	void Hide_Windows();

public:
	void MainDoor_Lock();
	void MainDoor_Unlock();
};

///////////////////////////////////////////////////////////////////////////////
