// SequenceMain.cpp : 구현 파일
//
#include "stdafx.h"
#include "CMI4200L.h"
#include "CMI4200LDlg.h"
#include "SequenceMain.h"
#include <math.h>

#include "CapAttachUDP.h"
#include "Inspector.h"
#include "WorkDlg.h"
#include "math.h"
#include "SequenceInit.h"
#include "MESInterface.h"

CSequenceMain *CSequenceMain::m_pInstance = NULL;

CSequenceMain* CSequenceMain::Get_Instance()
{
	if (!m_pInstance)
		m_pInstance = new CSequenceMain();
	return m_pInstance;
}

void CSequenceMain::Delete_Instance()
{
	if (m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////

CSequenceMain::CSequenceMain()
{
	pLogFile = CLogFile::Get_Instance();
	m_pAJinAXL = CAJinAXL::Get_Instance();

	for (int i = 0; i < AXIS_COUNT; i++) {
		m_pStatus[i] = m_pAJinAXL->Get_pStatus(i);
		m_pParam[i] = m_pAJinAXL->Get_pParam(i);
	}
	
	m_pDX0 = m_pAJinAXL->Get_pDX0();
	m_pDX1 = m_pAJinAXL->Get_pDX1();
	m_pDX2 = m_pAJinAXL->Get_pDX2();
	m_pDX3 = m_pAJinAXL->Get_pDX3();
	m_pDX4 = m_pAJinAXL->Get_pDX4();
	m_pDX5 = m_pAJinAXL->Get_pDX5();
	m_pDX6 = m_pAJinAXL->Get_pDX6();
	
	m_pDY0 = m_pAJinAXL->Get_pDY0();
	m_pDY1 = m_pAJinAXL->Get_pDY1();
	m_pDY2 = m_pAJinAXL->Get_pDY2();
	m_pDY3 = m_pAJinAXL->Get_pDY3();
	m_pDY4 = m_pAJinAXL->Get_pDY4();
	m_pDY5 = m_pAJinAXL->Get_pDY5();
	m_pDY6 = m_pAJinAXL->Get_pDY6();
	
	m_pDataManager = CDataManager::Get_Instance();
	m_pEquipData = m_pDataManager->Get_pEquipData();
	m_pModelData = m_pDataManager->Get_pModelData();
	m_pMoveData = m_pDataManager->Get_pMoveData();
	
	m_pCommon = CCommon::Get_Instance();

	m_bThreadMainRun = FALSE;
	m_pThreadMainRun = NULL;
/*	m_bThreadTilt1Run = FALSE;
	m_pThreadTilt1Run = NULL;
	m_bThreadTilt2Run = FALSE;
	m_pThreadTilt2Run = NULL;
*/
	gData.nLoadTrayCount = 0;
	gData.nEmptyTrayCount = 0;
	gData.nGoodTrayCount = 0;

	m_nLoad1Case = 100;
	m_nLoad2Case = 440;
	m_nUnload1Case = 100;
	m_nUnload2Case = 440;
	m_nInspectCase = 100;
	m_nBarcodeCase = 100;
	m_nNGPickerCase = 100;
	m_nGDPickerCase = 100;
	m_nLDPickerCase = 100;
	m_nULPickerCase = 100;
	m_nIndexTCase = 100;
	m_nNGTrayCase = 100;

	gData.bTrayChangeClicked = FALSE;
}

CSequenceMain::~CSequenceMain()
{
	if (m_pThreadMainRun) {
		m_bThreadMainRun = FALSE;
		WaitForSingleObject(m_pThreadMainRun->m_hThread, INFINITE);
	}
}

int CSequenceMain::Get_RunCase(int nRun)
{
	if (nRun == AUTO_LOAD1)			return m_nLoad1Case;
	if (nRun == AUTO_LOAD2)			return m_nLoad2Case;
	if (nRun == AUTO_UNLOAD1)		return m_nUnload1Case;
	if (nRun == AUTO_UNLOAD2)		return m_nUnload2Case;
	if (nRun == AUTO_INSPECT)		return m_nInspectCase;
	if (nRun == AUTO_NGPICKER)		return m_nNGPickerCase;
	if (nRun == AUTO_GDPICKER)		return m_nGDPickerCase;
	if (nRun == AUTO_LDPICKER)		return m_nLDPickerCase;
	if (nRun == AUTO_ULPICKER)		return m_nULPickerCase;
	if (nRun == AUTO_INDEXT)		return m_nIndexTCase;
	if (nRun == AUTO_NGTRAY)		return m_nNGTrayCase;
	if (nRun == AUTO_BARCODE)		return m_nBarcodeCase;

	return 0;
}

void CSequenceMain::Set_RunCase(int nRun, int nCase)
{
	if (nRun == AUTO_LOAD1)			m_nLoad1Case = nCase;
	if (nRun == AUTO_LOAD2)			m_nLoad2Case = nCase;
	if (nRun == AUTO_UNLOAD1)		m_nUnload1Case = nCase;
	if (nRun == AUTO_UNLOAD2)		m_nUnload2Case = nCase;
	if (nRun == AUTO_INSPECT)		m_nInspectCase = nCase;
	if (nRun == AUTO_NGPICKER)		m_nNGPickerCase = nCase;
	if (nRun == AUTO_GDPICKER)		m_nGDPickerCase = nCase;
	if (nRun == AUTO_LDPICKER)		m_nLDPickerCase = nCase;
	if (nRun == AUTO_ULPICKER)		m_nULPickerCase = nCase;
	if (nRun == AUTO_INDEXT)		m_nIndexTCase = nCase;
	if (nRun == AUTO_NGTRAY)		m_nNGTrayCase = nCase;
	if (nRun == AUTO_BARCODE)		m_nBarcodeCase = nCase;
}

void CSequenceMain::Get_RunCase(int &nCase1, int &nCase2, int &nCase3, int &nCase4, int &nCase5, int &nCase6, int &nCase7, int &nCase8, int &nCase9, int &nCase10, int &nCase11, int &nCase12)
{
	nCase1 = m_nLoad1Case;
	nCase2 = m_nLoad2Case;
	nCase3 = m_nUnload1Case;
	nCase4 = m_nUnload2Case;
	nCase5 = m_nInspectCase;
	nCase6 = m_nNGPickerCase;
	nCase7 = m_nGDPickerCase;
	nCase8 = m_nLDPickerCase;
	nCase9 = m_nULPickerCase;
	nCase10 =m_nIndexTCase;
	nCase11 =m_nNGTrayCase;
	nCase12 =m_nBarcodeCase;
}

void CSequenceMain::Set_RunCase(int nCase1, int nCase2, int nCase3, int nCase4, int nCase5, int nCase6, int nCase7, int nCase8, int nCase9, int nCase10, int nCase11, int nCase12)
{
	m_nLoad1Case = nCase1;
	m_nLoad2Case = nCase2;
	m_nUnload1Case = nCase3;
	m_nUnload2Case = nCase4;
	m_nInspectCase = nCase5;
	m_nNGPickerCase = nCase6;
	m_nGDPickerCase = nCase7;
	m_nLDPickerCase = nCase8;
	m_nULPickerCase = nCase9;
	m_nIndexTCase = nCase10;
	m_nNGTrayCase = nCase11;
	m_nBarcodeCase = nCase12;
}

int CSequenceMain::Get_IsAutoRun(int nType)
{
	int nCase[12];

	Get_RunCase(nCase[0], nCase[1], nCase[2], nCase[3], nCase[4], nCase[5], nCase[6], nCase[7], nCase[8], nCase[9], nCase[10], nCase[11]);
	for (int i = 0; i < 12; i++) {
		if (i==10) continue;	//NG Tray Skip

		if (i>=0 && i<=3) {		//Load, Unload Tray
			if (nCase[i] > 100 && nCase[i] != 250  && nCase[i] != 300 && nCase[i] != 440 && nCase[i] != 450 && nCase[i] != 520 ) return 1;
		} else {
			if (nCase[i] > 100) return 1;
		}
	}
	if (Check_PickerEmpty()==FALSE) return 1;
	if (Check_IndexEmpty(9)==FALSE) return 1;


	if (nType == 1) {
		if (!m_pEquipData->bUseMesApd) { gData.bAPDResultErr = FALSE; }

		// APD 요청
		if (m_pEquipData->bUseMesApd && !gData.bAviApdRequest) {
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_APDRequest(INSPECTOR_VISION);
			g_objCapAttachUDP.Set_APDRequest();
		}

		// Vision, Cap Attach Data 받았는지 확인.
		if (m_pEquipData->bUseMesApd && !gData.bAviApdReceive && (gData.dAssyLoadCellAvg + 0.1 < 0)) {
			if (gData.bAviApdRequest == TRUE) {
				if (gData.nVisionTimeOut > 500) {
					gData.nVisionTimeOut = 0; m_pCommon->Show_Error(996); 	return 2;
				} else {
					gData.nVisionTimeOut++;
				}
			}
			return 1;	// APD Data Receive 확인
		}
		gData.nVisionTimeOut = 0;

		// 데이터를 모두 받았다면 APD 파일 저장.
		if (m_pEquipData->bUseMesApd && !gData.bAviApdSave) {
			g_objMES.Clear_APDResult();	// APD 결과 폴더에 파일이 있다면 삭제.
			g_objMES.Save_AviApdData(gData.sLotID, 0, gData.sOperID);	// APD 파일 생성 및 MES 보고

		}		
		// APD 결과 파일 확인.
		if (m_pEquipData->bUseMesApd && !gData.bAviApdFileChk) {
			if (!g_objMES.Read_APDResult() && gData.bAviApdSave == TRUE) {
				if (gData.nMesTimeOut > 500) {
					gData.nMesTimeOut = 0; m_pCommon->Show_Error(997); 	return 2;
				} else {
					gData.nMesTimeOut++;
				}
			}
			return 1;
		}
		gData.nMesTimeOut = 0;
	}

	return 0;
}
void CSequenceMain::Begin_MainRunThread()
{
	Run_Replay();

	g_objCapAttachUDP.Set_StatusUpdate(1);

	if (m_pThreadMainRun) End_MainRunThread();
	m_bThreadMainRun = TRUE;
	m_pThreadMainRun = AfxBeginThread(Thread_MainRun, this);
}

void CSequenceMain::End_MainRunThread()
{
	if (m_pThreadMainRun) {
		m_bThreadMainRun = FALSE;
		WaitForSingleObject(m_pThreadMainRun->m_hThread, INFINITE);
	}
}
/*
void CSequenceMain::Begin_Tilt1RunThread()
{
	if (m_pThreadTilt1Run) End_Tilt1RunThread();
	m_bThreadTilt1Run = TRUE;
	m_pThreadTilt1Run = AfxBeginThread(Thread_Tilt1Run, this);
}

void CSequenceMain::End_Tilt1RunThread()
{
	if (m_pThreadTilt1Run) {
		m_bThreadTilt1Run = FALSE;
		WaitForSingleObject(m_pThreadTilt1Run->m_hThread, INFINITE);
	}
}

void CSequenceMain::Begin_Tilt2RunThread()
{
	if (m_pThreadTilt2Run) End_Tilt2RunThread();
	m_bThreadTilt2Run = TRUE;
	m_pThreadTilt2Run = AfxBeginThread(Thread_Tilt2Run, this);
}

void CSequenceMain::End_Tilt2RunThread()
{
	if (m_pThreadTilt2Run) {
		m_bThreadTilt2Run = FALSE;
		WaitForSingleObject(m_pThreadTilt2Run->m_hThread, INFINITE);
	}
}
*/
void CSequenceMain::Run_Replay()
{
	m_pCommon->Set_LoopTime(AUTO_LOAD1, 10000);
	m_pCommon->Set_LoopTime(AUTO_LOAD2, 10000);
	m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 10000);
	m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 10000);
	m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
	m_pCommon->Set_LoopTime(AUTO_NGPICKER, 10000);
	m_pCommon->Set_LoopTime(AUTO_GDPICKER, 10000);
	m_pCommon->Set_LoopTime(AUTO_LDPICKER, 10000);
	m_pCommon->Set_LoopTime(AUTO_ULPICKER, 10000);
	m_pCommon->Set_LoopTime(AUTO_INDEXT, 60000);
	m_pCommon->Set_LoopTime(AUTO_NGTRAY, 10000);
	m_pCommon->Set_LoopTime(AUTO_BARCODE, 10000);
}

/////////////////////////////////////////////////////////////////////////////
// Main Thread Function 
UINT CSequenceMain::Thread_MainRun(LPVOID lpVoid)
{
	CSequenceMain *pSeqMain = (CSequenceMain*)lpVoid;
	CCommon *pCommon = CCommon::Get_Instance();

	while (pSeqMain->m_bThreadMainRun) {

		if (!pCommon->Check_MainEmgAir()) break;
		if (!pCommon->Check_MainDoor()) break;
		
		if (!pCommon->Check_ServoOn()) break;
		if (!pCommon->Check_DirveAlarm()) break;
		if (!pCommon->Check_EndLimit()) break;
		if (!pCommon->Check_HomeDone()) break;

		if (!pSeqMain->Load1_Run()) break;		// 1.  (Error : 2100)
		if (!pSeqMain->Load2_Run()) break;		// 2.  (Error : 2600)
		if (!pSeqMain->Unload1_Run()) break;	// 3.  (Error : 3100)
		if (!pSeqMain->Unload2_Run()) break;	// 4.  (Error : 3600)
		if (!pSeqMain->Inspect_Run()) break;	// 5.  (Error : 4100)
		if (!pSeqMain->Barcode_Run()) break;	// 5.  (Error : 4300)
		if (!pSeqMain->NGPicker_Run()) break;	// 6.  (Error : 5100)
		if (!pSeqMain->GDPicker_Run()) break;	// 7.  (Error : 6100)
		if (!pSeqMain->LDPicker_Run()) break;	// 8.  (Error : 7100)
		if (!pSeqMain->ULPicker_Run()) break;	// 9.  (Error : 8100)
		if (!pSeqMain->IndexT_Run()) break;		// 10. (Error : 9100)
		if (!pSeqMain->NGTray_Run()) break;		// 11. (Error : 9600)
		if (!pSeqMain->Run_Simulation()) break;		//  99.

		if (gData.bCleanOutMode == TRUE) {
			if (pSeqMain->Check_LotEnd()==TRUE && pSeqMain->m_nULPickerCase == 100) {
				CWorkDlg *pWorkDlg = CWorkDlg::Get_Instance();
				pWorkDlg->Set_CleanOutMode();

				CSequenceInit *pSequenceInit = CSequenceInit::Get_Instance();
				pSequenceInit->Set_InitComplete(FALSE);
				pSeqMain->Beep_Post(1000);

				CCMI4200LDlg *pMainDlg = (CCMI4200LDlg*)AfxGetApp()->m_pMainWnd;
				pMainDlg->Set_MainState(STATE_NONE);
				break;
			}
		}

		// 후처리 Sequence
		if (gData.nStatus == 3) {
			pSeqMain->Set_RunCase(AUTO_LOAD1, 480);
			pCommon->Set_LoopTime(AUTO_LOAD1, 10000);
			gData.nStatus = 4;	// Load Tray Sort
		}
		int nRunSts = pSeqMain->Get_IsAutoRun();
		if (nRunSts == 0 && (gData.nStatus >= 3 && gData.nStatus < 5)) {
			pSeqMain->Set_RunCase(AUTO_INDEXT, 300);
			gData.nStatus = 5;	//최종 후처리
		}
		nRunSts = pSeqMain->Get_IsAutoRun(1);
		if (nRunSts == 2) break;	// Alarm
		if (nRunSts == 0 && gData.nStatus==5 && gData.nNGTrayPos==0) {
			pSeqMain->Job_LotEnd();
			pSeqMain->Beep_Post(1000);
			pSeqMain->Set_ClearRunData(1);

			CCMI4200LDlg *pMainDlg = (CCMI4200LDlg*)AfxGetApp()->m_pMainWnd;
			pMainDlg->Set_MainState(STATE_LOTEND);

			CWorkDlg *pWorkDlg = CWorkDlg::Get_Instance();
			pWorkDlg->AutoStop();

			if (gData.bAPDResultErr) { pCommon->Show_Error(998); }
			break;
		}

		Sleep(10);
	}
	g_objCapAttachUDP.Set_StatusUpdate(0);	
	//sch
	pSeqMain->m_bThreadMainRun = FALSE;
	pSeqMain->m_pThreadMainRun = NULL;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//  Thread Function 
/*
UINT CSequenceMain::Thread_Tilt1Run(LPVOID lpVoid)
{
	CSequenceMain *pSeqMain = (CSequenceMain*)lpVoid;
	CCommon *pCommon = CCommon::Get_Instance();

	pSeqMain->m_bTilted = FALSE;
	pSeqMain->m_nTiltSense = 0;
	pSeqMain->m_dTiltStartPos = 0;
	pSeqMain->m_dTiltEndPos = 0;

	while (pSeqMain->m_bThreadTilt1Run) {
		if (pSeqMain->m_bThreadMainRun == FALSE) break;

		if (!pSeqMain->Tilt_Check()) break;
		
		Sleep(1);
	}

	pSeqMain->m_bThreadTilt1Run = FALSE;
	pSeqMain->m_pThreadTilt1Run = NULL;

	return 0;
}

UINT CSequenceMain::Thread_Tilt2Run(LPVOID lpVoid)
{
	CSequenceMain *pSeqMain = (CSequenceMain*)lpVoid;
	CCommon *pCommon = CCommon::Get_Instance();

	pSeqMain->m_bTilted = FALSE;
	pSeqMain->m_nTiltSense = 0;
	pSeqMain->m_dTiltStartPos = 0;
	pSeqMain->m_dTiltEndPos = 0;

	while (pSeqMain->m_bThreadTilt2Run) {
		if (pSeqMain->m_bThreadMainRun == FALSE) break;

		if (!pSeqMain->Tilt_Check()) break;
		
		Sleep(1);
	}

	pSeqMain->m_bThreadTilt2Run = FALSE;
	pSeqMain->m_pThreadTilt2Run = NULL;

	return 0;
}

// Signal Tilt 검사
BOOL CSequenceMain::Tilt_Check()
{
	CString		sLog;
	double		dTiltStart, dTiltEnd;
	AXIS_STATUS *pStatus;

	dTiltStart = 10.0;
	dTiltEnd   = 175.0;

	if ((m_nThreadSource == 1 && !m_pDX0->iLoadTR1CMTilted) || (m_nThreadSource == 2 && !m_pDX0->iLoadTR2CMTilted)) return TRUE;

	CDataManager *pDataManager = CDataManager::Get_Instance();
	MOVE_DATA *pMoveData = pDataManager->Get_pMoveData();

	if (m_nThreadSource == 1) {
		pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_1_Y);
		m_dTiltStartPos = pMoveData->dLoadTrayTR1Y[4] - pStatus->dPos;
	} else {
		pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_2_Y);
		m_dTiltStartPos = pMoveData->dLoadTrayTR2Y[4] - pStatus->dPos;
	}

	if (m_dTiltStartPos < dTiltStart || m_dTiltStartPos > dTiltEnd) return TRUE;

	pLogFile = CLogFile::Get_Instance();
	sLog.Format("TR%d Tilted=True :: TStart[%0.1lf] - TEnd[%0.1lf] = Detect[%0.1lf]", m_nThreadSource, dTiltStart, dTiltEnd, m_dTiltStartPos);
	pLogFile->Save_HandlerLog(sLog);
	m_bTilted = TRUE;
	return FALSE;
}
*/
/*
// 구간 Tilt 검사
BOOL CSequenceMain::Tilt_Check()
{
	CString		sLog;
	double		dStartPos[20], dEndPos[20], dTiltStart, dTiltEnd;
	AXIS_STATUS *pStatus;

	if (m_nThreadSource == 1) {
		if (m_pDX0->iLoadTR1CMTilted) {
			if (m_nTiltSense == 0) {
				pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_1_Y);
				m_dTiltStartPos = pStatus->dPos;
				m_dTiltEndPos = 0;
			}
			m_nTiltSense = 1;
		} else {
			if (m_nTiltSense == 1) {
				pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_1_Y);
				m_dTiltEndPos = pStatus->dPos;
			}
			m_nTiltSense = 0;
		}
	} else {
		if (m_pDX0->iLoadTR2CMTilted) {
			if (m_nTiltSense == 0) {
				pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_2_Y);
				m_dTiltStartPos = pStatus->dPos;
				m_dTiltEndPos = 0;
			}
			m_nTiltSense = 1;
		} else {
			if (m_nTiltSense == 1) {
				pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_TR_2_Y);
				m_dTiltEndPos = pStatus->dPos;
			}
			m_nTiltSense = 0;
		}
	}

	if (m_dTiltStartPos <= 0 || m_dTiltEndPos <= 0 || m_dTiltStartPos <= m_dTiltEndPos) return TRUE;

	CDataManager *pDataManager = CDataManager::Get_Instance();
	MOVE_DATA *pMoveData = pDataManager->Get_pMoveData();

	MODEL_DATA *pModelData = pDataManager->Get_pModelData();
	pDataManager->Read_ModelData();

	if (m_nThreadSource == 1) {
		dTiltStart = pMoveData->dLoadTrayTR1Y[4] - m_dTiltStartPos;
		dTiltEnd   = pMoveData->dLoadTrayTR1Y[4] - m_dTiltEndPos;
	} else {
		dTiltStart = pMoveData->dLoadTrayTR2Y[4] - m_dTiltStartPos;
		dTiltEnd   = pMoveData->dLoadTrayTR2Y[4] - m_dTiltEndPos;
	}

	// Tilt구간 Search
	for(int w=0; w<gData.nArrayW; w++) {
		dStartPos[w] = pModelData->dTrayFirstW + (gData.dTrayPitchW * w) - (gData.dCMSizeW * 0.5);
		dEndPos[w]   = pModelData->dTrayFirstW + (gData.dTrayPitchW * w) + (gData.dCMSizeW * 0.5);
	}

	// Tile 유무 판단
	pLogFile = CLogFile::Get_Instance();
	for(int w=0; w<gData.nArrayW; w++) {
		if(w == 0 || w == 1 || w == 4 || w ==5) continue;

		if((dStartPos[w] > dTiltStart && dStartPos[w] > dTiltEnd) ||
		   (dEndPos[w]   < dTiltStart && dEndPos[w]   < dTiltEnd) ) {
			sLog.Format("TR%d Tilted False TStart[%0.1lf] TEnd[%0.1lf] = GStart[%0.1lf] GEnd[%0.1lf]", m_nThreadSource, dTiltStart, dTiltEnd, dStartPos[w], dEndPos[w]);
			pLogFile->Save_HandlerLog(sLog);
			m_bTilted = FALSE;
		} else {
			sLog.Format("TR%d Tilted True TStart[%0.1lf] TEnd[%0.1lf] = GStart[%0.1lf] GEnd[%0.1lf]", m_nThreadSource, dTiltStart, dTiltEnd, dStartPos[w], dEndPos[w]);
			pLogFile->Save_HandlerLog(sLog);
			m_bTilted = TRUE;
			return FALSE;
		}
	}
	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////

void CSequenceMain::Set_ClearRunData(int nType)
{
	gData.nLoadTrayCount = 0;
	gData.nGoodTrayNo = 0;
	gData.nStatus = 0;	//초기화
	gData.nGoodTrayCount = gData.nInsTrayNo = gData.nInsLineNo = 0;
	if(nType == 0){
		gData.nEmptyTrayCount = 0;		
	}

	for(int i=0; i<4; i++){
		gData.nTrayPos[i] = 0;
		for(int j=0; j<6; j++){
			gData.PickerInfor[i][j] = 0;
		}
	}
	for(int i=0; i<8; i++){
		gData.IDXLineNo[i] = 0;
		gData.IDXInspec[i] = 0;
		for(int j=0; j<6; j++){
			gData.IDXTrayNo[i][j] = 0;
			gData.IndexInfo[i][j] = 0;
			gData.IDXPoNo[i][j] = 0;
		}
	}
	for(int i=0; i<20; i++){
		for(int j=0; j<20; j++){
//			gData.NG1TrayInfo[i][j] = 0;	
//			gData.NG2TrayInfo[i][j] = 0;	
			gData.LoadTrayInfo[i][j] = 0;	
			gData.GoodTrayInfo[i][j] = 0;	
		}
	}

	gData.PickerNGTrayNo = 0;
	for(int i=0; i<6; i++){
		gData.IndexJob[i] = 0;
		gData.PickerNGPoNo[i] = gData.PickerGdPoNo[i] = gData.PickerUnPoNo[i] = 0;
		gData.PickerGdTrayNo[i] = gData.PickerUnTrayNo[i] = 0;
	}

	gData.nNGTrayPos = 0;
	for(int i=0; i<10; i++) {
		gLot.lTackData[i] = 0;
		gLot.nLog[i] = 0;
	}

	for(int i=0; i<30; i++){
		for(int j=0; j<60; j++){
			gLot.sBarLoad[i][j] = "";
			gLot.sNGCode[i][j] = "";
			gLot.sNGText[i][j] = "";

			gLot.nInsResult[i][j] = 0;
			gLot.nBarResult[i][j] = 0;
			gLot.nInsNG[i][j] = 0;
		}
	}

	// LotEnd 되고나서 무조건 FALSE로 바꿔주면 안된다.
	if (nType == 0) {
		gData.bCapTrayLoad = FALSE;
		gData.bCapAttachWork = FALSE;
	}

	gData.nDoorInterlock18 = gData.nDoorInterlock67 = 0;
	g_objMES.m_nMESSequence = 0;
	gLot.nNGT = gLot.nNGC = gLot.nGDT = gLot.nG1DC = 0;
	gAlm.nType = 0;

	m_nUnloadLotIndex = 0;
	m_nUnloadLotCmCnt = 0;
	for(int i=0; i<5; i++) { gData.bContinueLotEnd[i] = FALSE; }
	gData.nLotIdsIndex = 0;

	memset(gLot.nCmJigNo, 0x00, sizeof(int) * 30 * 60 * 10);
}

void CSequenceMain::Beep_Post(int nTime)
{
	if(nTime < 500) nTime = 500;

	m_pDY0->oAlarmBuzzer2 = TRUE;
	m_pAJinAXL->Write_Output(0);

	//CCommon *pCommon = CCommon::Get_Instance();
	//pCommon->Wait_Time(nTime);
	Sleep(nTime);
	m_pDY0->oAlarmBuzzer2 = FALSE;
	m_pAJinAXL->Write_Output(0);
}

///////////////////////////////////////////////////////////////////////////////
// 1. (Error : 2100)
BOOL CSequenceMain::Load1_Run()
{
	double	dPosZ1, dPosZ2;
	AXIS_STATUS *pStatus;

	switch (m_nLoad1Case) {
	case 100:
		if (gData.bCleanOutMode == FALSE) {
			if (gLot.nTrayCount > gData.nLoadTrayCount || gData.nLoadTrayCount == 0) {
				if (m_pDX1->iLS_Load1FCheck) {
					if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 0) && m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 0) ) {
						if (gData.nStatus==0) Job_LotStart();

						m_nLoad1Case = 110;
						pLogFile->Save_MCCLog("Load Stage1",100, "Stage Start");
						m_pCommon->Set_LoopTime(AUTO_LOAD1, 10000);
					}
				}
			}
		}
		break;
	case 110:
		if (!m_pDX1->iLS_Z1AlignS12In && m_pDX1->iLS_Z1AlignS12Out && !m_pDX1->iLS_Z1AlignM34In && m_pDX1->iLS_Z1AlignM34Out) {
#ifdef TRAY_CHECK2
			if (!m_pDX1->iLS_Z1Check1 && !m_pDX1->iLS_Z1Check2)
#else
			if (!m_pDX1->iLS_Z1Check1)
#endif
			{
// 				if (gData.nPortNo == 0) break;	// Cap Attach에서 Port No를 받는다.
				m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 2);
				m_nLoad1Case = 120;
				pLogFile->Save_MCCLog("Load Stage1", 110, "Tray Up Move");
				m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
			}
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 2)) {
			m_pDY1->oLS_LoadSupport12In = FALSE;
			m_pDY1->oLS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 130;
			pLogFile->Save_MCCLog("Load Stage1",  120 , "Tray Up Done & Port Support Out ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 130:
		if (!m_pDX1->iLS_LoadSupport1In && m_pDX1->iLS_LoadSupport1Out && !m_pDX1->iLS_LoadSupport2In && m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 3);
			m_nLoad1Case = 140;
			pLogFile->Save_MCCLog("Load Stage1",  130 , "Z Support Up Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 140:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 3)) {
			m_pDY1->oLS_LoadSupport12In = TRUE;
			m_pDY1->oLS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 150;
			pLogFile->Save_MCCLog("Load Stage1", 140, "Port Support In");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 150:
		if (m_pDX1->iLS_LoadSupport1In && !m_pDX1->iLS_LoadSupport1Out && m_pDX1->iLS_LoadSupport2In && !m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 0);
			m_nLoad1Case = 160;
			pLogFile->Save_MCCLog("Load Stage1", 150, "Z Down to Move Position ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 0)) {
			m_nLoad1Case = 170;
			pLogFile->Save_MCCLog("Load Stage1", 160, "Z Move Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 170:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z1Check1 && m_pDX1->iLS_Z1Check2)
#else
		if (m_pDX1->iLS_Z1Check1)
#endif
		{
			gData.nLoadTrayCount++;

			m_pDY1->oLS_Z1AlignM34In = TRUE;
			m_pDY1->oLS_Z1AlignM34Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 180;
			pLogFile->Save_MCCLog("Load Stage1", 170, "Load Stage Master Align In");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 180:
		if (m_pDX1->iLS_Z1AlignM34In && !m_pDX1->iLS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_pDY1->oLS_Z1AlignS12In = TRUE;
			m_pDY1->oLS_Z1AlignS12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 190;
			pLogFile->Save_MCCLog("Load Stage1", 180, "Slave In ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 190:
		if (m_pDX1->iLS_Z1AlignS12In && !m_pDX1->iLS_Z1AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_nLoad1Case = 191;
			pLogFile->Save_MCCLog("Load Stage1", 190, "Slave In Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 191:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z1Check1 && m_pDX1->iLS_Z1Check2)
#else
		if (m_pDX1->iLS_Z1Check1)
#endif
		{
			m_nLoad1Case = 192;
			pLogFile->Save_MCCLog("Load Stage1", 191, "Tray Exist Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;

	case 192:
#ifdef EQ_TYPE_C
			m_nLoad1Case = 200;
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
#else
		if (m_nLoad2Case < 190 || m_nLoad2Case > 430) 
		{
			m_nLoad1Case = 200;
			pLogFile->Save_MCCLog("Load Stage1", 192, "Stage 2 Collision Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
#endif
		return TRUE;

	case 200:
		if (gData.nTrayPos[0] > 0) {	// Load 줄번호
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 1);	// Wait Pos
			gData.LoadTrayWaitNo = gData.nLoadTrayCount;
			m_nLoad1Case = 210;
			pLogFile->Save_MCCLog("Load Stage1", 200, "Move to Waiting Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		} else {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 2);	// Start Pos
			gData.LoadTrayNo = gData.nLoadTrayCount;
			m_nLoad1Case = 290;
			pLogFile->Save_MCCLog("Load Stage1", 200, "Move to Unloading Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 210:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 1)) {
			m_nLoad1Case = 250;
			pLogFile->Save_MCCLog("Load Stage1", 210, "Move to Waiting Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;

	case 250:
		if (m_nLoad2Case==100 || m_nLoad2Case >=360)
			m_nLoad1Case = 260;
			pLogFile->Save_MCCLog("Load Stage1", 250, "Load Stage 2 Collision Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		break;
	case 260:
		if (gData.nTrayPos[0] > 0) {	// Load 줄번호
			m_nLoad1Case = 250;
			pLogFile->Save_MCCLog("Load Stage1", 260, "Ready For Unloading");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		} else {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 2);	// Start Pos
			gData.LoadTrayNo = gData.LoadTrayWaitNo;
			m_nLoad1Case = 290;
			pLogFile->Save_MCCLog("Load Stage1", 260, "Move to Unloading Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 290:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 2)) {
			gData.nTrayPos[0] = 1;
			Set_LoadTray();
			if(m_nLDPickerCase==100) m_nLDPickerCase=110;

			m_nLoad1Case = 300;
			pLogFile->Save_MCCLog("Load Stage1", 290, "Unloading Start");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;

	case 300:
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		break;
	case 310:
		pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_Z2);
		dPosZ2 = pStatus->dPos;
//		if(dPosZ2 < 10) {
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0) ) {
			if (gData.nTrayPos[0] > 1 || (gData.nTrayPos[0]==1 && (m_nLoad2Case<300 || m_nLoad2Case>430))) {
				gData.nTrayPos[0]++;
				if (gData.nTrayPos[0] > gData.nArrayW) {	// Load 줄번호
					m_nLoad1Case = 350;
					pLogFile->Save_MCCLog("Load Stage1", 310, "Unloading Done");
					m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
				} else {
					m_pCommon->PickerLoad_Move(1, gData.nTrayPos[0], 1);
					m_dLoadTrayX[0] = m_pCommon->m_dP1X[0];
					//m_pAJinAXL->Move_Absolute(AX_LOAD_TRAY_X1, m_dLoadTrayX);
					m_pAJinAXL->Move_Abs_Accel(AX_LOAD_TRAY_X1, m_dLoadTrayX[0], m_pMoveData->dDLoadTrayX1[2]);

					m_nLoad1Case = 320;
					pLogFile->Save_MCCLog("Load Stage1", 310, " Tray Move for Unloading");
					m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
				}
			}
		}
		return TRUE;
	case 320:
		if (m_pAJinAXL->Is_MoveDone(AX_LOAD_TRAY_X1, m_dLoadTrayX[0]) ) {
			if(m_nLDPickerCase==100) m_nLDPickerCase=110;
			m_nLoad1Case = 300;
			pLogFile->Save_MCCLog("Load Stage1", 320, "Make Picker Start to Load modules");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 350:
		if (!m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 3)) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 3);	// Unload Pos

			m_nLoad1Case = 360;
			pLogFile->Save_MCCLog("Load Stage1", 350, "Move to Tray Put Position ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 360:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 3)) {
			gData.nTrayPos[0] = 0;
			if(m_nLoad2Case==250) m_nLoad2Case=260;

			m_pDY1->oLS_Z1AlignS12In = FALSE;
			m_pDY1->oLS_Z1AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 370;
			pLogFile->Save_MCCLog("Load Stage1", 360, "Stage Slave Out");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 370:
		if (!m_pDX1->iLS_Z1AlignS12In && m_pDX1->iLS_Z1AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_pDY1->oLS_Z1AlignM34In = FALSE;
			m_pDY1->oLS_Z1AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 380;
			pLogFile->Save_MCCLog("Load Stage1", 370, "Stage Master Out");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 380:
		if (!m_pDX1->iLS_Z1AlignM34In && m_pDX1->iLS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 3);
			m_nLoad1Case = 390;
			pLogFile->Save_MCCLog("Load Stage1", 380, "Z Move to 'Support Up'");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 390:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 3)) {
			m_pDY1->oLS_UnloadSupport12In = FALSE;
			m_pDY1->oLS_UnloadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 400;
			pLogFile->Save_MCCLog("Load Stage1", 390, "Unload Port Support out");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 400:
		if (!m_pDX1->iLS_UnloadSupport1In && m_pDX1->iLS_UnloadSupport1Out && !m_pDX1->iLS_UnloadSupport2In && m_pDX1->iLS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 2);
			m_nLoad1Case = 410;
			pLogFile->Save_MCCLog("Load Stage1", 400, "Z move to 'Tray Up'");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 410:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 2)) {
			m_pDY1->oLS_UnloadSupport12In = TRUE;
			m_pDY1->oLS_UnloadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 420;
			pLogFile->Save_MCCLog("Load Stage1", 410, "Unload Port Support In");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 420:
		if (m_pDX1->iLS_UnloadSupport1In && !m_pDX1->iLS_UnloadSupport1Out && m_pDX1->iLS_UnloadSupport2In && !m_pDX1->iLS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 1);
			m_nLoad1Case = 430;
			pLogFile->Save_MCCLog("Load Stage1", 420, "Z Move to 'Down'");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 430:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 1)) {
			gData.nEmptyTrayCount++;
			m_nLoad1Case = 440;
			pLogFile->Save_MCCLog("Load Stage1", 430, "Z Down Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;

	case 440:
		if (m_nLoad2Case > 100 && m_nLoad2Case < 440) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 0);
			m_nLoad1Case = 450;
			pLogFile->Save_MCCLog("Load Stage1", 440, "Tray X back to Load");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 450:
		if (m_nLoad2Case > 290) {
			if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 0)) {
				m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 0);
				m_nLoad1Case = 460;
				pLogFile->Save_MCCLog("Load Stage1", 450, "Z Move to 'Ready'");
				m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
			}
		}
		break;
	case 460:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 0)) {
			m_nLoad1Case = 100;
			pLogFile->Save_MCCLog("Load Stage1", 460, "Z Move Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;


	// X1, X2 Tray Sorting 작업
	case 480:
#ifdef TRAY_CHECK2
		if (!m_pDX1->iLS_Z1Check1 && !m_pDX1->iLS_Z1Check2 && !m_pDX1->iLS_Z2Check1 && !m_pDX1->iLS_Z2Check2)
#else
		if (!m_pDX1->iLS_Z1Check1 && !m_pDX1->iLS_Z2Check1)
#endif
		{
			m_nLoad1Case = 481;
			m_nLoad2Case = 100;
			pLogFile->Save_MCCLog("Load Stage1", 480, "Tray Sort Start ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 481:
			pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_Z1);
			dPosZ1 = pStatus->dPos;
			pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_Z2);
			dPosZ2 = pStatus->dPos;

		if (dPosZ1 >= dPosZ2) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 0); //up side 
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 1); //down side
			m_nLoad1Case = 490;
			pLogFile->Save_MCCLog("Load Stage1", 481, "Z1 Move to Ready & Z2 Move to Unloading Position");
		} else {
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 1);//down side
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 0);//up side 
			m_nLoad1Case = 482;
			pLogFile->Save_MCCLog("Load Stage1",481, "Z1 Move to Unloading & Z2 Move to Ready");
		}
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		break;
	case 482:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 1) && m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 0)) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 0); //load port side
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 3); //unload port side 
			m_nLoad1Case = 483;
			pLogFile->Save_MCCLog("Load Stage1", 482, "X1 Move to Get & X2 Move to Put ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 483:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 0) && m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 3)) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 0); // up side
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 1); // down side 
			m_nLoad1Case = 490;
			pLogFile->Save_MCCLog("Load Stage1", 483, "Z1 Move to Ready & Z2 Move to Unload");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;

	case 490:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 0) && m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 1)) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 0); //load port position
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 3); //unload port position 
			m_nLoad1Case = 491;
			pLogFile->Save_MCCLog("Load Stage1",  490 , "X1 Move to Get & X2 Move to Put ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 491:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 0) && m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 3)) {

			//m_pDX1->iLS_Load1FCheck = TRUE; <-- Test할때 썼음
			//if (m_pDX1->iLS_Load1FCheck){
			//	m_nLoad1Case = 500; // 포트가 있으면 미리 한개 트레이만 로딩
			//}
			//else{
			//	m_nLoad1Case = 100;
			//}

			
			m_nLoad1Case = 100;
			m_nLoad2Case = 440;
			pLogFile->Save_MCCLog("Load Stage1", 491, "Tray Sort Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;

	case 500:
		m_nLoad1Case = 501;
		break;

	case 501:
		if (!m_pDX1->iLS_Z1AlignS12In && m_pDX1->iLS_Z1AlignS12Out && !m_pDX1->iLS_Z1AlignM34In && m_pDX1->iLS_Z1AlignM34Out) {
#ifdef TRAY_CHECK2
			if (!m_pDX1->iLS_Z1Check1 && !m_pDX1->iLS_Z1Check2)
#else
			if (!m_pDX1->iLS_Z1Check1)
#endif
			{
				//	if (gData.nPortNo == 0) break;	// Cap Attach에서 Port No를 받는다.
				m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 2);
				m_nLoad1Case = 502;
				pLogFile->Save_MCCLog("Load Stage1", 502, "Tray Up Move");
				m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
			}
		}
		break;
	case 502:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 2)) {
			m_pDY1->oLS_LoadSupport12In = FALSE;
			m_pDY1->oLS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 503;
			pLogFile->Save_MCCLog("Load Stage1",  502 , "Tray Up Done & Port Support Out ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 503:
		if (!m_pDX1->iLS_LoadSupport1In && m_pDX1->iLS_LoadSupport1Out && !m_pDX1->iLS_LoadSupport2In && m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 3); //support up 
			m_nLoad1Case = 504;
			pLogFile->Save_MCCLog("Load Stage1",  503 , "Z Support Up Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 504:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 3)) {
			m_pDY1->oLS_LoadSupport12In = TRUE;
			m_pDY1->oLS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 505;
			pLogFile->Save_MCCLog("Load Stage1", 504, "Port Support In");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 505:
		if (m_pDX1->iLS_LoadSupport1In && !m_pDX1->iLS_LoadSupport1Out && m_pDX1->iLS_LoadSupport2In && !m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z1, 0);
			m_nLoad1Case = 506;
			pLogFile->Save_MCCLog("Load Stage1", 505, "Z Down to Move Position ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 506:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z1, 0)) {
			m_nLoad1Case = 507;
			pLogFile->Save_MCCLog("Load Stage1", 506, "Z Move Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 507:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z1Check1 && m_pDX1->iLS_Z1Check2)
#else
		if (m_pDX1->iLS_Z1Check1)
#endif
		{
			//gData.nLoadTrayCount++; //미리 가있을때는 정보 처리 안함 나중에 스타트 버튼 누르면 한꺼번에 

			m_pDY1->oLS_Z1AlignM34In = TRUE;
			m_pDY1->oLS_Z1AlignM34Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 508;
			pLogFile->Save_MCCLog("Load Stage1", 507, "Load Stage Master Align In");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 508:
		if (m_pDX1->iLS_Z1AlignM34In && !m_pDX1->iLS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_pDY1->oLS_Z1AlignS12In = TRUE;
			m_pDY1->oLS_Z1AlignS12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad1Case = 509;
			pLogFile->Save_MCCLog("Load Stage1", 508, "Slave In ");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 509:
		if (m_pDX1->iLS_Z1AlignS12In && !m_pDX1->iLS_Z1AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD1, 500)) break;
			m_nLoad1Case = 510;
			pLogFile->Save_MCCLog("Load Stage1", 509, "Slave In Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 510:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z1Check1 && m_pDX1->iLS_Z1Check2)
#else
		if (m_pDX1->iLS_Z1Check1)
#endif
		{
			m_nLoad1Case = 511;
			pLogFile->Save_MCCLog("Load Stage1", 510, "Tray Exist Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;

	case 511:
#ifdef EQ_TYPE_C
		m_nLoad1Case = 200;
		m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
#else
		if (m_nLoad2Case < 190 || m_nLoad2Case > 430) 
		{
			m_nLoad1Case = 512;
			pLogFile->Save_MCCLog("Load Stage1", 511, "Stage 2 Collision Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
#endif
		return TRUE;

	case 512:
		{
			m_pCommon->Move_Position(AX_LOAD_TRAY_X1, 2);	// Start Pos
			gData.LoadTrayNo = gData.nLoadTrayCount;
			m_nLoad1Case = 513;
			pLogFile->Save_MCCLog("Load Stage1", 512, "Move to Unloading Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 30000);
		}
		break;
	case 513:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 2)) {
			pLogFile->Save_MCCLog("Load Stage1", 520, "Move to Waiting Done");
			m_nLoad1Case = 520;
			m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		}
		break;
	case 520:
		m_pCommon->Set_LoopTime(AUTO_LOAD1, 5000);
		break;

	

	}

	// 1. (Error : 2100)
	if (m_pCommon->Over_LoopTime(AUTO_LOAD1)) {
		if (m_nLoad1Case == 100 || m_nLoad1Case == 192 || m_nLoad1Case == 250 || m_nLoad1Case == 300 || m_nLoad1Case == 310 || m_nLoad1Case == 440) return TRUE;

		m_pCommon->Show_Error(2000 + m_nLoad1Case);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 2.  (Error : 2600)
BOOL CSequenceMain::Load2_Run()
{
	double	dPosZ1;
	AXIS_STATUS *pStatus;

	if (m_nLoad1Case > 460) return TRUE;
	switch (m_nLoad2Case) {
	case 100:
		if (gData.bCleanOutMode == FALSE) {
			if (gLot.nTrayCount > gData.nLoadTrayCount || gData.nLoadTrayCount == 0) {
				if (m_pDX1->iLS_Load1FCheck) {
					if (m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 0) && m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 0) ) {
						if (gData.nStatus==0) Job_LotStart();

						m_nLoad2Case = 110;
						pLogFile->Save_MCCLog("Load Stage2",100, "Stage Start");
						m_pCommon->Set_LoopTime(AUTO_LOAD2, 10000);
					}
				}
			}
		}
		break;
	case 110:
		if (!m_pDX1->iLS_Z2AlignS12In && m_pDX1->iLS_Z2AlignS12Out && !m_pDX1->iLS_Z2AlignM34In && m_pDX1->iLS_Z2AlignM34Out) {
#ifdef TRAY_CHECK2
			if (!m_pDX1->iLS_Z2Check1 && !m_pDX1->iLS_Z2Check2)
#else
			if (!m_pDX1->iLS_Z2Check1)
#endif
			{
// 				if (gData.nPortNo == 0) break;	// Cap Attach에서 Port No를 받는다.
				m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 2);
				m_nLoad2Case = 120;
				pLogFile->Save_MCCLog("Load Stage2", 110, "Tray Up Move");
				m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
			}
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 2)) {
			m_pDY1->oLS_LoadSupport12In = FALSE;
			m_pDY1->oLS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 130;
			pLogFile->Save_MCCLog("Load Stage2",  120 , "Tray Up Done & Port Support Out ");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 130:
		if (!m_pDX1->iLS_LoadSupport1In && m_pDX1->iLS_LoadSupport1Out && !m_pDX1->iLS_LoadSupport2In && m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 3);
			m_nLoad2Case = 140;
			pLogFile->Save_MCCLog("Load Stage2",  130 , "Z Support Up Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 140:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 3)) {
			m_pDY1->oLS_LoadSupport12In = TRUE;
			m_pDY1->oLS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 150;
			pLogFile->Save_MCCLog("Load Stage2", 140, "Port Support In");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 150:
		if (m_pDX1->iLS_LoadSupport1In && !m_pDX1->iLS_LoadSupport1Out && m_pDX1->iLS_LoadSupport2In && !m_pDX1->iLS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 0);
			m_nLoad2Case = 160;
			pLogFile->Save_MCCLog("Load Stage2", 150, "Z Down to Move Position ");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 0)) {
			m_nLoad2Case = 170;
			pLogFile->Save_MCCLog("Load Stage2", 160, "Z Move Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 170:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z2Check1 && m_pDX1->iLS_Z2Check2)
#else
		if (m_pDX1->iLS_Z2Check1)
#endif
		{
			gData.nLoadTrayCount++;
			m_pDY1->oLS_Z2AlignM34In = TRUE;
			m_pDY1->oLS_Z2AlignM34Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 180;
			pLogFile->Save_MCCLog("Load Stage2", 170, "Load Stage Master Align In");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 180:
		if (m_pDX1->iLS_Z2AlignM34In && !m_pDX1->iLS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 500)) break;
			m_pDY1->oLS_Z2AlignS12In = TRUE;
			m_pDY1->oLS_Z2AlignS12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 190;
			pLogFile->Save_MCCLog("Load Stage2", 180, "Slave In ");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 190:
		if (m_pDX1->iLS_Z2AlignS12In && !m_pDX1->iLS_Z2AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 500)) break;
			m_nLoad2Case = 191;
			pLogFile->Save_MCCLog("Load Stage2", 190, "Slave In Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 191:
#ifdef TRAY_CHECK2
		if (m_pDX1->iLS_Z2Check1 && m_pDX1->iLS_Z2Check2)
#else
		if (m_pDX1->iLS_Z2Check1)
#endif
		{
			m_nLoad2Case = 192;
			pLogFile->Save_MCCLog("Load Stage2", 191, "Tray Exist Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;

	case 192:
#ifdef EQ_TYPE_C
			m_nLoad2Case = 200;
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
#else
		if (m_nLoad1Case < 190 || m_nLoad1Case > 430) 
		{
			m_nLoad2Case = 200;
			pLogFile->Save_MCCLog("Load Stage2", 192, "Stage 1 Collision Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
#endif
		return TRUE;

	case 200:
		if (gData.nTrayPos[0] > 0) {	// Load 줄번호
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 1);	// Wait Pos
			gData.LoadTrayWaitNo = gData.nLoadTrayCount;
			m_nLoad2Case = 210;
			pLogFile->Save_MCCLog("Load Stage2", 200, "Move to Waiting Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		} else {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 2);	// Start Pos
			gData.LoadTrayNo = gData.nLoadTrayCount;
			m_nLoad2Case = 290;
			pLogFile->Save_MCCLog("Load Stage2", 200, "Move to Unloading Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 210:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 1)) {
			m_nLoad2Case = 250;
			pLogFile->Save_MCCLog("Load Stage2", 210, "Move to Waiting Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;

	case 250:
		if (m_nLoad1Case==100 || m_nLoad1Case >=360)
			m_nLoad2Case = 260;
			pLogFile->Save_MCCLog("Load Stage2", 250, "Load Stage 1 Collision Check");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		break;
	case 260:
		if (gData.nTrayPos[0] > 0) {	// Load 줄번호
			m_nLoad2Case = 250;
			pLogFile->Save_MCCLog("Load Stage2", 260, "Ready For Unloading");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		} else {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 2);	// Start Pos
			gData.LoadTrayNo = gData.LoadTrayWaitNo;
			m_nLoad2Case = 290;
			pLogFile->Save_MCCLog("Load Stage2", 260, "Move to Unloading Position");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 290:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 2)) {
			gData.nTrayPos[0] = 1;
			Set_LoadTray();
			if(m_nLDPickerCase==100) m_nLDPickerCase=110;

			m_nLoad2Case = 300;
			pLogFile->Save_MCCLog("Load Stage2", 290, "Unloading Start");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;

	case 300:
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		break;
	case 310:
		pStatus = m_pAJinAXL->Get_pStatus(AX_LOAD_TRAY_Z1);
		dPosZ1 = pStatus->dPos;
//		if(dPosZ1 < 10) {
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0) ) {
			if (gData.nTrayPos[0] > 1 || (gData.nTrayPos[0]==1 && (m_nLoad1Case<300 || m_nLoad1Case>430))) {
				gData.nTrayPos[0]++;
				if (gData.nTrayPos[0] > gData.nArrayW) {	// Load 줄번호
					m_nLoad2Case = 350;
					pLogFile->Save_MCCLog("Load Stage2", 310, "Unloading Done");
					m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
				} else {
					m_pCommon->PickerLoad_Move(1, gData.nTrayPos[0], 2);
					m_dLoadTrayX[1] = m_pCommon->m_dP1X[1];
					//m_pAJinAXL->Move_Absolute(AX_LOAD_TRAY_X2, m_dLoadTrayX);
					m_pAJinAXL->Move_Abs_Accel(AX_LOAD_TRAY_X2, m_dLoadTrayX[1], m_pMoveData->dDLoadTrayX2[2]);

					m_nLoad2Case = 320;
					pLogFile->Save_MCCLog("Load Stage2", 310, " Tray Move for Unloading");
					m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
				}
			}
		}
		return TRUE;
	case 320:
		if (m_pAJinAXL->Is_MoveDone(AX_LOAD_TRAY_X2, m_dLoadTrayX[1]) ) {
			if(m_nLDPickerCase==100) m_nLDPickerCase=110;
			m_nLoad2Case = 300;
			pLogFile->Save_MCCLog("Load Stage2", 320, "Make Picker Start to Load modules");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 350:
		if (!m_pCommon->Check_Position(AX_LOAD_TRAY_X1, 3)) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 3);	// Unload Pos

			m_nLoad2Case = 360;
			pLogFile->Save_MCCLog("Load Stage2", 350, "Move to Tray Put Position ");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 360:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 3)) {
			gData.nTrayPos[0] = 0;
			if(m_nLoad1Case==250) m_nLoad1Case=260;

			m_pDY1->oLS_Z2AlignS12In = FALSE;
			m_pDY1->oLS_Z2AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 370;
			pLogFile->Save_MCCLog("Load Stage2", 360, "Stage Slave Out");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 370:
		if (!m_pDX1->iLS_Z2AlignS12In && m_pDX1->iLS_Z2AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 500)) break;
			m_pDY1->oLS_Z2AlignM34In = FALSE;
			m_pDY1->oLS_Z2AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 380;
			pLogFile->Save_MCCLog("Load Stage2", 370, "Stage Master Out");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 380:
		if (!m_pDX1->iLS_Z2AlignM34In && m_pDX1->iLS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 500)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 3);
			m_nLoad2Case = 390;
			pLogFile->Save_MCCLog("Load Stage2", 380, "Z Move to 'Support Up'");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 390:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 3)) {
			m_pDY1->oLS_UnloadSupport12In = FALSE;
			m_pDY1->oLS_UnloadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 400;
			pLogFile->Save_MCCLog("Load Stage2", 390, "Unload Port Support out");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 400:
		if (!m_pDX1->iLS_UnloadSupport1In && m_pDX1->iLS_UnloadSupport1Out && !m_pDX1->iLS_UnloadSupport2In && m_pDX1->iLS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 2);
			m_nLoad2Case = 410;
			pLogFile->Save_MCCLog("Load Stage2", 400, "Z move to 'Tray Up'");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 410:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 2)) {
			m_pDY1->oLS_UnloadSupport12In = TRUE;
			m_pDY1->oLS_UnloadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(1);
			m_nLoad2Case = 420;
			pLogFile->Save_MCCLog("Load Stage2", 410, "Unload Port Support In");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;
	case 420:
		if (m_pDX1->iLS_UnloadSupport1In && !m_pDX1->iLS_UnloadSupport1Out && m_pDX1->iLS_UnloadSupport2In && !m_pDX1->iLS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 1);
			m_nLoad2Case = 430;
			pLogFile->Save_MCCLog("Load Stage2", 420, "Z Move to 'Down'");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 430:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 1)) {
			gData.nEmptyTrayCount++;
			m_nLoad2Case = 440;
			pLogFile->Save_MCCLog("Load Stage2", 430, "Z Down Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;

	case 440:
		if (m_nLoad1Case > 100 && m_nLoad1Case < 440) {
			m_pCommon->Move_Position(AX_LOAD_TRAY_X2, 0);
			m_nLoad2Case = 450;
			pLogFile->Save_MCCLog("Load Stage2", 440, "Tray X back to Load");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
		}
		break;
	case 450:
		if (m_nLoad1Case > 290) {
			if (m_pCommon->Check_Position(AX_LOAD_TRAY_X2, 0)) {
				m_pCommon->Move_Position(AX_LOAD_TRAY_Z2, 0);
				m_nLoad2Case = 460;
				pLogFile->Save_MCCLog("Load Stage2", 450, "Z Move to 'Ready'");
				m_pCommon->Set_LoopTime(AUTO_LOAD2, 30000);
			}
		}
		break;
	case 460:
		if (m_pCommon->Check_Position(AX_LOAD_TRAY_Z2, 0)) {
			m_nLoad2Case = 100;
			pLogFile->Save_MCCLog("Load Stage2", 460, "Z Move Done");
			m_pCommon->Set_LoopTime(AUTO_LOAD2, 5000);
		}
		break;

	}

	// 2.  (Error : 2600)
	if (m_pCommon->Over_LoopTime(AUTO_LOAD2)) {
		if (m_nLoad2Case == 100 || m_nLoad2Case == 192 || m_nLoad2Case == 250 || m_nLoad2Case == 300 || m_nLoad2Case == 310 || m_nLoad2Case == 440) return TRUE;

		m_pCommon->Show_Error(2500 + m_nLoad2Case);
		return FALSE;
	} else {
		return TRUE;
	}
}
	
// 3.  (Error : 3100)
BOOL CSequenceMain::Unload1_Run()
{
	double	dPosZ1, dPosZ2;
	double	dPosY1, dPosY2;
	AXIS_STATUS *pStatus;

	static int nSenseCnt = 0;

	if (m_pEquipData->bUseDoorLock && gData.nDoorInterlock67 == 1) {
		if (m_nUnload1Case != 130 && m_nUnload1Case != 140 && m_nUnload1Case != 400 && m_nUnload1Case != 410 &&
			m_nUnload1Case != 491 && m_nUnload1Case != 492 && m_nUnload1Case != 504 && m_nUnload1Case != 505) {
			if (!m_pDX0->iMainDoor6Close || !m_pDX0->iMainDoor7Close) {
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 10000);
				return TRUE;
			}
		}
	}

	switch (m_nUnload1Case) {
	case 100:
		if (Check_IndexEmpty(9)==FALSE || Check_PickerEmpty()==FALSE) {
			if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0)) {
				if (m_pDX6->iUS_Load1FCheck) {
					m_nUnload1Case = 110;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
				}
			}
		}
		break;
	case 110:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2)	// Head에 Tray 있음
#else
		if (m_pDX6->iUS_Z1Check1)	// Head에 Tray 있음
#endif
		{
			if (m_pDX6->iUS_Z1AlignS12In && !m_pDX6->iUS_Z1AlignS12Out && m_pDX6->iUS_Z1AlignM34In && !m_pDX6->iUS_Z1AlignM34Out) {
				m_nUnload1Case = 200;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
			}
		} else {					// Head에 TRAY 없음
			if (!m_pDX6->iUS_Z1AlignS12In && m_pDX6->iUS_Z1AlignS12Out && !m_pDX6->iUS_Z1AlignM34In && m_pDX6->iUS_Z1AlignM34Out) {
//				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 3000)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 2);	// Tray Up
				m_nUnload1Case = 120;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
			}
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 2)) {
			m_pDY6->oUS_LoadSupport12In = FALSE;
			m_pDY6->oUS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 130;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 130:
		if (!m_pDX6->iUS_LoadSupport1In && m_pDX6->iUS_LoadSupport1Out && !m_pDX6->iUS_LoadSupport2In && m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 3);	//Support Up
			m_nUnload1Case = 140;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 140:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 3)) {
			m_pDY6->oUS_LoadSupport12In = TRUE;
			m_pDY6->oUS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 150;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 150:
		if (m_pDX6->iUS_LoadSupport1In && !m_pDX6->iUS_LoadSupport1Out && m_pDX6->iUS_LoadSupport2In && !m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);	// Move Up
			m_nUnload1Case = 160;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0)) {
			m_nUnload1Case = 170;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 170:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2)
#else
		if (m_pDX6->iUS_Z1Check1)
#endif
		{
			m_pDY6->oUS_Z1AlignM34In = TRUE;
			m_pDY6->oUS_Z1AlignM34Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 180;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 180:
		if (m_pDX6->iUS_Z1AlignM34In && !m_pDX6->iUS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 100)) break;
			m_pDY6->oUS_Z1AlignS12In = TRUE;
			m_pDY6->oUS_Z1AlignS12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 190;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 190:
		if (m_pDX6->iUS_Z1AlignS12In && !m_pDX6->iUS_Z1AlignS12Out) {
			m_nUnload1Case = 191;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 191:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2)
#else
		if (m_pDX6->iUS_Z1Check1)
#endif
		{
			m_nUnload1Case = 200;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 200:
// 		if (gData.nTrayPos[3] > 0) {
// 			if (m_nUnload2Case < 200 || m_nUnload2Case > 260) {
// 				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 1);	// Wait Position
// 				m_nUnload1Case = 210;
// 				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
// 			}
// 		} else {
			if (m_nUnload2Case < 200 || m_nUnload2Case > 350) {
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 2);
				m_nUnload1Case = 290;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
			}
// 		}
		return TRUE;
// 	case 210:
// 		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 1)) {
// 			m_nUnload1Case = 250;
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
// 		}
// 		break;
// 	case 250:	// Waiting
// 		if (m_nUnload2Case <= 200 || m_nUnload2Case > 350) {
// 			m_nUnload1Case = 260;
// 		}
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 15000);
// 		break;
// 	case 260:
// 		if (gData.nTrayPos[3] > 0) {
// 			m_nUnload1Case = 250;
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
// 		} else {
// 			if (m_nUnload2Case <= 200 || m_nUnload2Case > 350) {
// 				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 2);
// 				m_nUnload1Case = 290;
// 				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
// 			}
// 		}
// 		break;

	case 290:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 2)) {
			gData.nTrayPos[3] = 1;
			Set_GoodTrayEmpty();
			m_nUnload1Case = 300;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 300:	// Working
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		break;

	case 310:
		pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Z2);
		dPosZ2 = pStatus->dPos;
		if (dPosZ2 < 50 || m_nUnload2Case <= 200) {
			gData.nTrayPos[3]++;
			if ((gData.nTrayPos[3] > gData.nArrayW || (Check_LotEnd()==TRUE))) {	// Load 줄번호
				if (m_pEquipData->bUseInlineMode) gData.nCTrayCmCnt = Get_GoodTrayCmCnt();
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 3);	// Unload Pos
				m_nUnload1Case = 350;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
			} else {
				m_pCommon->PickerUnload_Move(2, gData.nTrayPos[3], 1);
				m_dUnloadTrayY[0] = m_pCommon->m_dP2Y[0];
				//m_pAJinAXL->Move_Absolute(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY);
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0], m_pMoveData->dDUnloadTrayY1[2]);

				m_nUnload1Case = 320;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
			}
		}
		break;
	case 320:
		if (m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0]) ) {
			m_nUnload1Case = 300;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 350:
		if (!m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 3) || m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 3)) {
			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Y1);
			dPosY1 = pStatus->dPos;
			double dPos = m_pMoveData->dUnloadTrayY1[2];
			if (dPosY1 <= dPos) {
				gData.nTrayPos[3] = 0;
				if (Check_GoodTrayEmpty(0)==TRUE) {
					m_nUnload1Case = 420;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
				} else {
					if(m_pEquipData->bUseInlineMode) m_nUnload1Case = 351;
					else m_nUnload1Case = 360;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
				}
				if(m_nUnload2Case==250) m_nUnload2Case=260;
			}
		}
		break;
	case 351:
		if (m_pDX6->iUS_Unload1FCheck)
		{
			//m_sLog.Format("m_nUnload1Case,%d, %d",m_nUnload1Case,m_pDX6->iUS_Unload1FCheck); pLogFile->Save_MCCLog(m_sLog);
			break;
		}
		else 
		{
			//m_sLog.Format("m_nUnload1Case,%d, %d",m_nUnload1Case,m_pDX6->iUS_Unload1FCheck); pLogFile->Save_MCCLog(m_sLog);
			nSenseCnt++;
			if(nSenseCnt > 100)
			{
				//m_sLog.Format("m_nUnload1Case,%d, %d",m_nUnload1Case,m_pDX6->iUS_Unload1FCheck); pLogFile->Save_MCCLog(m_sLog);
				nSenseCnt = 0;
				m_nUnload1Case = 360;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 35000);
				
			}
		}
		break;
		
	case 360:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 3)) {
			if (m_pEquipData->bUseInlineMode) {
				if (gData.bCapTrayLoad) break;
				if ( m_pDX6->iUS_Unload1FCheck ) break;
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
				m_pDY6->oUS_UnloadSupport12In = FALSE;
				m_pDY6->oUS_UnloadSupport12Out = TRUE;
				m_pAJinAXL->Write_Output(6);
				m_nUnload1Case = 400;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 35000);
			} else {
				m_pDY6->oUS_Z1AlignS12In = FALSE;
				m_pDY6->oUS_Z1AlignS12Out = TRUE;
				m_pAJinAXL->Write_Output(6);
				m_nUnload1Case = 370;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
			}
		}
		break;

	case 370:
		if (!m_pDX6->iUS_Z1AlignS12In && m_pDX6->iUS_Z1AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 100)) break;
			m_pDY6->oUS_Z1AlignM34In = FALSE;
			m_pDY6->oUS_Z1AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 380;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 380:
		if (!m_pDX6->iUS_Z1AlignM34In && m_pDX6->iUS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 3);	// Support Up
			m_nUnload1Case = 390;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 390:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 3)) {
			m_pDY6->oUS_UnloadSupport12In = FALSE;
			m_pDY6->oUS_UnloadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 400;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 400:
		if (!m_pDX6->iUS_UnloadSupport1In && m_pDX6->iUS_UnloadSupport1Out && !m_pDX6->iUS_UnloadSupport2In && m_pDX6->iUS_UnloadSupport2Out)
		{
			if (m_pEquipData->bUseInlineMode)
			{
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
				if ( m_pDX6->iUS_Unload1FCheck ) break;
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 2);	// Tray Up
				m_nUnload1Case = 401;
			}
			else
			{
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 2);	// Tray Up
				m_nUnload1Case = 410;
			}
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;

	case 401:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 2)) {
			m_pDY6->oUS_Z1AlignS12In = FALSE;
			m_pDY6->oUS_Z1AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 402;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 10000);
		}
		break;
	case 402:
		if (!m_pDX6->iUS_Z1AlignS12In && m_pDX6->iUS_Z1AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 100)) break;
			m_pDY6->oUS_Z1AlignM34In = FALSE;
			m_pDY6->oUS_Z1AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 403;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 10000);
		}
		break;
	case 403:
		if (!m_pDX6->iUS_Z1AlignM34In && m_pDX6->iUS_Z1AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 500)) break;
			m_nUnload1Case = 410;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;

	case 410:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 2)) {
			m_pDY6->oUS_UnloadSupport12In = TRUE;
			m_pDY6->oUS_UnloadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 411;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 411:
		if (m_pDX6->iUS_UnloadSupport1In && !m_pDX6->iUS_UnloadSupport1Out && m_pDX6->iUS_UnloadSupport2In && !m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 1);	// Move Down
			m_nUnload1Case = 412;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 412:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 1)) {
			m_nUnload1Case = 413;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 413:
#ifdef TRAY_CHECK2
		if (!m_pDX6->iUS_Z1Check1 && !m_pDX6->iUS_Z1Check2)
#else
		if (!m_pDX6->iUS_Z1Check1)
#endif
		{
			m_nUnload1Case = 430;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;

	case 420:
		if (m_pDX6->iUS_UnloadSupport1In && !m_pDX6->iUS_UnloadSupport1Out && m_pDX6->iUS_UnloadSupport2In && !m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 1);	// Move Down
			m_nUnload1Case = 430;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;

	case 430:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 1)) {
			gData.nGoodTrayCount++;
			if (m_pEquipData->bUseInlineMode) {
				gData.bCapTrayLoad = TRUE;
				g_objCapAttachUDP.Set_TrayLoad(gData.nPortNo);
				//g_objCapAttachUDP.Set_TrayLoad(1);
				// Cap Attach에서도 투입되는 Lot 한꺼번에 Lotend 될수 있도록 해주자.
// 				if (m_nUnloadLotIndex > 0) {
// 					if (gData.bContinueLotEnd[m_nUnloadLotIndex-1] == TRUE) {
// 						g_objCapAttachUDP.Set_TrayEnd(gData.nPortNo);
// 						Sleep(30);
// 						g_objCapAttachUDP.Set_LotEnd(gData.nPortNo);
// 					}
// 				}
			}
			m_nUnload1Case = 440;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 440:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 2) || m_nUnload2Case >= 300 ) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 0);
			m_nUnload1Case = 450;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 450:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 0)) {
			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Y2);
			dPosY2 = pStatus->dPos;
			double dPos = 0.0;
			if (gData.bUnloadSortFront)  dPos = m_pMoveData->dUnloadTrayY2[2];
			else						 dPos = m_pMoveData->dUnloadTrayY2[2] - (gData.nArrayW * gData.dTrayPitchW);

			if (m_nUnload2Case > 350 && dPosY2 <= dPos)
			{
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);
				m_nUnload1Case = 460;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
			
			} else { return TRUE; }			
		}
		break;
	case 460:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0)) {
			m_nUnload1Case = 100;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	// 후처리
	case 480:
#ifdef TRAY_CHECK2
		if (!m_pDX6->iUS_Z1Check1 && !m_pDX6->iUS_Z1Check2 && !m_pDX6->iUS_Z2Check1 && !m_pDX6->iUS_Z2Check2)
#else
		if (!m_pDX6->iUS_Z1Check1 && !m_pDX6->iUS_Z2Check1)
#endif
		{
			m_nUnload1Case = 470;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		} else {
			m_nUnload1Case = 481;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 481:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2)
#else
		if (m_pDX6->iUS_Z1Check1)
#endif
		{
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload1Case = 484;
		} else {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 1);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 0);
			m_nUnload1Case = 482;
		}
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		break;
	case 482:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 1) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 3);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 0);
			m_nUnload1Case = 483;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 483:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 3) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 0)) {
			m_nUnload1Case = 500;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 484:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 1)) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 3);
			m_nUnload1Case = 485;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 485:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3)) {
			m_nUnload1Case = 486;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 486:
			m_nUnload1Case = 487;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		break;
	case 487:
			m_pDY6->oUS_Z1AlignS12In = FALSE;
			m_pDY6->oUS_Z1AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 488;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		break;
	case 488:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2 && !m_pDX6->iUS_Z1AlignS12In && m_pDX6->iUS_Z1AlignS12Out)
#else
		if (m_pDX6->iUS_Z1Check1 && !m_pDX6->iUS_Z1AlignS12In && m_pDX6->iUS_Z1AlignS12Out)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
			m_pDY6->oUS_Z1AlignM34In = FALSE;
			m_pDY6->oUS_Z1AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 489;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 489:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2 && !m_pDX6->iUS_Z1AlignM34In && m_pDX6->iUS_Z1AlignM34Out)
#else
		if (m_pDX6->iUS_Z1Check1 && !m_pDX6->iUS_Z1AlignM34In && m_pDX6->iUS_Z1AlignM34Out)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 3);
			m_nUnload1Case = 490;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 490:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 3)) {
			m_pDY6->oUS_LoadSupport12In = FALSE;
			m_pDY6->oUS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 491;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 491:
		if (!m_pDX6->iUS_LoadSupport1In && m_pDX6->iUS_LoadSupport1Out && !m_pDX6->iUS_LoadSupport2In && m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 2);
			m_nUnload1Case = 492;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 492:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 2)) {
			m_pDY6->oUS_LoadSupport12In = TRUE;
			m_pDY6->oUS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 493;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 493:
		if (m_pDX6->iUS_LoadSupport1In && !m_pDX6->iUS_LoadSupport1Out && m_pDX6->iUS_LoadSupport2In && !m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);
			m_nUnload1Case = 494;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 494:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0)) {
			m_nUnload1Case = 480;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 500:
			m_pDY6->oUS_Z2AlignS12In = FALSE;
			m_pDY6->oUS_Z2AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 501;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		break;
	case 501:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2 && !m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out)
#else
		if (m_pDX6->iUS_Z2Check1 && !m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 300)) break;
			m_pDY6->oUS_Z2AlignM34In = FALSE;
			m_pDY6->oUS_Z2AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 502;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 502:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2 && !m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out)
#else
		if (m_pDX6->iUS_Z2Check1 && !m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 3);
			m_nUnload1Case = 503;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 503:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 3)) {
			m_pDY6->oUS_LoadSupport12In = FALSE;
			m_pDY6->oUS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 504;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 504:
		if (!m_pDX6->iUS_LoadSupport1In && m_pDX6->iUS_LoadSupport1Out && !m_pDX6->iUS_LoadSupport2In && m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 2);
			m_nUnload1Case = 505;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 505:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 2)) {
			m_pDY6->oUS_LoadSupport12In = TRUE;
			m_pDY6->oUS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload1Case = 506;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;
	case 506:
		if (m_pDX6->iUS_LoadSupport1In && !m_pDX6->iUS_LoadSupport1Out && m_pDX6->iUS_LoadSupport2In && !m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD1, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 0);
			m_nUnload1Case = 507;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 507:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
			m_nUnload1Case = 480;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	case 470:
/*			m_pDY2->oInspCMAlign1In = FALSE;
			m_pDY2->oInspCMAlign1Out = TRUE;
			m_pDY2->oInspCMAlign2In = FALSE;
			m_pDY2->oInspCMAlign2Out = TRUE;
			m_pDY2->oInspCMAlign3In = FALSE;
			m_pDY2->oInspCMAlign3Out = TRUE;
			m_pDY2->oInspCMAlign4In = FALSE;
			m_pDY2->oInspCMAlign4Out = TRUE;
			m_pAJinAXL->Write_Output(2);
*/
			m_nUnload1Case = 471;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		break;
	case 471:
			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Z1);
			dPosZ1 = pStatus->dPos;

			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Z2);
			dPosZ2 = pStatus->dPos;

		if (dPosZ1 >= dPosZ2) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload1Case = 474;
		} else {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 1);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 0);
			m_nUnload1Case = 472;
		}
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		break;
	case 472:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 1) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 3);
			m_nUnload1Case = 473;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 473:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3)) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload1Case = 474;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 474:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 1)) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y1, 0);
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 3);
			m_nUnload1Case = 475;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);
		}
		break;
	case 475:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3)) {
			m_nUnload1Case = 100;
			m_nUnload2Case = 440;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 5000);
		}
		break;

	}

	// 3.  (Error : 3100)
	if (m_pCommon->Over_LoopTime(AUTO_UNLOAD1)) {
		if (m_nUnload1Case == 100 || m_nUnload1Case == 250 || m_nUnload1Case == 300 || m_nUnload1Case == 310 || m_nUnload1Case == 440) return TRUE;
		
		m_pCommon->Show_Error(3000 + m_nUnload1Case	);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 4.  (Error : 3600)
BOOL CSequenceMain::Unload2_Run()
{
	double	dPosZ1;
	double	dPosY1, dPosY2;
	AXIS_STATUS *pStatus;

	static int nSenseCnt = 0;

	if (m_pEquipData->bUseDoorLock && gData.nDoorInterlock67 == 1) {
		if (m_nUnload2Case != 130 && m_nUnload2Case != 140 && m_nUnload2Case != 400 && m_nUnload2Case != 410 &&
			m_nUnload2Case != 491 && m_nUnload2Case != 492 && m_nUnload2Case != 504 && m_nUnload2Case != 505) {
			if (!m_pDX0->iMainDoor6Close || !m_pDX0->iMainDoor7Close) {
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 10000);
				return TRUE;
			}
		}
	}

// 	if (m_pDX1->iLS_Load1FCheck && !m_pDX6->iUS_Load1FCheck) {
// 		m_pCommon->Show_Error(3600);
// 		return FALSE;
// 	}

	switch (m_nUnload2Case) {
	case 100:
		if (Check_IndexEmpty(9)==FALSE || Check_PickerEmpty()==FALSE) {
			if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 0) && m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
				if (m_pDX6->iUS_Load1FCheck && m_nUnload1Case < 460) {
					m_nUnload2Case = 110;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
				}
			}
		}
		break;
	case 110:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2)	// Head에 Tray 있음
#else
		if (m_pDX6->iUS_Z2Check1)	// Head에 Tray 있음
#endif
		{
			if (m_pDX6->iUS_Z2AlignS12In && !m_pDX6->iUS_Z2AlignS12Out && m_pDX6->iUS_Z2AlignM34In && !m_pDX6->iUS_Z2AlignM34Out) {
				m_nUnload2Case = 200;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
			}
		} else {					// Head에 TRAY 없음
			if (!m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out && !m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out) {
//				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 3000)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 2);
				m_nUnload2Case = 120;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
			}
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 2)) {
			m_pDY6->oUS_LoadSupport12In = FALSE;
			m_pDY6->oUS_LoadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 130;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 130:
		if (!m_pDX6->iUS_LoadSupport1In && m_pDX6->iUS_LoadSupport1Out && !m_pDX6->iUS_LoadSupport2In && m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 3);
			m_nUnload2Case = 140;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 140:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 3)) {
			m_pDY6->oUS_LoadSupport12In = TRUE;
			m_pDY6->oUS_LoadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 150;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 150:
		if (m_pDX6->iUS_LoadSupport1In && !m_pDX6->iUS_LoadSupport1Out && m_pDX6->iUS_LoadSupport2In && !m_pDX6->iUS_LoadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 0);
			m_nUnload2Case = 160;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
			m_nUnload2Case = 170;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 170:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2)
#else
		if (m_pDX6->iUS_Z2Check1)
#endif
		{
			m_pDY6->oUS_Z2AlignM34In = TRUE;
			m_pDY6->oUS_Z2AlignM34Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 180;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 180:
		if (m_pDX6->iUS_Z2AlignM34In && !m_pDX6->iUS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 100)) break;
			m_pDY6->oUS_Z2AlignS12In = TRUE;
			m_pDY6->oUS_Z2AlignS12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 190;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 190:
		if (m_pDX6->iUS_Z2AlignS12In && !m_pDX6->iUS_Z2AlignS12Out) {
			m_nUnload2Case = 191;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 191:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2)
#else
		if (m_pDX6->iUS_Z2Check1)
#endif
		{
			m_nUnload2Case = 200;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;

	case 200:
// 		if (gData.nTrayPos[3] > 0) {
// 			if (m_nUnload1Case < 200 || m_nUnload1Case > 260) {
// 				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 1);
// 				m_nUnload2Case = 210;
// 				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
// 			}
// 		} else {
			if (m_nUnload1Case < 200 || m_nUnload1Case > 350) {
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 2);
				m_nUnload2Case = 290;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
			}
// 		}
		return TRUE;
// 	case 210:
// 		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 1)) {
// 			m_nUnload2Case = 250;
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
// 		}
// 		break;
// 	case 250:
// 		if (m_nUnload1Case <= 200 || m_nUnload1Case > 430) { 
// 			m_nUnload2Case = 260;
// 		}
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 15000);
// 		break;
// 	case 260:
// 		if (gData.nTrayPos[3] > 0) {
// 			m_nUnload2Case = 250;
// 			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
// 		} else {
// 			if (m_nUnload1Case <= 200 || m_nUnload1Case > 430) {
// 				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 2);
// 				m_nUnload2Case = 290;
// 				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
// 			}
// 		}
// 		break;

	case 290:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 2)) {
			gData.nTrayPos[3] = 1;
			Set_GoodTrayEmpty();
			m_nUnload2Case = 300;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;

	case 300:
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		break;

	case 310:
		pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Z1);
		dPosZ1 = pStatus->dPos;
		if (dPosZ1 < 50 || m_nUnload1Case <= 200) {
			gData.nTrayPos[3]++;
			if ((gData.nTrayPos[3] > gData.nArrayW || (Check_LotEnd()==TRUE))) {	// Load 줄번호
				if (m_pEquipData->bUseInlineMode) gData.nCTrayCmCnt = Get_GoodTrayCmCnt();
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 3);	// Unload Pos
				m_nUnload2Case = 350;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
			} else {
				m_pCommon->PickerUnload_Move(2, gData.nTrayPos[3], 2);
				m_dUnloadTrayY[1] = m_pCommon->m_dP2Y[1];
				//m_pAJinAXL->Move_Absolute(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY);
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1], m_pMoveData->dDUnloadTrayY2[2]);

				m_nUnload2Case = 320;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
			}
		}
		break;
	case 320:
		if (m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1]) ) {
			m_nUnload2Case = 300;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 350:
		if (!m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3) || m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3)) {
			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Y2);
			dPosY2 = pStatus->dPos;
			double dPos = m_pMoveData->dUnloadTrayY2[2];
			if (dPosY2 <= dPos) {
				gData.nTrayPos[3] = 0;
				if (Check_GoodTrayEmpty(0)==TRUE) {
					m_nUnload2Case = 420;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
				} else {
					if(m_pEquipData->bUseInlineMode) m_nUnload2Case = 351;
					else m_nUnload2Case = 360;
					m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
				}
				if(m_nUnload1Case==250) m_nUnload1Case=260;
			}
		}
		break;
	case 351:
		if (m_pDX6->iUS_Unload1FCheck)
		{
			//m_sLog.Format("m_nUnload2Case,%d, %d",m_nUnload2Case,m_pDX6->iUS_Unload1FCheck); pLogFile->Save_MCCLog(m_sLog);
			break;
		}
		else 
		{
			//m_sLog.Format("m_nUnload2Case,%d, %d",m_nUnload2Case,m_pDX6->iUS_Unload1FCheck); pLogFile->Save_MCCLog(m_sLog);
			nSenseCnt++;
			if(nSenseCnt > 100)
			{
				nSenseCnt = 0;
				m_nUnload2Case = 360;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD1, 30000);

			}
		}
		break;
	case 360:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 3)) {
			if (m_pEquipData->bUseInlineMode) {
				if (gData.bCapTrayLoad ) break;
				if(m_pDX6->iUS_Unload1FCheck ) break;
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 300)) break;
				m_pDY6->oUS_UnloadSupport12In = FALSE;
				m_pDY6->oUS_UnloadSupport12Out = TRUE;
				m_pAJinAXL->Write_Output(6);
				m_nUnload2Case = 400;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 40000);
			} else {
				m_pDY6->oUS_Z2AlignS12In = FALSE;
				m_pDY6->oUS_Z2AlignS12Out = TRUE;
				m_pAJinAXL->Write_Output(6);
				m_nUnload2Case = 370;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
			}
		}
		break;
	case 370:
		if (!m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 100)) break;
			m_pDY6->oUS_Z2AlignM34In = FALSE;
			m_pDY6->oUS_Z2AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 380;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 380:
		if (!m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 3);
			m_nUnload2Case = 390;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 390:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 3)) {
			m_pDY6->oUS_UnloadSupport12In = FALSE;
			m_pDY6->oUS_UnloadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 400;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 400:
		if (!m_pDX6->iUS_UnloadSupport1In && m_pDX6->iUS_UnloadSupport1Out && !m_pDX6->iUS_UnloadSupport2In && m_pDX6->iUS_UnloadSupport2Out) 
		{
			if (m_pEquipData->bUseInlineMode)
			{
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 300)) break;
				if( m_pDX6->iUS_Unload1FCheck ) break;
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 300)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 2);
				m_nUnload2Case = 401;
			}
			else
			{			
				if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 300)) break;
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 2);
				m_nUnload2Case = 410;
			}
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 401:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 2)) {
			m_pDY6->oUS_Z2AlignS12In = FALSE;
			m_pDY6->oUS_Z2AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 402;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 10000);
		}
		break;
	case 402:
		if (!m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 100)) break;
			m_pDY6->oUS_Z2AlignM34In = FALSE;
			m_pDY6->oUS_Z2AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 403;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 10000);
		}
		break;
	case 403:
		if (!m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_nUnload2Case = 410;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;

	case 410:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 2)) {
			m_pDY6->oUS_UnloadSupport12In = TRUE;
			m_pDY6->oUS_UnloadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 411;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 411:
		if (m_pDX6->iUS_UnloadSupport1In && !m_pDX6->iUS_UnloadSupport1Out && m_pDX6->iUS_UnloadSupport2In && !m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload2Case = 412;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 412:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 1)) {
			m_nUnload2Case = 413;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 413:
#ifdef TRAY_CHECK2
		if (!m_pDX6->iUS_Z2Check1 && !m_pDX6->iUS_Z2Check2)
#else
		if (!m_pDX6->iUS_Z2Check1)
#endif
		{
			m_nUnload2Case = 430;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;

	case 420:
		if (m_pDX6->iUS_UnloadSupport1In && !m_pDX6->iUS_UnloadSupport1Out && m_pDX6->iUS_UnloadSupport2In && !m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 300)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload2Case = 430;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 430:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 1)) {
			gData.nGoodTrayCount++;
			if (m_pEquipData->bUseInlineMode) {
				gData.bCapTrayLoad = TRUE;
				g_objCapAttachUDP.Set_TrayLoad(gData.nPortNo);
				//g_objCapAttachUDP.Set_TrayLoad(1);
// 				if (m_nUnloadLotIndex > 0) {
// 					if (gData.bContinueLotEnd[m_nUnloadLotIndex-1] == TRUE) {
// 						g_objCapAttachUDP.Set_TrayEnd(gData.nPortNo);
// 						Sleep(30);
// 						g_objCapAttachUDP.Set_LotEnd(gData.nPortNo);
// 					}
// 				}
			}
			m_nUnload2Case = 440;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;

	case 440:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y1, 2) || m_nUnload1Case >= 300) {
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Y2, 0);
			m_nUnload2Case = 450;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 450:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Y2, 0)) {
			pStatus = m_pAJinAXL->Get_pStatus(AX_UNLOAD_TRAY_Y1);
			dPosY1 = pStatus->dPos;
			double dPos = 0.0;
			if (gData.bUnloadSortFront) dPos = m_pMoveData->dUnloadTrayY1[2];
			else						dPos = m_pMoveData->dUnloadTrayY1[2] - (gData.nArrayW * gData.dTrayPitchW);

			if (m_nUnload1Case > 350 && dPosY1 <= dPos)
			{
				m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 0);
				m_nUnload2Case = 460;
				m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
			
			} else { return TRUE; }
		}
		break;
	case 460:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 0)) {
			m_nUnload2Case = 100;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;

	case 486:
#ifdef TRAY_CHECK2
		if (m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2)
#else
		if (m_pDX6->iUS_Z2Check1)
#endif
		{
			m_nUnload2Case = 487;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		} else {
			m_nUnload2Case = 440;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 487:
			m_pDY6->oUS_Z2AlignS12In = FALSE;
			m_pDY6->oUS_Z2AlignS12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 488;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		break;
	case 488:
		if (!m_pDX6->iUS_Z2AlignS12In && m_pDX6->iUS_Z2AlignS12Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_pDY6->oUS_Z2AlignM34In = FALSE;
			m_pDY6->oUS_Z2AlignM34Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 489;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 489:
		if (!m_pDX6->iUS_Z2AlignM34In && m_pDX6->iUS_Z2AlignM34Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 500)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 3);
			m_nUnload2Case = 490;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 490:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 3)) {
			m_pDY6->oUS_UnloadSupport12In = FALSE;
			m_pDY6->oUS_UnloadSupport12Out = TRUE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 491;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 491:
		if (!m_pDX6->iUS_UnloadSupport1In && m_pDX6->iUS_UnloadSupport1Out && !m_pDX6->iUS_UnloadSupport2In && m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 2);
			m_nUnload2Case = 492;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 492:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 2)) {
			m_pDY6->oUS_UnloadSupport12In = TRUE;
			m_pDY6->oUS_UnloadSupport12Out = FALSE;
			m_pAJinAXL->Write_Output(6);
			m_nUnload2Case = 493;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	case 493:
		if (m_pDX6->iUS_UnloadSupport1In && !m_pDX6->iUS_UnloadSupport1Out && m_pDX6->iUS_UnloadSupport2In && !m_pDX6->iUS_UnloadSupport2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_UNLOAD2, 1000)) break;
			m_pCommon->Move_Position(AX_UNLOAD_TRAY_Z2, 1);
			m_nUnload2Case = 494;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 30000);
		}
		break;
	case 494:
		if (m_pCommon->Check_Position(AX_UNLOAD_TRAY_Z2, 1)) {
			m_nUnload2Case = 440;
			m_pCommon->Set_LoopTime(AUTO_UNLOAD2, 5000);
		}
		break;
	}

	// 4.  (Error : 3600)
	if (m_pCommon->Over_LoopTime(AUTO_UNLOAD2)) {
		if (m_nUnload2Case == 100 || m_nUnload2Case == 250 || m_nUnload2Case == 300 || m_nUnload2Case == 310 || m_nUnload2Case == 440) return TRUE;

		m_pCommon->Show_Error(3500 + m_nUnload2Case);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 5.  (Error : 4100)
BOOL CSequenceMain::Inspect_Run()
{
	switch (m_nInspectCase) {
	case 100:
		if (m_nIndexTCase <= 200 && gData.IndexJob[1] == 0) {
			m_nInspectCase = 110;
			
			pLogFile->Save_MCCLog("Inspect",100, "Index Status Checked");
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
		}
		break;

	case 110:
		if (m_nIndexTCase <= 200 && gData.IndexJob[1] == 0) {
			if (Check_IndexEmpty(1) == TRUE || (gData.bCleanOutMode == TRUE)) {
				gData.IndexJob[1] = 1;
				m_nInspectCase = 100;
				pLogFile->Save_MCCLog("Inspect",110, "Make IndexJob Done");
			} else {
				m_pCommon->Move_Position(AX_INSPECTION_A, 0);
				if (gData.bUseCMPress) {
					m_pCommon->Move_Position(AX_INSPECTION_Z, 1);
					m_nInspectCase = 125;
					pLogFile->Save_MCCLog("Inspect",110, "Cam Move To Press Position");
				} else {
					m_pCommon->Move_Position(AX_INSPECTION_Z, 0);
					m_nInspectCase = 120;
					pLogFile->Save_MCCLog("Inspect",110, "Cam Move To Work Position for Scan");
				}
			}
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 30000);
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_INSPECTION_A, 0) && m_pCommon->Check_Position(AX_INSPECTION_Z, 0)) {
			if (!m_pEquipData->bUseVisionInspect) {m_nInspectCase = 200;}
			else							
			{	//if( gData.bUse100ScanRun )  m_nInspectCase = 131; else 
				m_nInspectCase = 130;
			}
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",120, "Wait Inspection Done or Skip Inspection");
		}
		break;
	case 125:
		if (m_pCommon->Check_Position(AX_INSPECTION_A, 0) && m_pCommon->Check_Position(AX_INSPECTION_Z, 1)) {
			if (!m_pEquipData->bUseVisionInspect) m_nInspectCase = 200;
			else								  m_nInspectCase = 126;
			
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",125, "125");
			
		}
		break;
	case 126:
		if (m_pCommon->Check_Position(AX_INSPECTION_A, 0) && m_pCommon->Check_Position(AX_INSPECTION_Z, 1)) {
			m_pDY2->oCMPressUp = FALSE;
			m_pDY2->oCMPressDn = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nInspectCase = 127;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",126, "126");
		}
		break;
	case 127:
		if (!m_pDX2->iCMPressUp && m_pDX2->iCMPressDn) {
			m_pCommon->Move_Position(AX_INSPECTION_Z, 0);
			m_nInspectCase = 128;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",126, "126");
		}
		break;
	case 128:
		if (m_pCommon->Check_Position(AX_INSPECTION_Z, 0))
		{
			//if( gData.bUse100ScanRun )  m_nInspectCase = 131; else
			m_nInspectCase = 130;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",128, "128");
		}
		break;
	case 131:
		m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
		break;

	case 130:	//검사완료까지 대기, NG 포지션으로 넘어간 모듈들이 검사가 완료될때 까지 대기한다. 
		if (Check_IndexEmpty(2) == TRUE || Check_InspecDone()) 
		{
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_StatusUpdate(INSPECTOR_VISION, 2);
			pInspector->Set_LoadComplete(INSPECTOR_VISION, gData.IDXTrayNo[1][0], gData.IDXLineNo[1]);
			m_nInspectCase = 150;
			pLogFile->Save_MCCLog("Inspect",130, "Load Complete");
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 30000);	// KCS(2024.07.08) 무언정지 수정 (5000 -> 30000)
		}
		break;
	case 150:	// Wait
		if (!m_pEquipData->bUseVisionInspect) // KCS(2024.07.09) 무언정지 수정
		{		
			m_nInspectCase = 200;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",150, "150");
		}
		break;
	case 160:
			m_pAJinAXL->Move_Abs_Accel(AX_INSPECTION_A, m_dVisionPosA, m_pMoveData->dDInspectionA[0]);
			m_pCommon->Move_Position(AX_INSPECTION_Z, 0);
			m_nInspectCase = 170;
			pLogFile->Save_MCCLog("Inspect",160, "A axis Move");
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
		break;
	case 170:
		if (m_pAJinAXL->Is_MoveDone(AX_INSPECTION_A, m_dVisionPosA) && m_pCommon->Check_Position(AX_INSPECTION_Z, 0)) {
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_MoveComplete(INSPECTOR_VISION, "I");
			m_nInspectCase = 150;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
			pLogFile->Save_MCCLog("Inspect",170, "Set Move Complete");
		}
		break;

	case 180:
		if (m_pAJinAXL->Is_Done(AX_INSPECTION_Z)) {
			m_pAJinAXL->Move_Abs_Accel(AX_INSPECTION_Z, m_dVisionPosZ, m_pMoveData->dDInspectionZ[0]);
			m_nInspectCase = 190;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
			pLogFile->Save_MCCLog("Inspect",180, "Z Axis Move to get Focus");
		}
		break;
	case 190:
		if (m_pAJinAXL->Is_MoveDone(AX_INSPECTION_Z, m_dVisionPosZ)) {
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_MoveComplete(INSPECTOR_VISION, "I");
			m_nInspectCase = 150;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
			pLogFile->Save_MCCLog("Inspect",190, "Z Axis Set Move Complete");
		}
		break;

	case 200:
			if (!m_pEquipData->bUseVisionInspect) 
			{	//gjc
				for(int i=0; i<gData.nPickCnt; i++) 
				{
					if (gData.IndexInfo[1][i] > 0) 
					{
						int nRand = m_pCommon->Get_Random(0, 99);
						int nJudge = (nRand < gData.nNGPercent ? 2 : 1);
						if (nJudge == 2)
						{
							nRand = m_pCommon->Get_Random(0, 99);
							nJudge = (nRand < 50 ? 13 : 2);	// 9:Barrel, 13:Fiducial, 14:Shiny edge
							gData.IndexInfo[1][i] = nJudge;
						}
						else
						{
							gData.IndexInfo[1][i] = 1;	//Good
						}
					}
				}
			}

			if (gData.bUse100ScanRun &&
				(!gData.bDone100ScanMotions[0] || !gData.bDone100ScanMotions[1]
				 || !gData.bDone100ScanMotions[2]|| !gData.bDone100ScanMotions[3])
			)
			{
				return TRUE;
	        }


			m_pCommon->Move_Position(AX_INSPECTION_A, 0);
			if (gData.bUseCMPress) 
			{
				m_pCommon->Move_Position(AX_INSPECTION_Z, 1);
				m_nInspectCase = 220;
				pLogFile->Save_MCCLog("Inspect",200, "200");
			} 
			else 
			{
				m_pCommon->Move_Position(AX_INSPECTION_Z, 0);
				gData.IndexJob[1] = 1;
				m_nInspectCase = 210;
				pLogFile->Save_MCCLog("Inspect",200, "Z Axis Back to Ready and make jobDone_Flag(Inspection Flag) True");
			}
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
		break;

	case 210:
		if (m_pCommon->Check_Position(AX_INSPECTION_A, 0) && m_pCommon->Check_Position(AX_INSPECTION_Z, 0)) {
			m_nInspectCase = 100;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
			pLogFile->Save_MCCLog("Inspect",210, "Inspection Done");
		}
		break;
	case 220:
		if (m_pCommon->Check_Position(AX_INSPECTION_Z, 1)) {
			m_pDY2->oCMPressUp = TRUE;
			m_pDY2->oCMPressDn = FALSE;
			m_pAJinAXL->Write_Output(2);
			m_nInspectCase = 221;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 10000);
			pLogFile->Save_MCCLog("Inspect",220, "FPCB Press");
		}
		break;
	case 221:
		if (m_pDX2->iCMPressUp && !m_pDX2->iCMPressDn) {
			gData.IndexJob[1] = 1;
			m_nInspectCase = 100;
			m_pCommon->Set_LoopTime(AUTO_INSPECT, 5000);
			pLogFile->Save_MCCLog("Inspect",221, "221");
		}
		break;

	}

	// 5.  (Error : 4100)
	if (m_pCommon->Over_LoopTime(AUTO_INSPECT)) {
		if (m_nInspectCase == 100 || m_nInspectCase == 130) return TRUE;

		m_pCommon->Show_Error(4000 + m_nInspectCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 5.  (Error : 4300)
BOOL CSequenceMain::Barcode_Run()
{
	switch (m_nBarcodeCase) {
	case 100:
		if (m_nIndexTCase <= 200 && gData.IndexJob[2] == 0) {
			m_nBarcodeCase = 110;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 5000);
		}
		break;

	case 110:
		if (m_nIndexTCase <= 200 && gData.IndexJob[2] == 0) {
			if (Check_IndexEmpty(1) == TRUE || (gData.bCleanOutMode == TRUE)) {
				gData.IndexJob[2] = 1;
				m_nBarcodeCase = 100;
			} else {
				m_pCommon->Move_Position(AX_BARCODE_A, 0);
				m_nBarcodeCase = 120;
			}
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 30000);
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_BARCODE_A, 0)) {
			if (!m_pEquipData->bUseVisionInspect) 
			{
				m_nBarcodeCase = 200;
			} 
			else
			{
				if(gData.bUse100ScanRun) m_nBarcodeCase = 130;
				else					 m_nBarcodeCase = 150;
			}
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 30000);	// KCS(2024.07.08) 무언정지 수정 (5000 -> 30000)
		}
		break;
	case 130:
		m_pCommon->Set_LoopTime(AUTO_BARCODE, 30000);
		break;

	case 150:	// Wait
		if (!m_pEquipData->bUseVisionInspect) {		// KCS(2024.07.09) 무언정지 수정
			m_nBarcodeCase = 200;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 5000);
		}
		break;
	case 160://
//			m_pAJinAXL->Move_Abs_Accel(AX_BARCODE_A, m_dBarcodePosA, m_pMoveData->dDBarCodeA[0]);
			m_pAJinAXL->Move_Abs_Accel(AX_BARCODE_A, m_dBarcodePosA, m_pMoveData->dDBarCodeA[0]);
			m_nBarcodeCase = 170;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 10000);
		break;
	case 170:
		if (m_pAJinAXL->Is_MoveDone(AX_BARCODE_A, m_dBarcodePosA)) {
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_MoveComplete(INSPECTOR_VISION, "B");
			m_nBarcodeCase = 150;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 10000);
		}
		break;

	case 200:
			gData.IndexJob[2] = 1;
			m_pCommon->Move_Position(AX_BARCODE_A, 0);
			m_nBarcodeCase = 210;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 10000);
		break;
	case 210:
		if (m_pCommon->Check_Position(AX_BARCODE_A, 0)) {
			m_nBarcodeCase = 100;
			m_pCommon->Set_LoopTime(AUTO_BARCODE, 10000);
		}
		break;
	}


	// 5.  (Error : 4300)
	if (m_pCommon->Over_LoopTime(AUTO_BARCODE)) {
		if (m_nBarcodeCase == 100 || m_nBarcodeCase == 130) return TRUE;

		m_pCommon->Show_Error(4200 + m_nBarcodeCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 6.  (Error : 5100)
BOOL CSequenceMain::NGPicker_Run()
{
	switch (m_nNGPickerCase) {
	case 100:
		if (m_nIndexTCase <= 200 && gData.IndexJob[3] == 0) {
			m_nNGPickerCase = 101;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 30000);	// KCS(2024.07.09) 무언정지 개선

			pLogFile->Save_MCCLog("NG Picker",100, "NG Picker Start");
		}
		else if( gData.bUse100ScanRun && m_nInspectCase == 130 )
		{
			gData.bDone100ScanMotions[0] = FALSE;
			m_nNGPickerCase = 102;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		return TRUE;
	case 102:
		m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[1]);
		m_nNGPickerCase = 103; m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		break;
	case 103:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX)) 
		{
			m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[0]);
			m_nNGPickerCase = 104; m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 104:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX)) 
		{
			if(m_nInspectCase == 200)
			{
				gData.bDone100ScanMotions[0] = TRUE;
				m_nNGPickerCase = 100;m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
			}
			else
			{
				m_nNGPickerCase = 102; m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
			}			
		}
		break;

	case 101:	// Check Inspect End
		if (gData.bCleanOutMode || Check_IndexEmpty(2)) 
		{
			gData.IndexJob[3] = 1;
			m_nNGPickerCase = 100;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);

			pLogFile->Save_MCCLog("NG Picker",101, "Make JobDone Flag True");
		} 
		else if (Check_InspecEnd(2)) 
		{	// KCS(2024.07.09) 무언정지 개선
			m_nNGPickerCase = 110;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);

			pLogFile->Save_MCCLog("NG Picker",101, "Inspection Done Checked of CM which have been scanned");
		}
		break;

	case 110:
		if (m_nIndexTCase <= 200 && gData.IndexJob[3] == 0) {
			if (Check_IndexNGJobExist() == FALSE) {
				m_pDY2->oInspCMAlign2In = TRUE;
				m_pDY2->oInspCMAlign2Out = FALSE;
				m_pAJinAXL->Write_Output(2);

				gData.IndexJob[3] = 1;
				m_nNGPickerCase = 100;

				pLogFile->Save_MCCLog("NG Picker",110, "There are No NG");
			} else {
				CString strLog, sIndx, sNGdx, sPicker;
				sIndx.Format("%1d%1d%1d%1d%1d%1d", gData.IndexInfo[2][0], gData.IndexInfo[2][1], gData.IndexInfo[2][2], gData.IndexInfo[2][3], gData.IndexInfo[2][4], gData.IndexInfo[2][5]);
				sPicker.Format("%1d%1d%1d%1d%1d", gData.PickerInfor[1][0], gData.PickerInfor[1][1], gData.PickerInfor[1][2], gData.PickerInfor[1][3], gData.PickerInfor[1][4]);
				
				strLog.Format("NG Picker 0 => I(%s) P(%s)", sIndx, sPicker);
				pLogFile->Save_HandlerLog(strLog);

				if (gData.bUseNGVacuum) {
					m_pDY5->oNGVacuumUp = TRUE;
					m_pDY5->oNGVacuumDown = FALSE;
					m_pAJinAXL->Write_Output(5);
				}
				m_nNGPickerCase = 120;

				pLogFile->Save_MCCLog("NG Picker",110, "NG Checked");
			}
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 120:
		if((m_pCommon->Check_Position(AX_NG_PICKER_Z, 0)) &&
		   (gData.bUseNGVacuum==FALSE || (m_pDX5->iNGVacuumUp && !m_pDX5->iNGVacuumDown)) ) {
			if (gData.bUseNGVacuum) {
				m_pDY5->oNGVacuumPad1On = TRUE;
				m_pDY5->oNGVacuumPad2On = TRUE;
				m_pDY5->oNGVacuumPad3On = TRUE;
				m_pDY5->oNGVacuumPad4On = TRUE;
				m_pDY5->oNGVacuumPad5On = TRUE;
				m_pDY5->oNGVacuumPad6On = TRUE;
				m_pAJinAXL->Write_Output(5);
				m_nNGPickerCase = 121;

				pLogFile->Save_MCCLog("NG Picker",120, "Vacuum On");
			} else {
				m_nNGPickerCase = 130;
			}
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 30000);
		}
		break;
	case 121:
//		if((gData.IndexInfo[2][0]==0 || m_pDX5->iNGVacuumPad1Check) &&
//		   (gData.IndexInfo[2][1]==0 || m_pDX5->iNGVacuumPad2Check)	&&
//		   (gData.IndexInfo[2][2]==0 || m_pDX5->iNGVacuumPad3Check) && 
//		   (gData.IndexInfo[2][3]==0 || m_pDX5->iNGVacuumPad4Check) &&
//		   (gData.IndexInfo[2][4]==0 || m_pDX5->iNGVacuumPad5Check) ) {
			m_nNGPickerCase = 130;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 30000);
//		}
		break;

	case 130:
		if (Check_NGPickerAllUp() == TRUE) {
			gData.NGPicNo = gData.NGIdxNo = 0;
			for(int i=0; i<10; i++) gData.NGJobPic[i]=gData.NGJobIdx[i]=0;
			if (m_pCommon->PickerNG_IdxMove()) {
				m_dNGPickerX = m_pCommon->m_dP3X;
				m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[0]);
				m_nNGPickerCase = 140;

				pLogFile->Save_MCCLog("NG Picker",130, "Picker X Move");
			} else {
				m_nNGPickerCase = 110;
				pLogFile->Save_MCCLog("NG Picker",130, "There is No NG");
			}
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 10000);
		}
		break;
	case 140:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX)) {
			m_pDY2->oInspCMAlign2In = FALSE;
			m_pDY2->oInspCMAlign2Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nNGPickerCase = 150;
			pLogFile->Save_MCCLog("NG Picker",140, "Inspection Aligner Out");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 150:
		if (!m_pDX2->iInspCMAlign2In && m_pDX2->iInspCMAlign2Out) {
			m_pCommon->Set_NGPickerDown(gData.NGPicNo, gData.NGIdxNo);
			m_pAJinAXL->Write_Output(3);
			m_nNGPickerCase = 160;
			pLogFile->Save_MCCLog("NG Picker",150, "NG Picker Down");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 160:
		if((gData.NGJobPic[0]==0 || (gData.NGJobPic[0]==1 && !m_pDX3->iNGPicker1Up && m_pDX3->iNGPicker1Down)) &&
		   (gData.NGJobPic[1]==0 || (gData.NGJobPic[1]==1 && !m_pDX3->iNGPicker2Up && m_pDX3->iNGPicker2Down)) &&
		   (gData.NGJobPic[2]==0 || (gData.NGJobPic[2]==1 && !m_pDX3->iNGPicker3Up && m_pDX3->iNGPicker3Down)) &&
		   (gData.NGJobPic[3]==0 || (gData.NGJobPic[3]==1 && !m_pDX3->iNGPicker4Up && m_pDX3->iNGPicker4Down)) &&
		   (gData.NGJobPic[4]==0 || (gData.NGJobPic[4]==1 && !m_pDX3->iNGPicker5Up && m_pDX3->iNGPicker5Down)) ) {
			m_pDY5->oNGVacuumPad1On = FALSE;
			m_pDY5->oNGVacuumPad2On = FALSE;
			m_pDY5->oNGVacuumPad3On = FALSE;
			m_pDY5->oNGVacuumPad4On = FALSE;
			m_pDY5->oNGVacuumPad5On = FALSE;
			m_pDY5->oNGVacuumPad6On = FALSE;
			m_pAJinAXL->Write_Output(5);

			m_nNGPickerCase = 161;
			pLogFile->Save_MCCLog("NG Picker",160, "NG Vac Off");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 161:
		if (!m_pDX2->iInspCMAlign2In && m_pDX2->iInspCMAlign2Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_NGPICKER, 200)) break;
			if (gData.NGJobPic[0]==1) { m_pDY3->oNGPicker1Open = FALSE; m_pDY3->oNGPicker1Close = TRUE; }
			if (gData.NGJobPic[1]==1) { m_pDY3->oNGPicker2Open = FALSE; m_pDY3->oNGPicker2Close = TRUE; }
			if (gData.NGJobPic[2]==1) { m_pDY3->oNGPicker3Open = FALSE; m_pDY3->oNGPicker3Close = TRUE; }
			if (gData.NGJobPic[3]==1) { m_pDY3->oNGPicker4Open = FALSE; m_pDY3->oNGPicker4Close = TRUE; }
			if (gData.NGJobPic[4]==1) { m_pDY3->oNGPicker5Open = FALSE; m_pDY3->oNGPicker5Close = TRUE; }
			m_pAJinAXL->Write_Output(3);
			m_nNGPickerCase = 162;
			pLogFile->Save_MCCLog("NG Picker",161, "NG Picker Grip");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 162:
		if( (gData.NGJobPic[0]==0 || (gData.NGJobPic[0]==1 && !m_pDX3->iNGPicker1Open)) &&
			(gData.NGJobPic[1]==0 || (gData.NGJobPic[1]==1 && !m_pDX3->iNGPicker2Open)) &&
			(gData.NGJobPic[2]==0 || (gData.NGJobPic[2]==1 && !m_pDX3->iNGPicker3Open)) &&
			(gData.NGJobPic[3]==0 || (gData.NGJobPic[3]==1 && !m_pDX3->iNGPicker4Open)) &&
			(gData.NGJobPic[4]==0 || (gData.NGJobPic[4]==1 && !m_pDX3->iNGPicker5Open)) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_NGPICKER, 200)) break;
			if (gData.NGJobPic[0]==1) { m_pDY3->oNGPicker1Up = TRUE; m_pDY3->oNGPicker1Down = FALSE; }
			if (gData.NGJobPic[1]==1) { m_pDY3->oNGPicker2Up = TRUE; m_pDY3->oNGPicker2Down = FALSE; }
			if (gData.NGJobPic[2]==1) { m_pDY3->oNGPicker3Up = TRUE; m_pDY3->oNGPicker3Down = FALSE; }
			if (gData.NGJobPic[3]==1) { m_pDY3->oNGPicker4Up = TRUE; m_pDY3->oNGPicker4Down = FALSE; }
			if (gData.NGJobPic[4]==1) { m_pDY3->oNGPicker5Up = TRUE; m_pDY3->oNGPicker5Down = FALSE; }
			m_pAJinAXL->Write_Output(3);

			if (gData.bUseNGVacuum) {
				m_pDY5->oNGVacuumUp = FALSE;
				m_pDY5->oNGVacuumDown = TRUE;
				m_pAJinAXL->Write_Output(5);
			}

			m_nNGPickerCase = 170;
			pLogFile->Save_MCCLog("NG Picker",162, "NG Picker Up");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 170:
		if((gData.NGJobPic[0]==0 || (gData.NGJobPic[0]==1 && m_pDX3->iNGPicker1Up && !m_pDX3->iNGPicker1Down)) &&
		   (gData.NGJobPic[1]==0 || (gData.NGJobPic[1]==1 && m_pDX3->iNGPicker2Up && !m_pDX3->iNGPicker2Down)) &&
		   (gData.NGJobPic[2]==0 || (gData.NGJobPic[2]==1 && m_pDX3->iNGPicker3Up && !m_pDX3->iNGPicker3Down)) &&
		   (gData.NGJobPic[3]==0 || (gData.NGJobPic[3]==1 && m_pDX3->iNGPicker4Up && !m_pDX3->iNGPicker4Down)) &&
		   (gData.NGJobPic[4]==0 || (gData.NGJobPic[4]==1 && m_pDX3->iNGPicker5Up && !m_pDX3->iNGPicker5Down)) ) {
			m_nNGPickerCase = 180;
			pLogFile->Save_MCCLog("NG Picker",170, "NG Picker Up Checked");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 180:
//		if((!m_pEquipData->bUseCMCheck) ||
		if((gData.bUseDryRun) ||
		   ((gData.NGJobPic[0]==0 || (gData.NGJobPic[0]==1 && m_pDX3->iNGPicker1CMCheck)) &&
		    (gData.NGJobPic[1]==0 || (gData.NGJobPic[1]==1 && m_pDX3->iNGPicker2CMCheck)) &&
		    (gData.NGJobPic[2]==0 || (gData.NGJobPic[2]==1 && m_pDX3->iNGPicker3CMCheck)) &&
		    (gData.NGJobPic[3]==0 || (gData.NGJobPic[3]==1 && m_pDX3->iNGPicker4CMCheck)) &&
		    (gData.NGJobPic[4]==0 || (gData.NGJobPic[4]==1 && m_pDX3->iNGPicker5CMCheck))) ) {
			m_pDY2->oInspCMAlign2In = TRUE;
			m_pDY2->oInspCMAlign2Out = FALSE;
			m_pAJinAXL->Write_Output(2);

			gData.PickerNGTrayNo = gData.IDXTrayNo[2][0];
			int cm = (gData.IDXLineNo[2] - 1) * gData.nPickCnt;
			for(int i=0; i<5; i++) {
				if (gData.NGJobPic[i]==1) {
					int nIndxNo = gData.NGJobIdx[i];
					gData.PickerInfor[1][i] = gData.IndexInfo[2][nIndxNo]; gData.IndexInfo[2][nIndxNo] = 0;
					gData.PickerNGPoNo[i]   = gData.IDXPoNo[2][nIndxNo];   gData.IDXPoNo[2][nIndxNo] = 0;
								
				}
			}
			m_nNGPickerCase = 190;
			pLogFile->Save_MCCLog("NG Picker",180, "Aligner In and Info Exchange");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 190:	// Index NG에 작업이 더 있는지 확인
		if (Check_NGPickerAllUp() == TRUE) {
			int nIndxCnt = 0, nPikCnt = 0;
			for(int i=0; i<gData.nPickCnt; i++) {
				if (gData.IndexInfo[2][i] >= 2) nIndxCnt++;
			}
			for(int i=0; i<5; i++) {
				if (gData.PickerInfor[1][i] == 0) nPikCnt++;
			}

			if (nIndxCnt > 0) gData.bNGNextjob = TRUE;
			else			  gData.bNGNextjob = FALSE;
			if (nIndxCnt > 0 && nPikCnt > 0) m_nNGPickerCase = 110;
			else							 m_nNGPickerCase = 200;

			pLogFile->Save_MCCLog("NG Picker",190, "NG Index Job Done Check");

			int nYY = gData.nTrayPos[gData.nNGTrayPos];
			CString strLog, sIndx, sNGdx, sPicker;
			sIndx.Format("%1d%1d%1d%1d%1d%1d", gData.IndexInfo[2][0], gData.IndexInfo[2][1], gData.IndexInfo[2][2], gData.IndexInfo[2][3], gData.IndexInfo[2][4], gData.IndexInfo[2][5]);
			sNGdx.Format("%1d%1d%1d%1d%1d%1d", gData.NG1TrayInfo[nYY-1][0], gData.NG1TrayInfo[nYY-1][1], gData.NG1TrayInfo[nYY-1][2], gData.NG1TrayInfo[nYY-1][3], gData.NG1TrayInfo[nYY-1][4], gData.NG1TrayInfo[nYY-1][5]);
			sPicker.Format("%1d%1d%1d%1d%1d", gData.PickerInfor[1][0], gData.PickerInfor[1][1], gData.PickerInfor[1][2], gData.PickerInfor[1][3], gData.PickerInfor[1][4]);
			
			strLog.Format("NG Picker 3 => I(%s) P(%s) NG(%s) Tray:%d Picker:%d XPos(%0.3lf) TraypOS(%d) Seq(%d)", sIndx, sPicker, sNGdx, gData.NGIdxNo, gData.NGPicNo, m_dNGPickerX, gData.nNGTrayPos, m_nNGPickerCase);
			pLogFile->Save_HandlerLog(strLog);

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 200:
		if (Check_NGPickerAllUp() == TRUE) {
			if (gData.bUseNGVacuum==FALSE || (!m_pDX5->iNGVacuumUp && m_pDX5->iNGVacuumDown)) {
				if (gData.bNGNextjob == FALSE) gData.IndexJob[3] = 1;

				m_pCommon->Move_Position(AX_NG_PICKER_Z, 1);
				m_nNGPickerCase = 210;
				pLogFile->Save_MCCLog("NG Picker",200, "NG Picker Z Down to Tray Position");

				m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
			}
		}
		break;
	case 210:	// NG Tray 위치로 이동 확인.
		if (m_pCommon->Check_Position(AX_NG_PICKER_Z, 1) ) {
			m_pCommon->Move_Position(AX_NG_PICKER_X, 1);
			m_dNGPickerX = m_pMoveData->dNGPickerX[1];	// NG 연속으로 내려 놓을때는 X축 위치가 바뀌기 때문에...

			if (gData.bUseSeparationMode) m_nNGPickerCase = 221;
			else						  m_nNGPickerCase = 220;

			pLogFile->Save_MCCLog("NG Picker",210, "NG Picker Z Down Done");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 60000);
		}
		break;

	case 220:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX) && Check_NGPickerAllUp() == TRUE) {
			if (m_nNGTrayCase == 150 || m_nNGTrayCase == 250) {
				gData.NGPicNo = gData.NGIdxNo = 0;
				Check_NGPickDownSelect();
				if (gData.NGPicNo > 0 && gData.NGIdxNo > 0) {
					int nYY = gData.nTrayPos[gData.nNGTrayPos];
					m_pCommon->PickerNG_DnMove(gData.NGPicNo, gData.NGIdxNo, nYY, gData.nNGTrayPos);
					m_dNGPickerX = m_pCommon->m_dP3X;

					m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[1]);
					m_pCommon->Move_Position(AX_NG_PICKER_Z, 1);

					if (gData.bNgSortFront) {
						if (m_nNGTrayCase == 150) m_dNGStageY = m_pMoveData->dNGStageY[1] + ((gData.nTrayPos[1]-1) * (gData.dTrayPitchW + m_pMoveData->dNGStageY[3]));
						if (m_nNGTrayCase == 250) m_dNGStageY = m_pMoveData->dNGStageY[2] + ((gData.nTrayPos[2]-1) * (gData.dTrayPitchW + m_pMoveData->dNGStageY[3]));

					} else {
						if (m_nNGTrayCase == 150) m_dNGStageY = m_pMoveData->dNGStageY[1] - ((gData.nTrayPos[1]-1) * (gData.dTrayPitchW + m_pMoveData->dNGStageY[3]));
						if (m_nNGTrayCase == 250) m_dNGStageY = m_pMoveData->dNGStageY[2] - ((gData.nTrayPos[2]-1) * (gData.dTrayPitchW + m_pMoveData->dNGStageY[3]));
					}					

					m_nNGPickerCase = 230;
					pLogFile->Save_MCCLog("NG Picker",220, "NG Picker X and NG Tray Move");

					m_pCommon->Set_LoopTime(AUTO_NGPICKER, 10000);
				} else {
					if (Check_NGTrayMoving()==TRUE) {
						if (m_nNGTrayCase == 150) m_nNGTrayCase = 160;
						if (m_nNGTrayCase == 250) m_nNGTrayCase = 260;
					}
				}
			}
		}
		break;

	case 221:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX) && Check_NGPickerAllUp() == TRUE) {
			if (m_nNGTrayCase == 140) {
				gData.NGPicNo = gData.NGIdxNo = 0;
				// NG Tray 1번 확인
				if (Check_NGTray1PickDownSelect())
				{
					gData.nNGTrayPos = 1;
					int nYY = gData.nTrayPos[gData.nNGTrayPos];
					m_pCommon->PickerNG_DnMove(gData.NGPicNo, gData.NGIdxNo, nYY, gData.nNGTrayPos);
					m_dNGPickerX = m_pCommon->m_dP3X;
					m_dNGPickerZ = m_pCommon->m_dP3Z;
					m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[2]);
					m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_Z, m_dNGPickerZ, m_pMoveData->dDNGPickerZ[2]);

					m_pCommon->PickerNGStageMove(gData.nTrayPos[1], 1);
					m_dNGStageY = m_pCommon->m_dP3Y;
					m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

					m_nNGPickerCase = 230;
					pLogFile->Save_MCCLog("NG Picker",220, "NG Picker X and NG Tray Y Move");
					m_pCommon->Set_LoopTime(AUTO_NGPICKER, 10000);
				} 
				else
				{
					// NG Tray 라인 이동
					if (gData.NGPicNo > 0 && gData.NGIdxNo == 0) {
						gData.nNGTrayPos = 1;
						if (Check_NGTrayMoving()==TRUE) {
							if (m_nNGTrayCase == 140) m_nNGTrayCase = 141;
						}
					}
					// NG Tray 2번 확인
					if (gData.NGPicNo == 0) {
						gData.NGPicNo = gData.NGIdxNo = 0;
						if (Check_NGTray2PickDownSelect()){
							gData.nNGTrayPos = 2;
							int nYY = gData.nTrayPos[gData.nNGTrayPos];
							m_pCommon->PickerNG_DnMove(gData.NGPicNo, gData.NGIdxNo, nYY, gData.nNGTrayPos);
							m_dNGPickerX = m_pCommon->m_dP3X;
							m_dNGPickerZ = m_pCommon->m_dP3Z;
							m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_X, m_dNGPickerX, m_pMoveData->dDNGPickerX[2]);
							m_pAJinAXL->Move_Abs_Accel(AX_NG_PICKER_Z, m_dNGPickerZ, m_pMoveData->dDNGPickerX[2]);

							m_pCommon->PickerNGStageMove(gData.nTrayPos[2], 2);
							m_dNGStageY = m_pCommon->m_dP3Y;
							m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

							m_nNGPickerCase = 230;

							pLogFile->Save_MCCLog("NG Picker",220, "NG Picker X and NG Tray Y Move");
							m_pCommon->Set_LoopTime(AUTO_NGPICKER, 10000);

						} else {
							// NG Tray 라인 이동
							if (gData.NGPicNo > 0 && gData.NGIdxNo == 0) {
								gData.nNGTrayPos = 2;
#ifdef MODEL_SC	// 라인별로 분리 배출을 하기 때문에 다음 라인으로 넘어가는걸 체크하지 않고 Full 체크를 해준다.
								if (Check_NGTrayFull(gData.nNGTrayPos)==TRUE) {
									gData.nTrayPos[gData.nNGTrayPos] = gData.nArrayW;
									if (m_nNGTrayCase == 140) m_nNGTrayCase = 143;
								}
#else
								
								if (Check_NGTrayMoving()==TRUE) {
									if (m_nNGTrayCase == 140) m_nNGTrayCase = 143;
								}
#endif
							}
						}						
					}
				}
			}
		}
		break;

	case 230:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_PICKER_X, m_dNGPickerX) &&
			m_pAJinAXL->Is_MoveDone(AX_NG_STAGE_Y, m_dNGStageY) &&
			m_pCommon->Check_Position(AX_NG_PICKER_Z, 1) ) {
			
			m_nNGPickerCase = 240;
			pLogFile->Save_MCCLog("NG Picker",230, "NG Picker X and NG Tray Y Move (Done)");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 240:
//		if((!m_pEquipData->bUseCMCheck) ||
		if((gData.bUseDryRun) ||
		   (gData.NGPicNo==1 && m_pDX3->iNGPicker1CMCheck) ||
		   (gData.NGPicNo==2 && m_pDX3->iNGPicker2CMCheck) ||
		   (gData.NGPicNo==3 && m_pDX3->iNGPicker3CMCheck) ||
		   (gData.NGPicNo==4 && m_pDX3->iNGPicker4CMCheck) ||
		   (gData.NGPicNo==5 && m_pDX3->iNGPicker5CMCheck) ) {
			if (gData.NGPicNo==1) { m_pDY3->oNGPicker1Up = FALSE; m_pDY3->oNGPicker1Down = TRUE; }
			if (gData.NGPicNo==2) { m_pDY3->oNGPicker2Up = FALSE; m_pDY3->oNGPicker2Down = TRUE; }
			if (gData.NGPicNo==3) { m_pDY3->oNGPicker3Up = FALSE; m_pDY3->oNGPicker3Down = TRUE; }
			if (gData.NGPicNo==4) { m_pDY3->oNGPicker4Up = FALSE; m_pDY3->oNGPicker4Down = TRUE; }
			if (gData.NGPicNo==5) { m_pDY3->oNGPicker5Up = FALSE; m_pDY3->oNGPicker5Down = TRUE; }
			m_pAJinAXL->Write_Output(3);




			m_nNGPickerCase = 250;
			pLogFile->Save_MCCLog("NG Picker",240, "NG Picker Down");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 250:
		if((gData.NGPicNo==1 && !m_pDX3->iNGPicker1Up && m_pDX3->iNGPicker1Down) ||
		   (gData.NGPicNo==2 && !m_pDX3->iNGPicker2Up && m_pDX3->iNGPicker2Down) ||
		   (gData.NGPicNo==3 && !m_pDX3->iNGPicker3Up && m_pDX3->iNGPicker3Down) ||
		   (gData.NGPicNo==4 && !m_pDX3->iNGPicker4Up && m_pDX3->iNGPicker4Down) ||
		   (gData.NGPicNo==5 && !m_pDX3->iNGPicker5Up && m_pDX3->iNGPicker5Down) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_NGPICKER, 100)) break;
			if (gData.NGPicNo==1) { m_pDY3->oNGPicker1Open = TRUE; m_pDY3->oNGPicker1Close = FALSE; }
			if (gData.NGPicNo==2) { m_pDY3->oNGPicker2Open = TRUE; m_pDY3->oNGPicker2Close = FALSE; }
			if (gData.NGPicNo==3) { m_pDY3->oNGPicker3Open = TRUE; m_pDY3->oNGPicker3Close = FALSE; }
			if (gData.NGPicNo==4) { m_pDY3->oNGPicker4Open = TRUE; m_pDY3->oNGPicker4Close = FALSE; }
			if (gData.NGPicNo==5) { m_pDY3->oNGPicker5Open = TRUE; m_pDY3->oNGPicker5Close = FALSE; }
			m_pAJinAXL->Write_Output(3);
			m_nNGPickerCase = 260;
			pLogFile->Save_MCCLog("NG Picker",250, "NG Picker Ungrip");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 260:
		if((gData.NGPicNo==1 && m_pDX3->iNGPicker1Open) ||
		   (gData.NGPicNo==2 && m_pDX3->iNGPicker2Open) ||
		   (gData.NGPicNo==3 && m_pDX3->iNGPicker3Open) ||
		   (gData.NGPicNo==4 && m_pDX3->iNGPicker4Open) ||
		   (gData.NGPicNo==5 && m_pDX3->iNGPicker5Open) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_NGPICKER, 100)) break;
			if (gData.NGPicNo==1) { m_pDY3->oNGPicker1Up = TRUE; m_pDY3->oNGPicker1Down = FALSE; }
			if (gData.NGPicNo==2) { m_pDY3->oNGPicker2Up = TRUE; m_pDY3->oNGPicker2Down = FALSE; }
			if (gData.NGPicNo==3) { m_pDY3->oNGPicker3Up = TRUE; m_pDY3->oNGPicker3Down = FALSE; }
			if (gData.NGPicNo==4) { m_pDY3->oNGPicker4Up = TRUE; m_pDY3->oNGPicker4Down = FALSE; }
			if (gData.NGPicNo==5) { m_pDY3->oNGPicker5Up = TRUE; m_pDY3->oNGPicker5Down = FALSE; }
			m_pAJinAXL->Write_Output(3);
			m_nNGPickerCase = 270;
			pLogFile->Save_MCCLog("NG Picker",260, "NG Picker Up");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 270:
		if((gData.NGPicNo==1 && m_pDX3->iNGPicker1Up && !m_pDX3->iNGPicker1Down) ||
		   (gData.NGPicNo==2 && m_pDX3->iNGPicker2Up && !m_pDX3->iNGPicker2Down) ||
		   (gData.NGPicNo==3 && m_pDX3->iNGPicker3Up && !m_pDX3->iNGPicker3Down) ||
		   (gData.NGPicNo==4 && m_pDX3->iNGPicker4Up && !m_pDX3->iNGPicker4Down) ||
		   (gData.NGPicNo==5 && m_pDX3->iNGPicker5Up && !m_pDX3->iNGPicker5Down) ) {

			int nTNo = (gData.PickerNGTrayNo > 0 ? (gData.PickerNGTrayNo - 1) : 0);
			int nPno = (gData.PickerNGPoNo[gData.NGPicNo-1] > 0 ? (gData.PickerNGPoNo[gData.NGPicNo-1] - 1) : 0);
			int nYY = gData.nTrayPos[gData.nNGTrayPos];
			int nNGPos = (gData.nPickCnt * (nYY-1)) + gData.NGIdxNo;

			if (gData.nNGTrayPos == 1) {	
				gData.NG1TrayInfo[nYY-1][gData.NGIdxNo-1] = gData.PickerInfor[1][gData.NGPicNo-1];
			} else {
				gData.NG2TrayInfo[nYY-1][gData.NGIdxNo-1] = gData.PickerInfor[1][gData.NGPicNo-1];
			}

			if (gData.PickerInfor[1][gData.NGPicNo-1]== 2 || gData.PickerInfor[1][gData.NGPicNo-1]== 3 ||
				gData.PickerInfor[1][gData.NGPicNo-1]== 7 || gData.PickerInfor[1][gData.NGPicNo-1]== 8 ||
				gData.PickerInfor[1][gData.NGPicNo-1]==11 || gData.PickerInfor[1][gData.NGPicNo-1]==12 ||
				gData.PickerInfor[1][gData.NGPicNo-1]==13 || gData.PickerInfor[1][gData.NGPicNo-1]==14)	gLot.nNGCnt++;
			if (gData.PickerInfor[1][gData.NGPicNo-1]==13) { gLot.nLfNGCnt++; }
			if (gData.PickerInfor[1][gData.NGPicNo-1]==14) { gLot.nSeNGCnt++; }
			if (gData.PickerInfor[1][gData.NGPicNo-1]==8) { gLot.nSideNgCnt++; }

			if (gData.PickerInfor[1][gData.NGPicNo-1]==3) {
				gLot.nBsNGCnt++;
				gData.nNGPos = nNGPos;
				CWorkDlg *pWorkDlg = CWorkDlg::Get_Instance();
				pWorkDlg->SendMessage(UM_UPDATE_SHIFT_LIST, (WPARAM)OP_INSERT, (LPARAM)NULL);
			}
			// NG Picker Jig 정보 저장 
			if (gData.PickerInfor[1][gData.NGPicNo-1] >= 2) {
				gLot.nCmJigNo[gData.PickerNGTrayNo-1][gData.nNGTrayCmNo-1][4] = gData.NGPicNo;
			}	

			gData.PickerInfor[1][gData.NGPicNo-1] = 0;
			//g_objMES.Set_Result(gLot.sLotID, gLot.sBarLoad[nTNo][nPno], "NG", gLot.sNGCode[nTNo][nPno], gLot.sNGText[nTNo][nPno], nTNo+1, nPno+1, 0,0, gData.nNGTrayPos, nNGPos);
			
			pLogFile->Save_CmTrackingLog("NG", gData.nNGTrayPos, gData.NGIdxNo, nYY, gData.PickerNGTrayNo, gData.PickerNGPoNo[gData.NGPicNo-1]);

			if(gLot.sNGCode[nTNo][nPno] == "16")
			{
				g_objMES.Save_ProcessedData(gLot.sLotID, gLot.sBarLoad[nTNo][nPno], "NG", gLot.sNGCode[nTNo][nPno], gLot.sNGText[nTNo][nPno], nTNo+1, nPno+1, 0,0, gData.nNGTrayPos, nNGPos);
			}
			
			CString strLog, sIndx, sNGdx, sPicker;
			sIndx.Format("%1d%1d%1d%1d%1d%1d", gData.IndexInfo[2][0], gData.IndexInfo[2][1], gData.IndexInfo[2][2], gData.IndexInfo[2][3], gData.IndexInfo[2][4], gData.IndexInfo[2][5]);
			sNGdx.Format("%1d%1d%1d%1d%1d%1d", gData.NG1TrayInfo[nYY-1][0], gData.NG1TrayInfo[nYY-1][1], gData.NG1TrayInfo[nYY-1][2], gData.NG1TrayInfo[nYY-1][3], gData.NG1TrayInfo[nYY-1][4], gData.NG1TrayInfo[nYY-1][5]);
			sPicker.Format("%1d%1d%1d%1d%1d", gData.PickerInfor[1][0], gData.PickerInfor[1][1], gData.PickerInfor[1][2], gData.PickerInfor[1][3], gData.PickerInfor[1][4]);

			strLog.Format("NG Picker 4 => I(%s) P(%s) NG(%s) Tray:%d Picker:%d XPos(%0.3lf) TraypOS(%d)", sIndx, sPicker, sNGdx, gData.NGIdxNo, gData.NGPicNo, m_dNGPickerX, gData.nNGTrayPos);
			pLogFile->Save_HandlerLog(strLog);

			m_nNGPickerCase = 280;
			pLogFile->Save_MCCLog("NG Picker",260, "NG Picker Up Check and Info Exchange");

			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;

	case 280:
		if (Check_NGTrayMoving()==TRUE) {
			if (m_nNGTrayCase == 150) m_nNGTrayCase = 160;
			if (m_nNGTrayCase == 250) m_nNGTrayCase = 260;
		}
			m_nNGPickerCase = 290;
			pLogFile->Save_MCCLog("NG Picker",280, "Make NgTray Move");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		break;

	case 290:
		if (gData.PickerInfor[1][0] > 0 || gData.PickerInfor[1][1] > 0 || gData.PickerInfor[1][2] > 0 || gData.PickerInfor[1][3] > 0 || gData.PickerInfor[1][4] > 0) {
			if (gData.bUseSeparationMode) m_nNGPickerCase = 221;
			else						  m_nNGPickerCase = 220;
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 90000);
		} else {
			if (Check_NGPickerAllUp() == TRUE) {
				m_pCommon->Move_Position(AX_NG_PICKER_X, 0);
				m_nNGPickerCase = 300;
				pLogFile->Save_MCCLog("NG Picker",290, "NG Picker X Back to Ready");
				m_pCommon->Set_LoopTime(AUTO_NGPICKER, 30000);
			}
		}
		break;
	case 300:
		if (m_pCommon->Check_Position(AX_NG_PICKER_X, 0) /*&& m_pCommon->Check_Position(AX_NG_PICKER_Z, 0)*/ ) {
			m_pCommon->Move_Position(AX_NG_PICKER_Z, 0);
			m_nNGPickerCase = 301;

			pLogFile->Save_MCCLog("NG Picker",300, "NG Picker Z Back to Ready");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
		}
		break;
	case 301:
		if (gData.bNGNextjob) {
			m_nNGPickerCase = 110;
			pLogFile->Save_MCCLog("NG Picker",301, "NG Picker still have things more to do");
			m_pCommon->Set_LoopTime(AUTO_NGPICKER, 30000);
		} else {
			if((gData.bUseDryRun) ||
			   (m_pCommon->Check_Position(AX_NG_PICKER_Z, 0) &&
			   !m_pDX3->iNGPicker1CMCheck && !m_pDX3->iNGPicker2CMCheck && !m_pDX3->iNGPicker3CMCheck && !m_pDX3->iNGPicker4CMCheck && !m_pDX3->iNGPicker5CMCheck) )
			{
				m_nNGPickerCase = 100;
				pLogFile->Save_MCCLog("NG Picker",301, "NG Picker Job Done");
				m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
			}
		}
		break;

	}

	// 6.  (Error : 5100)
	if (m_pCommon->Over_LoopTime(AUTO_NGPICKER)) {
		if (m_nNGPickerCase == 150) {
			if(gData.NGJobPic[0]==1 && (m_pDX3->iNGPicker1Up || !m_pDX3->iNGPicker1Down)) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && (m_pDX3->iNGPicker2Up || !m_pDX3->iNGPicker2Down)) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && (m_pDX3->iNGPicker3Up || !m_pDX3->iNGPicker3Down)) gData.NGPicNo = gData.NGIdxNo=3;
			if(gData.NGJobPic[3]==1 && (m_pDX3->iNGPicker4Up || !m_pDX3->iNGPicker4Down)) gData.NGPicNo = gData.NGIdxNo=4;
			if(gData.NGJobPic[4]==1 && (m_pDX3->iNGPicker5Up || !m_pDX3->iNGPicker5Down)) gData.NGPicNo = gData.NGIdxNo=5;
		}
		if (m_nNGPickerCase == 162) {
			if(gData.NGJobPic[0]==1 && m_pDX3->iNGPicker1Open) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && m_pDX3->iNGPicker2Open) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && m_pDX3->iNGPicker3Open) gData.NGPicNo = gData.NGIdxNo=3;
			if(gData.NGJobPic[3]==1 && m_pDX3->iNGPicker4Open) gData.NGPicNo = gData.NGIdxNo=4;
			if(gData.NGJobPic[4]==1 && m_pDX3->iNGPicker5Open) gData.NGPicNo = gData.NGIdxNo=5;
		}
		if (m_nNGPickerCase == 170) {
			if(gData.NGJobPic[0]==1 && (!m_pDX3->iNGPicker1Up || m_pDX3->iNGPicker1Down)) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && (!m_pDX3->iNGPicker2Up || m_pDX3->iNGPicker2Down)) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && (!m_pDX3->iNGPicker3Up || m_pDX3->iNGPicker3Down)) gData.NGPicNo = gData.NGIdxNo=3;
			if(gData.NGJobPic[3]==1 && (!m_pDX3->iNGPicker4Up || m_pDX3->iNGPicker4Down)) gData.NGPicNo = gData.NGIdxNo=4;
			if(gData.NGJobPic[4]==1 && (!m_pDX3->iNGPicker5Up || m_pDX3->iNGPicker5Down)) gData.NGPicNo = gData.NGIdxNo=5;
		}
		if (m_nNGPickerCase == 180) {
			if(gData.NGJobPic[0]==1 && !m_pDX3->iNGPicker1CMCheck) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && !m_pDX3->iNGPicker2CMCheck) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && !m_pDX3->iNGPicker3CMCheck) gData.NGPicNo = gData.NGIdxNo=3;
			if(gData.NGJobPic[3]==1 && !m_pDX3->iNGPicker4CMCheck) gData.NGPicNo = gData.NGIdxNo=4;
			if(gData.NGJobPic[4]==1 && !m_pDX3->iNGPicker5CMCheck) gData.NGPicNo = gData.NGIdxNo=5;
		}

		if (m_nNGPickerCase == 151) {
			if(gData.NGJobPic[0]==1 && (m_pDX3->iNGPicker1Up || !m_pDX3->iNGPicker1Down)) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && (m_pDX3->iNGPicker2Up || !m_pDX3->iNGPicker2Down)) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && (m_pDX3->iNGPicker3Up || !m_pDX3->iNGPicker3Down)) gData.NGPicNo = gData.NGIdxNo=3;
		}
		if (m_nNGPickerCase == 166) {
			if(gData.NGJobPic[0]==1 && m_pDX3->iNGPicker1Open) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && m_pDX3->iNGPicker2Open) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && m_pDX3->iNGPicker3Open) gData.NGPicNo = gData.NGIdxNo=3;
		}
		if (m_nNGPickerCase == 171) {
			if(gData.NGJobPic[0]==1 && (!m_pDX3->iNGPicker1Up || m_pDX3->iNGPicker1Down)) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && (!m_pDX3->iNGPicker2Up || m_pDX3->iNGPicker2Down)) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && (!m_pDX3->iNGPicker3Up || m_pDX3->iNGPicker3Down)) gData.NGPicNo = gData.NGIdxNo=3;
		}
		if (m_nNGPickerCase == 181) {
			if(gData.NGJobPic[0]==1 && !m_pDX3->iNGPicker1CMCheck) gData.NGPicNo = gData.NGIdxNo=1;
			if(gData.NGJobPic[1]==1 && !m_pDX3->iNGPicker2CMCheck) gData.NGPicNo = gData.NGIdxNo=2;
			if(gData.NGJobPic[2]==1 && !m_pDX3->iNGPicker3CMCheck) gData.NGPicNo = gData.NGIdxNo=3;
		}

		if (m_nNGPickerCase == 301) {
			if(m_pDX3->iNGPicker1CMCheck) gData.nPicNo = 1;
			if(m_pDX3->iNGPicker2CMCheck) gData.nPicNo = 2;
			if(m_pDX3->iNGPicker3CMCheck) gData.nPicNo = 3;
#ifdef NG_PICKER_5
			if(m_pDX3->iNGPicker4CMCheck) gData.nPicNo = 4;
			if(m_pDX3->iNGPicker5CMCheck) gData.nPicNo = 5;
#endif
		}

		m_pCommon->Show_Error(5000 + m_nNGPickerCase);
		return FALSE;
	} else {
		return TRUE;
	}
}
	
// 7.  (Error : 6100)
BOOL CSequenceMain::GDPicker_Run()
{
	switch (m_nGDPickerCase) {
	case 100:
		if (m_nIndexTCase <= 200 && gData.IndexJob[4] == 0) {
			m_nGDPickerCase = 110;
			pLogFile->Save_MCCLog("Good Picker",100, "100");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		else if( gData.bUse100ScanRun && m_nInspectCase == 130 )
		{
			gData.bDone100ScanMotions[1] = FALSE;
			m_nGDPickerCase = 102;
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 102:
		m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Y, m_dGDPickerY, m_pMoveData->dDGdPickerY[5]);
		m_nGDPickerCase = 103; m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		break;
	case 103:
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY)) 
		{
			m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Y, m_dGDPickerY, m_pMoveData->dDGdPickerY[0]);
			m_nGDPickerCase = 104; m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 104:
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY)) 
		{
			if(m_nInspectCase == 200)
			{
				gData.bDone100ScanMotions[1] = TRUE;
				m_nGDPickerCase = 100;m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
			}
			else
			{
				m_nGDPickerCase = 102; m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
			}			
		}
		break;

	case 110:
		if (m_nIndexTCase <= 200 && gData.IndexJob[4] == 0) {
			if (Check_GdPickerAllUp()==TRUE) {
				if (Check_IndexGoodJobExist() == FALSE) {
					m_pDY2->oInspCMAlign3In = TRUE;
					m_pDY2->oInspCMAlign3Out = FALSE;
					m_pAJinAXL->Write_Output(2);

					if (gData.bUseGDVacuum) {
						m_pDY5->oGoodVacuumUp = FALSE;
						m_pDY5->oGoodVacuumDown = TRUE;

						m_pDY5->oGoodVacuumPad1On = FALSE;
						m_pDY5->oGoodVacuumPad2On = FALSE;
						m_pDY5->oGoodVacuumPad3On = FALSE;
						m_pDY5->oGoodVacuumPad4On = FALSE;
						m_pDY5->oGoodVacuumPad5On = FALSE;
						m_pDY5->oGoodVacuumPad6On = FALSE;
						m_pAJinAXL->Write_Output(5);
					}

					gData.IndexJob[4] = 1;
					m_pCommon->Move_Position(AX_GOOD_PICKER_Y, 1);
					m_pCommon->Move_Position(AX_GOOD_PICKER_Z, 0);
					m_nGDPickerCase = 120;
					pLogFile->Save_MCCLog("Good Picker",110, "110");
				} else {
					if (gData.bUseGDVacuum) {
						m_pDY5->oGoodVacuumUp = TRUE;
						m_pDY5->oGoodVacuumDown = FALSE;

						m_pDY5->oGoodVacuumPad1On = TRUE;
						m_pDY5->oGoodVacuumPad2On = TRUE;
						m_pDY5->oGoodVacuumPad3On = TRUE;
						m_pDY5->oGoodVacuumPad4On = TRUE;
						m_pDY5->oGoodVacuumPad5On = TRUE;
						m_pDY5->oGoodVacuumPad6On = TRUE;
						m_pAJinAXL->Write_Output(5);
					}
					m_nGDPickerCase = 130;
					pLogFile->Save_MCCLog("Good Picker",110, "110");
				}
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
			}
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_GOOD_PICKER_Y, 1) &&
			m_pCommon->Check_Position(AX_GOOD_PICKER_Z, 0) ) {
			m_nGDPickerCase = 100;
			pLogFile->Save_MCCLog("Good Picker",120, "120");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 30000);
		}
		break;

	case 130:
		if (Check_GdPickerAllUp()==TRUE) {
			gData.GDPicNo = gData.GDIdxNo = 0;
/*			if (Check_GoodAllPick()==TRUE) {
				m_pCommon->PickerGood_Move(1, 1, 1);
				m_dGDPickerY = m_pCommon->m_dP4Y;
				m_dGDPickerZ = m_pCommon->m_dP4Z;
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Y, m_dGDPickerY, m_pMoveData->dDGdPickerY[1]);
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Z, m_dGDPickerZ, m_pMoveData->dDGdPickerZ[1]);

				m_nGDPickerCase = 300;
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 10000);
			} else if (Check_GoodPickJobUp()==TRUE) { */
			if (Check_GoodPickJobUp()==TRUE) {
				m_pDY2->oInspCMAlign3In = FALSE;
				m_pDY2->oInspCMAlign3Out = TRUE;
				m_pAJinAXL->Write_Output(2);

				Check_GoodPickUpSelect();
				m_pCommon->PickerGood_Move(1, gData.GDPicNo, gData.GDIdxNo);
				m_dGDPickerY = m_pCommon->m_dP4Y;
				m_dGDPickerZ = m_pCommon->m_dP4Z;
				//m_pAJinAXL->Move_Absolute(AX_GOOD_PICKER_Y, m_dGDPickerY);
				//m_pAJinAXL->Move_Absolute(AX_GOOD_PICKER_Z, m_dGDPickerZ);
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Y, m_dGDPickerY, m_pMoveData->dDGdPickerY[1]);
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Z, m_dGDPickerZ, m_pMoveData->dDGdPickerZ[1]);

				m_nGDPickerCase = 140;
				pLogFile->Save_MCCLog("Good Picker",130, "130");
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 10000);
			} else {
				Check_GoodPickDownSelect();
				m_pCommon->PickerGood_Move(2, gData.GDPicNo, gData.GDIdxNo);
				m_dGDPickerY = m_pCommon->m_dP4Y;
				m_dGDPickerZ = m_pCommon->m_dP4Z;
				//m_pAJinAXL->Move_Absolute(AX_GOOD_PICKER_Y, m_dGDPickerY);
				//m_pAJinAXL->Move_Absolute(AX_GOOD_PICKER_Z, m_dGDPickerZ);
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Y, m_dGDPickerY, m_pMoveData->dDGdPickerY[2]);
				m_pAJinAXL->Move_Abs_Accel(AX_GOOD_PICKER_Z, m_dGDPickerZ, m_pMoveData->dDGdPickerZ[2]);

				m_nGDPickerCase = 220;
				pLogFile->Save_MCCLog("Good Picker",130, "130");
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 10000);
			}
		}
		break;

	case 140:
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY) &&
			m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Z, m_dGDPickerZ) ) 
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 700)) break;
			if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
				if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Up = FALSE; m_pDY4->oGoodPicker1Down = TRUE; }
				if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Up = FALSE; m_pDY4->oGoodPicker2Down = TRUE; }
				if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Up = FALSE; m_pDY4->oGoodPicker3Down = TRUE; }
				if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Up = FALSE; m_pDY4->oGoodPicker4Down = TRUE; }
				if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Up = FALSE; m_pDY4->oGoodPicker5Down = TRUE; }
				if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Up = FALSE; m_pDY4->oGoodPicker6Down = TRUE; }
				m_pAJinAXL->Write_Output(4);
				m_nGDPickerCase = 150;
				pLogFile->Save_MCCLog("Good Picker",140, "140");
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
			}
		}
		break;
	case 150:
		if((gData.GDPicNo==1 && !m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker1Down) ||
		   (gData.GDPicNo==2 && !m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker2Down) ||
		   (gData.GDPicNo==3 && !m_pDX4->iGoodPicker3Up && m_pDX4->iGoodPicker3Down) ||
		   (gData.GDPicNo==4 && !m_pDX4->iGoodPicker4Up && m_pDX4->iGoodPicker4Down) ||
		   (gData.GDPicNo==5 && !m_pDX4->iGoodPicker5Up && m_pDX4->iGoodPicker5Down) ||
		   (gData.GDPicNo==6 && !m_pDX4->iGoodPicker6Up && m_pDX4->iGoodPicker6Down) ) {
//			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 100)) break;
//			m_pDY2->oInspCMAlign3In = FALSE;
//			m_pDY2->oInspCMAlign3Out = TRUE;
//			m_pAJinAXL->Write_Output(2);
			m_nGDPickerCase = 160;
			pLogFile->Save_MCCLog("Good Picker",150, "150");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 160:
		if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
			if (gData.bUseGDVacuum) {
				m_pDY5->oGoodVacuumPad1On = FALSE;
				m_pDY5->oGoodVacuumPad2On = FALSE;
				m_pDY5->oGoodVacuumPad3On = FALSE;
				m_pDY5->oGoodVacuumPad4On = FALSE;
				m_pDY5->oGoodVacuumPad5On = FALSE;
				m_pDY5->oGoodVacuumPad6On = FALSE;
				m_pAJinAXL->Write_Output(5);
			}
			m_nGDPickerCase = 161;
			pLogFile->Save_MCCLog("Good Picker",160, "160");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 161:
		if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 50)) break;
			if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Open = FALSE; m_pDY4->oGoodPicker1Close = TRUE; }
			if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Open = FALSE; m_pDY4->oGoodPicker2Close = TRUE; }
			if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Open = FALSE; m_pDY4->oGoodPicker3Close = TRUE; }
			if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Open = FALSE; m_pDY4->oGoodPicker4Close = TRUE; }
			if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Open = FALSE; m_pDY4->oGoodPicker5Close = TRUE; }
			if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Open = FALSE; m_pDY4->oGoodPicker6Close = TRUE; }
			m_pAJinAXL->Write_Output(4);

			m_nGDPickerCase = 162;
			pLogFile->Save_MCCLog("Good Picker",161, "161");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 162:
		if( (gData.GDPicNo==1 && !m_pDX4->iGoodPicker1Open) ||
			(gData.GDPicNo==2 && !m_pDX4->iGoodPicker2Open) ||
			(gData.GDPicNo==3 && !m_pDX4->iGoodPicker3Open) ||
			(gData.GDPicNo==4 && !m_pDX4->iGoodPicker4Open) ||
			(gData.GDPicNo==5 && !m_pDX4->iGoodPicker5Open) ||
			(gData.GDPicNo==6 && !m_pDX4->iGoodPicker6Open) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 100)) break;
			if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Up = TRUE; m_pDY4->oGoodPicker1Down = FALSE; }
			if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Up = TRUE; m_pDY4->oGoodPicker2Down = FALSE; }
			if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Up = TRUE; m_pDY4->oGoodPicker3Down = FALSE; }
			if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Up = TRUE; m_pDY4->oGoodPicker4Down = FALSE; }
			if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Up = TRUE; m_pDY4->oGoodPicker5Down = FALSE; }
			if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Up = TRUE; m_pDY4->oGoodPicker6Down = FALSE; }
			m_pAJinAXL->Write_Output(4);
			m_nGDPickerCase = 170;
			pLogFile->Save_MCCLog("Good Picker",162, "162");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;

	case 170:
		if((gData.GDPicNo==1 && m_pDX4->iGoodPicker1Up && !m_pDX4->iGoodPicker1Down) ||
		   (gData.GDPicNo==2 && m_pDX4->iGoodPicker2Up && !m_pDX4->iGoodPicker2Down) ||
		   (gData.GDPicNo==3 && m_pDX4->iGoodPicker3Up && !m_pDX4->iGoodPicker3Down) ||
		   (gData.GDPicNo==4 && m_pDX4->iGoodPicker4Up && !m_pDX4->iGoodPicker4Down) ||
		   (gData.GDPicNo==5 && m_pDX4->iGoodPicker5Up && !m_pDX4->iGoodPicker5Down) ||
		   (gData.GDPicNo==6 && m_pDX4->iGoodPicker6Up && !m_pDX4->iGoodPicker6Down) ) {
			m_nGDPickerCase = 180;
			pLogFile->Save_MCCLog("Good Picker",170, "170");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 180:
//		if((!m_pEquipData->bUseCMCheck) ||
		if((gData.bUseDryRun) ||
		   (gData.GDPicNo==1 && m_pDX4->iGoodPicker1CMCheck) ||
		   (gData.GDPicNo==2 && m_pDX4->iGoodPicker2CMCheck) ||
		   (gData.GDPicNo==3 && m_pDX4->iGoodPicker3CMCheck) ||
		   (gData.GDPicNo==4 && m_pDX4->iGoodPicker4CMCheck) ||
		   (gData.GDPicNo==5 && m_pDX4->iGoodPicker5CMCheck) ||
		   (gData.GDPicNo==6 && m_pDX4->iGoodPicker6CMCheck) ) {

			gData.PickerInfor[2][gData.GDPicNo-1] = gData.IndexInfo[4][gData.GDIdxNo-1]; gData.IndexInfo[4][gData.GDIdxNo-1] = 0;
			gData.PickerGdPoNo[gData.GDPicNo-1]   = gData.IDXPoNo[4][gData.GDIdxNo-1];   gData.IDXPoNo[4][gData.GDIdxNo-1] = 0;
			gData.PickerGdTrayNo[gData.GDPicNo-1] = gData.IDXTrayNo[4][gData.GDIdxNo-1]; gData.IDXTrayNo[4][gData.GDIdxNo-1] = 0;

			gLot.nCmJigNo[gData.PickerGdTrayNo[gData.GDPicNo-1]-1][gData.PickerGdPoNo[gData.GDPicNo-1]-1][5] = gData.GDIdxNo;

			m_nGDPickerCase = 110;
			pLogFile->Save_MCCLog("Good Picker",180, "180");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;

	case 220:
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY) &&
			m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Z, m_dGDPickerZ) ) {
			m_pDY2->oInspCMAlign3In = FALSE;
			m_pDY2->oInspCMAlign3Out = TRUE;
			m_pAJinAXL->Write_Output(2);

			m_nGDPickerCase = 221;
			pLogFile->Save_MCCLog("Good Picker",220, "220");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 221:
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY) &&
			m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Z, m_dGDPickerZ) ) {
			if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
				if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 700)) break;
				m_nGDPickerCase = 230;
				pLogFile->Save_MCCLog("Good Picker",221, "221");
				m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
			}
		}
		break;

	case 230:
//		if((!m_pEquipData->bUseCMCheck) ||
		if((gData.bUseDryRun) ||
		   (gData.GDPicNo==1 && m_pDX4->iGoodPicker1CMCheck) ||
		   (gData.GDPicNo==2 && m_pDX4->iGoodPicker2CMCheck) ||
		   (gData.GDPicNo==3 && m_pDX4->iGoodPicker3CMCheck) ||
		   (gData.GDPicNo==4 && m_pDX4->iGoodPicker4CMCheck) ||
		   (gData.GDPicNo==5 && m_pDX4->iGoodPicker5CMCheck) ||
		   (gData.GDPicNo==6 && m_pDX4->iGoodPicker6CMCheck) ) 
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 300)) break;
			if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Up = FALSE; m_pDY4->oGoodPicker1Down = TRUE; }
			if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Up = FALSE; m_pDY4->oGoodPicker2Down = TRUE; }
			if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Up = FALSE; m_pDY4->oGoodPicker3Down = TRUE; }
			if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Up = FALSE; m_pDY4->oGoodPicker4Down = TRUE; }
			if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Up = FALSE; m_pDY4->oGoodPicker5Down = TRUE; }
			if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Up = FALSE; m_pDY4->oGoodPicker6Down = TRUE; }
			m_pAJinAXL->Write_Output(4);
			m_nGDPickerCase = 240;
			pLogFile->Save_MCCLog("Good Picker",230, "230");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 240:
		if((gData.GDPicNo==1 && !m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker1Down) ||
		   (gData.GDPicNo==2 && !m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker2Down) ||
		   (gData.GDPicNo==3 && !m_pDX4->iGoodPicker3Up && m_pDX4->iGoodPicker3Down) ||
		   (gData.GDPicNo==4 && !m_pDX4->iGoodPicker4Up && m_pDX4->iGoodPicker4Down) ||
		   (gData.GDPicNo==5 && !m_pDX4->iGoodPicker5Up && m_pDX4->iGoodPicker5Down) ||
		   (gData.GDPicNo==6 && !m_pDX4->iGoodPicker6Up && m_pDX4->iGoodPicker6Down) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 100)) break;
			if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Open = TRUE; m_pDY4->oGoodPicker1Close = FALSE; }
			if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Open = TRUE; m_pDY4->oGoodPicker2Close = FALSE; }
			if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Open = TRUE; m_pDY4->oGoodPicker3Close = FALSE; }
			if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Open = TRUE; m_pDY4->oGoodPicker4Close = FALSE; }
			if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Open = TRUE; m_pDY4->oGoodPicker5Close = FALSE; }
			if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Open = TRUE; m_pDY4->oGoodPicker6Close = FALSE; }
			m_pAJinAXL->Write_Output(4);

			if (gData.bUseGDVacuum) {
				m_pDY5->oGoodVacuumPad1On = FALSE;
				m_pDY5->oGoodVacuumPad2On = FALSE;
				m_pDY5->oGoodVacuumPad3On = FALSE;
				m_pDY5->oGoodVacuumPad4On = FALSE;
				m_pDY5->oGoodVacuumPad5On = FALSE;
				m_pDY5->oGoodVacuumPad6On = FALSE;
				m_pAJinAXL->Write_Output(5);
			}
			m_nGDPickerCase = 250;
			pLogFile->Save_MCCLog("Good Picker",240, "240");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 250:
		if((gData.GDPicNo==1 && m_pDX4->iGoodPicker1Open) ||
		   (gData.GDPicNo==2 && m_pDX4->iGoodPicker2Open) ||
		   (gData.GDPicNo==3 && m_pDX4->iGoodPicker3Open) ||
		   (gData.GDPicNo==4 && m_pDX4->iGoodPicker4Open) ||
		   (gData.GDPicNo==5 && m_pDX4->iGoodPicker5Open) ||
		   (gData.GDPicNo==6 && m_pDX4->iGoodPicker6Open) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 300)) break;
			m_pDY2->oInspCMAlign3In = FALSE;
			m_pDY2->oInspCMAlign3Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nGDPickerCase = 251;
			pLogFile->Save_MCCLog("Good Picker",250, "250");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 251:
		if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 700)) break;
			if (gData.GDPicNo==1) { m_pDY4->oGoodPicker1Up = TRUE; m_pDY4->oGoodPicker1Down = FALSE; }
			if (gData.GDPicNo==2) { m_pDY4->oGoodPicker2Up = TRUE; m_pDY4->oGoodPicker2Down = FALSE; }
			if (gData.GDPicNo==3) { m_pDY4->oGoodPicker3Up = TRUE; m_pDY4->oGoodPicker3Down = FALSE; }
			if (gData.GDPicNo==4) { m_pDY4->oGoodPicker4Up = TRUE; m_pDY4->oGoodPicker4Down = FALSE; }
			if (gData.GDPicNo==5) { m_pDY4->oGoodPicker5Up = TRUE; m_pDY4->oGoodPicker5Down = FALSE; }
			if (gData.GDPicNo==6) { m_pDY4->oGoodPicker6Up = TRUE; m_pDY4->oGoodPicker6Down = FALSE; }
			m_pAJinAXL->Write_Output(4);
			m_nGDPickerCase = 260;
			pLogFile->Save_MCCLog("Good Picker",251, "251");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 260:
		if((gData.GDPicNo==1 && m_pDX4->iGoodPicker1Up && !m_pDX4->iGoodPicker1Down) ||
		   (gData.GDPicNo==2 && m_pDX4->iGoodPicker2Up && !m_pDX4->iGoodPicker2Down) ||
		   (gData.GDPicNo==3 && m_pDX4->iGoodPicker3Up && !m_pDX4->iGoodPicker3Down) ||
		   (gData.GDPicNo==4 && m_pDX4->iGoodPicker4Up && !m_pDX4->iGoodPicker4Down) ||
		   (gData.GDPicNo==5 && m_pDX4->iGoodPicker5Up && !m_pDX4->iGoodPicker5Down) ||
		   (gData.GDPicNo==6 && m_pDX4->iGoodPicker6Up && !m_pDX4->iGoodPicker6Down) ) {

			gData.IndexInfo[4][gData.GDIdxNo-1] = gData.PickerInfor[2][gData.GDPicNo-1]; gData.PickerInfor[2][gData.GDPicNo-1] = 0;
			gData.IDXPoNo[4][gData.GDIdxNo-1]   = gData.PickerGdPoNo[gData.GDPicNo-1];   gData.PickerGdPoNo[gData.GDPicNo-1] = 0;
			gData.IDXTrayNo[4][gData.GDIdxNo-1] = gData.PickerGdTrayNo[gData.GDPicNo-1]; gData.PickerGdTrayNo[gData.GDPicNo-1] = 0;

			m_nGDPickerCase = 110;
			pLogFile->Save_MCCLog("Good Picker",260, "260");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;

	case 300:	//All Up
		if (m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Y, m_dGDPickerY) &&
			m_pAJinAXL->Is_MoveDone(AX_GOOD_PICKER_Z, m_dGDPickerZ) ) 
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 100)) break;
			m_pDY2->oInspCMAlign3In = FALSE;
			m_pDY2->oInspCMAlign3Out = TRUE;
			m_pAJinAXL->Write_Output(2);

			m_nGDPickerCase = 310;
			pLogFile->Save_MCCLog("Good Picker",300, "300");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 310:
		if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
//			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 100)) break;														  
			if (gData.IndexInfo[4][0] > 0) { m_pDY4->oGoodPicker1Up = FALSE; m_pDY4->oGoodPicker1Down = TRUE; }
			if (gData.IndexInfo[4][1] > 0) { m_pDY4->oGoodPicker2Up = FALSE; m_pDY4->oGoodPicker2Down = TRUE; }
			if (gData.IndexInfo[4][2] > 0) { m_pDY4->oGoodPicker3Up = FALSE; m_pDY4->oGoodPicker3Down = TRUE; }
			if (gData.IndexInfo[4][3] > 0) { m_pDY4->oGoodPicker4Up = FALSE; m_pDY4->oGoodPicker4Down = TRUE; }
			if (gData.IndexInfo[4][4] > 0) { m_pDY4->oGoodPicker5Up = FALSE; m_pDY4->oGoodPicker5Down = TRUE; }
			if (gData.IndexInfo[4][5] > 0) { m_pDY4->oGoodPicker6Up = FALSE; m_pDY4->oGoodPicker6Down = TRUE; }
			m_pAJinAXL->Write_Output(4);
			m_nGDPickerCase = 320;
			pLogFile->Save_MCCLog("Good Picker",310, "310");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 320:
		if(((gData.IndexInfo[4][0] > 0 && !m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker1Down) || gData.IndexInfo[4][0] == 0) &&
		   ((gData.IndexInfo[4][1] > 0 && !m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker2Down) || gData.IndexInfo[4][1] == 0) &&
		   ((gData.IndexInfo[4][2] > 0 && !m_pDX4->iGoodPicker3Up && m_pDX4->iGoodPicker3Down) || gData.IndexInfo[4][2] == 0) &&
		   ((gData.IndexInfo[4][3] > 0 && !m_pDX4->iGoodPicker4Up && m_pDX4->iGoodPicker4Down) || gData.IndexInfo[4][3] == 0) &&
		   ((gData.IndexInfo[4][4] > 0 && !m_pDX4->iGoodPicker5Up && m_pDX4->iGoodPicker5Down) || gData.IndexInfo[4][4] == 0) &&
		   ((gData.IndexInfo[4][5] > 0 && !m_pDX4->iGoodPicker6Up && m_pDX4->iGoodPicker6Down) || gData.IndexInfo[4][5] == 0) ) {
			if (gData.bUseGDVacuum) {
				m_pDY5->oGoodVacuumPad1On = FALSE;
				m_pDY5->oGoodVacuumPad2On = FALSE;
				m_pDY5->oGoodVacuumPad3On = FALSE;
				m_pDY5->oGoodVacuumPad4On = FALSE;
				m_pDY5->oGoodVacuumPad5On = FALSE;
				m_pDY5->oGoodVacuumPad6On = FALSE;
				m_pAJinAXL->Write_Output(5);
			}
			m_nGDPickerCase = 321;
			pLogFile->Save_MCCLog("Good Picker",320, "320");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 321:
		if (!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 700)) break;														  
			if (gData.IndexInfo[4][0] > 0) { m_pDY4->oGoodPicker1Open = FALSE; m_pDY4->oGoodPicker1Close = TRUE; }
			if (gData.IndexInfo[4][1] > 0) { m_pDY4->oGoodPicker2Open = FALSE; m_pDY4->oGoodPicker2Close = TRUE; }
			if (gData.IndexInfo[4][2] > 0) { m_pDY4->oGoodPicker3Open = FALSE; m_pDY4->oGoodPicker3Close = TRUE; }
			if (gData.IndexInfo[4][3] > 0) { m_pDY4->oGoodPicker4Open = FALSE; m_pDY4->oGoodPicker4Close = TRUE; }
			if (gData.IndexInfo[4][4] > 0) { m_pDY4->oGoodPicker5Open = FALSE; m_pDY4->oGoodPicker5Close = TRUE; }
			if (gData.IndexInfo[4][5] > 0) { m_pDY4->oGoodPicker6Open = FALSE; m_pDY4->oGoodPicker6Close = TRUE; }
			m_pAJinAXL->Write_Output(4);

			m_nGDPickerCase = 322;
			pLogFile->Save_MCCLog("Good Picker",321, "321");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 322:
		if(((gData.IndexInfo[4][0] > 0 && !m_pDX4->iGoodPicker1Open) || gData.IndexInfo[4][0] == 0) &&
			((gData.IndexInfo[4][1] > 0 && !m_pDX4->iGoodPicker2Open) || gData.IndexInfo[4][1] == 0) &&
			((gData.IndexInfo[4][2] > 0 && !m_pDX4->iGoodPicker3Open) || gData.IndexInfo[4][2] == 0) &&
			((gData.IndexInfo[4][3] > 0 && !m_pDX4->iGoodPicker4Open) || gData.IndexInfo[4][3] == 0) &&
			((gData.IndexInfo[4][4] > 0 && !m_pDX4->iGoodPicker5Open) || gData.IndexInfo[4][4] == 0) &&
			((gData.IndexInfo[4][5] > 0 && !m_pDX4->iGoodPicker6Open) || gData.IndexInfo[4][5] == 0) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_GDPICKER, 200)) break;
			if (gData.IndexInfo[4][0] > 0) { m_pDY4->oGoodPicker1Up = TRUE; m_pDY4->oGoodPicker1Down = FALSE; }
			if (gData.IndexInfo[4][1] > 0) { m_pDY4->oGoodPicker2Up = TRUE; m_pDY4->oGoodPicker2Down = FALSE; }
			if (gData.IndexInfo[4][2] > 0) { m_pDY4->oGoodPicker3Up = TRUE; m_pDY4->oGoodPicker3Down = FALSE; }
			if (gData.IndexInfo[4][3] > 0) { m_pDY4->oGoodPicker4Up = TRUE; m_pDY4->oGoodPicker4Down = FALSE; }
			if (gData.IndexInfo[4][4] > 0) { m_pDY4->oGoodPicker5Up = TRUE; m_pDY4->oGoodPicker5Down = FALSE; }
			if (gData.IndexInfo[4][5] > 0) { m_pDY4->oGoodPicker6Up = TRUE; m_pDY4->oGoodPicker6Down = FALSE; }
			m_pAJinAXL->Write_Output(4);
			m_nGDPickerCase = 330;
			pLogFile->Save_MCCLog("Good Picker",322, "322");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;

	case 330:
		if(((gData.IndexInfo[4][0] > 0 && m_pDX4->iGoodPicker1Up) || gData.IndexInfo[4][0] == 0) &&
		   ((gData.IndexInfo[4][1] > 0 && m_pDX4->iGoodPicker2Up) || gData.IndexInfo[4][1] == 0) &&
		   ((gData.IndexInfo[4][2] > 0 && m_pDX4->iGoodPicker3Up) || gData.IndexInfo[4][2] == 0) &&
		   ((gData.IndexInfo[4][3] > 0 && m_pDX4->iGoodPicker4Up) || gData.IndexInfo[4][3] == 0) &&
		   ((gData.IndexInfo[4][4] > 0 && m_pDX4->iGoodPicker5Up) || gData.IndexInfo[4][4] == 0) &&
		   ((gData.IndexInfo[4][5] > 0 && m_pDX4->iGoodPicker6Up) || gData.IndexInfo[4][5] == 0) ) {
			m_pDY2->oInspCMAlign3In = TRUE;
			m_pDY2->oInspCMAlign3Out = FALSE;
			m_pAJinAXL->Write_Output(2);
			m_nGDPickerCase = 340;
			pLogFile->Save_MCCLog("Good Picker",330, "330");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	case 340:
//		if((!m_pEquipData->bUseCMCheck) ||
		if((gData.bUseDryRun) ||
		  (((gData.IndexInfo[4][0] > 0 && m_pDX4->iGoodPicker1CMCheck) || gData.IndexInfo[4][0] == 0) &&
		   ((gData.IndexInfo[4][1] > 0 && m_pDX4->iGoodPicker2CMCheck) || gData.IndexInfo[4][1] == 0) &&
		   ((gData.IndexInfo[4][2] > 0 && m_pDX4->iGoodPicker3CMCheck) || gData.IndexInfo[4][2] == 0) &&
		   ((gData.IndexInfo[4][3] > 0 && m_pDX4->iGoodPicker4CMCheck) || gData.IndexInfo[4][3] == 0) &&
		   ((gData.IndexInfo[4][4] > 0 && m_pDX4->iGoodPicker5CMCheck) || gData.IndexInfo[4][4] == 0) &&
		   ((gData.IndexInfo[4][5] > 0 && m_pDX4->iGoodPicker6CMCheck) || gData.IndexInfo[4][5] == 0) )) {

			for(int i=0; i<6; i++) {
				gData.PickerInfor[2][i] = gData.IndexInfo[4][i]; gData.IndexInfo[4][i] = 0;
				gData.PickerGdPoNo[i]   = gData.IDXPoNo[4][i];   gData.IDXPoNo[4][i] = 0;
				gData.PickerGdTrayNo[i] = gData.IDXTrayNo[4][i]; gData.IDXTrayNo[4][i] = 0;

				if (gData.PickerInfor[2][i] > 0) {
					gLot.nCmJigNo[gData.PickerGdTrayNo[i]-1][gData.PickerGdPoNo[i]-1][5] = i+1;
				}
			}
			m_nGDPickerCase = 110;
			pLogFile->Save_MCCLog("Good Picker",340, "340");
			m_pCommon->Set_LoopTime(AUTO_GDPICKER, 5000);
		}
		break;
	}

	// 7.  (Error : 6100)
	if (m_pCommon->Over_LoopTime(AUTO_GDPICKER)) {
		if (m_nGDPickerCase == 100) return TRUE;

		if (m_nGDPickerCase == 310) {
			if(gData.IndexInfo[4][0] > 0 && !m_pDX4->iGoodPicker1Down) gData.GDPicNo = 1;
			if(gData.IndexInfo[4][1] > 0 && !m_pDX4->iGoodPicker2Down) gData.GDPicNo = 2;
			if(gData.IndexInfo[4][2] > 0 && !m_pDX4->iGoodPicker3Down) gData.GDPicNo = 3;
			if(gData.IndexInfo[4][3] > 0 && !m_pDX4->iGoodPicker4Down) gData.GDPicNo = 4;
			if(gData.IndexInfo[4][4] > 0 && !m_pDX4->iGoodPicker5Down) gData.GDPicNo = 5;
			if(gData.IndexInfo[4][5] > 0 && !m_pDX4->iGoodPicker6Down) gData.GDPicNo = 6;
		}
		if (m_nGDPickerCase == 322) {
			if(gData.IndexInfo[4][0] > 0 && m_pDX4->iGoodPicker1Open) gData.GDPicNo = 1;
			if(gData.IndexInfo[4][1] > 0 && m_pDX4->iGoodPicker2Open) gData.GDPicNo = 2;
			if(gData.IndexInfo[4][2] > 0 && m_pDX4->iGoodPicker3Open) gData.GDPicNo = 3;
			if(gData.IndexInfo[4][3] > 0 && m_pDX4->iGoodPicker4Open) gData.GDPicNo = 4;
			if(gData.IndexInfo[4][4] > 0 && m_pDX4->iGoodPicker5Open) gData.GDPicNo = 5;
			if(gData.IndexInfo[4][5] > 0 && m_pDX4->iGoodPicker6Open) gData.GDPicNo = 6;
		}
		if (m_nGDPickerCase == 330) {
			if(gData.IndexInfo[4][0] > 0 && !m_pDX4->iGoodPicker1Up) gData.GDPicNo = 1;
		 	if(gData.IndexInfo[4][1] > 0 && !m_pDX4->iGoodPicker2Up) gData.GDPicNo = 2;
		 	if(gData.IndexInfo[4][2] > 0 && !m_pDX4->iGoodPicker3Up) gData.GDPicNo = 3;
		 	if(gData.IndexInfo[4][3] > 0 && !m_pDX4->iGoodPicker4Up) gData.GDPicNo = 4;
		 	if(gData.IndexInfo[4][4] > 0 && !m_pDX4->iGoodPicker5Up) gData.GDPicNo = 5;
			if(gData.IndexInfo[4][5] > 0 && !m_pDX4->iGoodPicker6Up) gData.GDPicNo = 6;
		}
		if (m_nGDPickerCase == 340) {
			if(gData.IndexInfo[4][0] > 0 && !m_pDX4->iGoodPicker1CMCheck) gData.GDPicNo = 1;
		 	if(gData.IndexInfo[4][1] > 0 && !m_pDX4->iGoodPicker2CMCheck) gData.GDPicNo = 2;
		 	if(gData.IndexInfo[4][2] > 0 && !m_pDX4->iGoodPicker3CMCheck) gData.GDPicNo = 3;
		 	if(gData.IndexInfo[4][3] > 0 && !m_pDX4->iGoodPicker4CMCheck) gData.GDPicNo = 4;
		 	if(gData.IndexInfo[4][4] > 0 && !m_pDX4->iGoodPicker5CMCheck) gData.GDPicNo = 5;
			if(gData.IndexInfo[4][5] > 0 && !m_pDX4->iGoodPicker6CMCheck) gData.GDPicNo = 6;
		}

		if (m_nGDPickerCase == 310 || m_nGDPickerCase == 320 || m_nGDPickerCase == 330 || m_nGDPickerCase == 340) {
			gData.GDIdxNo = gData.GDPicNo;
		}
		m_pCommon->Show_Error(6000 + m_nGDPickerCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 8.  (Error : 7100)
BOOL CSequenceMain::LDPicker_Run()
{
	int		x;
	static int nLDPSNo = 0;

	switch (m_nLDPickerCase) {
	case 100:
		if((m_nIndexTCase <= 200 && gData.IndexJob[0] == 0) &&
			((m_nLoad1Case == 100 && m_nLoad2Case == 440) ||
			(m_nLoad1Case == 440 && m_nLoad2Case == 100) ||
			(m_nLoad1Case == 520 && m_nLoad2Case == 440)) ) {
				m_nLDPickerCase = 101;
				pLogFile->Save_MCCLog("Load Picker",100, "100_1");
				m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		} else 
			if (gData.bCleanOutMode == TRUE && gData.IndexJob[0] == 0) {
				m_nLDPickerCase = 102;
				pLogFile->Save_MCCLog("Load Picker",100, "100_2");
				m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
			} else 
				if (gData.nTrayPos[0] > 0 && (gData.bCleanOutMode == FALSE) &&
					(m_nLoad1Case == 300 || m_nLoad2Case == 300)) {
						m_nLDPickerCase = 110;
						pLogFile->Save_MCCLog("Load Picker",100, "100_3");
						m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
				}
				else if( gData.bUse100ScanRun && m_nInspectCase == 130)
				{
					gData.bDone100ScanMotions[2] = FALSE;
					m_nLDPickerCase = 103;
					m_pCommon->Set_LoopTime(AUTO_NGPICKER, 5000);
				}

				break;

	case 103:
		m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 2);
		m_nLDPickerCase = 104; m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		break;
	case 104:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 2)) 
		{
			m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 0);
			m_nLDPickerCase = 105; m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 105:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 0)) 
		{
			if(m_nInspectCase == 200)
			{
				gData.bDone100ScanMotions[2] = TRUE;
				m_nLDPickerCase = 100;m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
			}
			else
			{
				m_nLDPickerCase = 103; m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
			}			
		}
		break;

	case 101:
		if((m_nIndexTCase <= 200 && gData.IndexJob[0] == 0) &&
		  ((m_nLoad1Case == 100 && m_nLoad2Case == 440) ||
		   (m_nLoad1Case == 440 && m_nLoad2Case == 100) || 
		   (m_nLoad1Case == 520 && m_nLoad2Case == 440)) )
		{
			gData.IndexJob[0] = 1;//load 부에 Tray 가 다 들어가서 없어도 Job Done 으로 만들어줘서 Unloading 계속 진행 
			m_nLDPickerCase = 100;
			pLogFile->Save_MCCLog("Load Picker",101, "101");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 102:
		if (gData.bCleanOutMode == TRUE && gData.IndexJob[0] == 0) {
			gData.IndexJob[0] = 1;
			m_nLDPickerCase = 100;
			pLogFile->Save_MCCLog("Load Picker",102, "102");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;

	case 110:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0) ) {
			if (gData.nTrayPos[0] > 0 && //gData.IndexJob[0] == 0 &&
			   (m_nLoad1Case == 300 || m_nLoad2Case == 300)) {
				if (Check_LoadTrayCM() == TRUE) {
					if (m_nLoad1Case == 300) { m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 0); nLDPSNo = 1; }
					if (m_nLoad2Case == 300) { m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 1); nLDPSNo = 2; }
					m_nLDPickerCase = 120;
					pLogFile->Save_MCCLog("Load Picker",110, "110_1");
					m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
				} else {
					m_nLDPickerCase = 100;
					pLogFile->Save_MCCLog("Load Picker",110, "110_2");
					if (m_nLoad1Case==300) m_nLoad1Case = 310;
					if (m_nLoad2Case==300) m_nLoad2Case = 310;
					m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
				}
			}
		}
		break;
	case 120:
		if((m_nLoad1Case == 300 && m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 0)) ||
		   (m_nLoad2Case == 300 && m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 1)) ) {
			m_pCommon->Move_Position(AX_LOAD_PICKER_Z, 1);
			m_nLDPickerCase = 130;
			pLogFile->Save_MCCLog("Load Picker",120, "120");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 20000);
		}
		break;
	case 130:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 1) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 200)) break;
			m_pDY2->oMLPicker1Open = FALSE;
			m_pDY2->oMLPicker1Close = TRUE;
			m_pDY2->oMLPicker2Open = FALSE;
			m_pDY2->oMLPicker2Close = TRUE;
			m_pDY2->oMLPicker3Open = FALSE;
			m_pDY2->oMLPicker3Close = TRUE;
#ifdef PICKER_5
			m_pDY2->oMLPicker4Open = FALSE;
			m_pDY2->oMLPicker4Close = TRUE;
			m_pDY2->oMLPicker5Open = FALSE;
			m_pDY2->oMLPicker5Close = TRUE;
#endif
#ifdef PICKER_6
			m_pDY2->oMLPicker4Open = FALSE;
			m_pDY2->oMLPicker4Close = TRUE;
			m_pDY2->oMLPicker5Open = FALSE;
			m_pDY2->oMLPicker5Close = TRUE;
			m_pDY2->oMLPicker6Open = FALSE;
			m_pDY2->oMLPicker6Close = TRUE;
#endif
			m_pAJinAXL->Write_Output(2);

			m_nLDPickerCase = 140;
			pLogFile->Save_MCCLog("Load Picker",130, "130");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 140:
#ifdef PICKER_3
		if (!m_pDX2->iMLPicker1Open && !m_pDX2->iMLPicker2Open && !m_pDX2->iMLPicker3Open )
#endif
#ifdef PICKER_5
		if (!m_pDX2->iMLPicker1Open && !m_pDX2->iMLPicker2Open && !m_pDX2->iMLPicker3Open && 
			!m_pDX2->iMLPicker4Open && !m_pDX2->iMLPicker5Open )
#endif
#ifdef PICKER_6
		if (!m_pDX2->iMLPicker1Open && !m_pDX2->iMLPicker2Open && !m_pDX2->iMLPicker3Open && 
			!m_pDX2->iMLPicker4Open && !m_pDX2->iMLPicker5Open && !m_pDX2->iMLPicker6Open)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 200)) break;
			m_pCommon->Move_Position(AX_LOAD_PICKER_Z, 0);
			m_nLDPickerCase = 150;
			pLogFile->Save_MCCLog("Load Picker",140, "140");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 10000);
		}
		break;
	case 150:
		//x=gData.PickerLoadLineNo-1;
		//x=Get_TrayLineConvert(gData.nTrayPos[0])-1;
		x=gData.nTrayPos[0]-1;
		if((m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0)) &&
//		   ((!m_pEquipData->bUseCMCheck) ||
		   ((gData.bUseDryRun) ||
			(((gData.LoadTrayInfo[x][0]>0 && m_pDX2->iMLPicker1CMCheck) || gData.LoadTrayInfo[x][0]==0) &&
			 ((gData.LoadTrayInfo[x][1]>0 && m_pDX2->iMLPicker2CMCheck) || gData.LoadTrayInfo[x][1]==0) &&
			 ((gData.LoadTrayInfo[x][2]>0 && m_pDX2->iMLPicker3CMCheck) || gData.LoadTrayInfo[x][2]==0) &&
			 ((gData.LoadTrayInfo[x][3]>0 && m_pDX2->iMLPicker4CMCheck) || gData.LoadTrayInfo[x][3]==0) &&
			 ((gData.LoadTrayInfo[x][4]>0 && m_pDX2->iMLPicker5CMCheck) || gData.LoadTrayInfo[x][4]==0) &&
			 ((gData.LoadTrayInfo[x][5]>0 && m_pDX2->iMLPicker6CMCheck) || gData.LoadTrayInfo[x][5]==0)) )) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 200)) break;

			gData.PickerLoadTrayNo = gData.LoadTrayNo;
			gData.PickerLoadLineNo = gData.nTrayPos[0];
			//x=Get_TrayLineConvert(gData.PickerLoadLineNo)-1;

			int cm = (gData.PickerLoadLineNo - 1) * gData.nPickCnt;
			for(int i=0; i<gData.nPickCnt; i++) {
				gData.PickerInfor[0][i] = gData.LoadTrayInfo[x][i]; gData.LoadTrayInfo[x][i] = 0;
				if (gData.PickerInfor[0][i] > 0) {
					gLot.nCmJigNo[gData.PickerLoadTrayNo-1][cm+i][0] = nLDPSNo;
					gLot.nCmJigNo[gData.PickerLoadTrayNo-1][cm+i][1] = i+1;
				}
			}

			m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 2);
			m_pCommon->Move_Position(AX_LOAD_PICKER_Y2, 1);
			if (m_nLoad1Case==300) m_nLoad1Case = 310;
			if (m_nLoad2Case==300) m_nLoad2Case = 310;

			m_nLDPickerCase = 160;
			pLogFile->Save_MCCLog("Load Picker",150, "150");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 2) && m_pCommon->Check_Position(AX_LOAD_PICKER_Y2, 1) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 50)) break;
			m_nLDPickerCase = 200;
			pLogFile->Save_MCCLog("Load Picker",160, "160");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
		}
		break;

	case 200:
		if(((m_nIndexTCase >= 100 && m_nIndexTCase <= 131) && gData.IndexJob[0] == 0) ||
		   ( m_nIndexTCase == 200 && gData.IndexJob[0] == 0 && gData.bCleanOutMode == TRUE)) {
			m_pDY2->oInspCMAlign1In = FALSE;
			m_pDY2->oInspCMAlign1Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nLDPickerCase = 201;
			pLogFile->Save_MCCLog("Load Picker",200, "200");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 201:		
//		if ((!m_pEquipData->bUseCMCheck) ||
		if ((gData.bUseDryRun) ||
		   (((gData.PickerInfor[0][0]>0 && m_pDX2->iMLPicker1CMCheck) || gData.PickerInfor[0][0]==0) &&
			((gData.PickerInfor[0][1]>0 && m_pDX2->iMLPicker2CMCheck) || gData.PickerInfor[0][1]==0) &&
			((gData.PickerInfor[0][2]>0 && m_pDX2->iMLPicker3CMCheck) || gData.PickerInfor[0][2]==0) &&
			((gData.PickerInfor[0][3]>0 && m_pDX2->iMLPicker4CMCheck) || gData.PickerInfor[0][3]==0) &&
			((gData.PickerInfor[0][4]>0 && m_pDX2->iMLPicker5CMCheck) || gData.PickerInfor[0][4]==0) &&
			((gData.PickerInfor[0][5]>0 && m_pDX2->iMLPicker6CMCheck) || gData.PickerInfor[0][5]==0)) ) {
			m_nLDPickerCase = 202;
			pLogFile->Save_MCCLog("Load Picker",201, "201");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 202:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 2) ) {
			
			if (!m_pDX2->iInspCMAlign1In && m_pDX2->iInspCMAlign1Out) {
				if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 400)) break;
				m_pCommon->Move_Position(AX_LOAD_PICKER_Z, 3);
				m_nLDPickerCase = 210;
				pLogFile->Save_MCCLog("Load Picker",202, "202");
				m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
			}
		}
		break;
	case 210:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 3) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER,	100)) break;
			m_pDY2->oMLPicker1Open = TRUE;
			m_pDY2->oMLPicker1Close = FALSE;
			m_pDY2->oMLPicker2Open = TRUE;
			m_pDY2->oMLPicker2Close = FALSE;
			m_pDY2->oMLPicker3Open = TRUE;
			m_pDY2->oMLPicker3Close = FALSE;
#ifdef PICKER_5
			m_pDY2->oMLPicker4Open = TRUE;
			m_pDY2->oMLPicker4Close = FALSE;
			m_pDY2->oMLPicker5Open = TRUE;
			m_pDY2->oMLPicker5Close = FALSE;
#endif
#ifdef PICKER_6
			m_pDY2->oMLPicker4Open = TRUE;
			m_pDY2->oMLPicker4Close = FALSE;
			m_pDY2->oMLPicker5Open = TRUE;
			m_pDY2->oMLPicker5Close = FALSE;
			m_pDY2->oMLPicker6Open = TRUE;
			m_pDY2->oMLPicker6Close = FALSE;
#endif
			m_pAJinAXL->Write_Output(2);

			m_nLDPickerCase = 220;
			pLogFile->Save_MCCLog("Load Picker",210, "210");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 220:
#ifdef PICKER_3
		if (m_pDX2->iMLPicker1Open && m_pDX2->iMLPicker2Open && m_pDX2->iMLPicker3Open )
#endif
#ifdef PICKER_5
		if (m_pDX2->iMLPicker1Open && m_pDX2->iMLPicker2Open && m_pDX2->iMLPicker3Open && 
			m_pDX2->iMLPicker4Open && m_pDX2->iMLPicker5Open )
#endif
#ifdef PICKER_6
		if (m_pDX2->iMLPicker1Open && m_pDX2->iMLPicker2Open && m_pDX2->iMLPicker3Open && 
			m_pDX2->iMLPicker4Open && m_pDX2->iMLPicker5Open && m_pDX2->iMLPicker6Open)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 50)) break;
			m_pCommon->Move_Position(AX_LOAD_PICKER_Z, 0);
			m_nLDPickerCase = 221;
			pLogFile->Save_MCCLog("Load Picker",220, "220");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;
	case 221:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 100)) break;
			m_pDY2->oInspCMAlign1In = TRUE;
			m_pDY2->oInspCMAlign1Out = FALSE;
			m_pAJinAXL->Write_Output(2);
			m_nLDPickerCase = 230;
			pLogFile->Save_MCCLog("Load Picker",221, "221");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
		}
		break;

	case 230:
		if (m_pDX2->iInspCMAlign1In && !m_pDX2->iInspCMAlign1Out) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 50)) break;

			gData.IDXLineNo[0] = gData.PickerLoadLineNo;
			gData.PickerLoadLineNo = 0;
			//int cm = (Get_TrayLineConvert(gData.IDXLineNo[0]) -1) * 5;
			int cm = (gData.IDXLineNo[0] - 1) * gData.nPickCnt;
			for(int i=0; i<6; i++) {
				gData.IDXTrayNo[0][i] = gData.PickerLoadTrayNo;
				gData.IndexInfo[0][i] = gData.PickerInfor[0][i]; gData.PickerInfor[0][i] = 0;
				gData.IDXPoNo[0][i] = cm+i+1;	//Pocket No Set
				if (gData.IndexInfo[0][i] > 0) {
					gLot.nCmJigNo[gData.PickerLoadTrayNo-1][cm+i][2] = i+1; // 몇번 피커가 Pick Up 했던건지 
				}
			}
			gData.PickerLoadTrayNo = 0;
			gData.IndexJob[0] = 1;
			gData.nStatus = 2;	//Load Picker 작업시작

			m_pCommon->Move_Position(AX_LOAD_PICKER_Y1, 0);
			m_pCommon->Move_Position(AX_LOAD_PICKER_Y2, 0);

			m_nLDPickerCase = 240;
			pLogFile->Save_MCCLog("Load Picker",230, "230");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 30000);
		}
		break;
	case 240:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 0) && m_pCommon->Check_Position(AX_LOAD_PICKER_Y2, 0) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_LDPICKER, 300)) break;
			m_nLDPickerCase = 100;
			pLogFile->Save_MCCLog("Load Picker",240, "240");
			m_pCommon->Set_LoopTime(AUTO_LDPICKER, 5000);
		}
		break;

	}

	// 8.  (Error : 7100)
	if (m_pCommon->Over_LoopTime(AUTO_LDPICKER)) {
		if (m_nLDPickerCase == 100 || m_nLDPickerCase == 200) return TRUE;

		if (m_nLDPickerCase == 140) {
			if (m_pDX2->iMLPicker1Open) gData.nPicNo = 1;
			if (m_pDX2->iMLPicker2Open) gData.nPicNo = 2;
			if (m_pDX2->iMLPicker3Open) gData.nPicNo = 3;
#ifdef PICKER_5
			if (m_pDX2->iMLPicker4Open) gData.nPicNo = 4;
			if (m_pDX2->iMLPicker5Open) gData.nPicNo = 5;
#endif
#ifdef PICKER_6
			if (m_pDX2->iMLPicker4Open) gData.nPicNo = 4;
			if (m_pDX2->iMLPicker5Open) gData.nPicNo = 5;
			if (m_pDX2->iMLPicker6Open) gData.nPicNo = 6;
#endif
		}
		if (m_nLDPickerCase == 150) {
			//x=Get_TrayLineConvert(gData.nTrayPos[0])-1;
			x = gData.nTrayPos[0] - 1;
			if (gData.LoadTrayInfo[x][0]>0 && !m_pDX2->iMLPicker1CMCheck) gData.nPicNo = 1;
			if (gData.LoadTrayInfo[x][1]>0 && !m_pDX2->iMLPicker2CMCheck) gData.nPicNo = 2;
			if (gData.LoadTrayInfo[x][2]>0 && !m_pDX2->iMLPicker3CMCheck) gData.nPicNo = 3;
			if (gData.LoadTrayInfo[x][3]>0 && !m_pDX2->iMLPicker4CMCheck) gData.nPicNo = 4;
			if (gData.LoadTrayInfo[x][4]>0 && !m_pDX2->iMLPicker5CMCheck) gData.nPicNo = 5;
			if (gData.LoadTrayInfo[x][5]>0 && !m_pDX2->iMLPicker6CMCheck) gData.nPicNo = 6;
		}
		if (m_nLDPickerCase == 201) {
		    if (gData.PickerInfor[0][0]>0 && !m_pDX2->iMLPicker1CMCheck) gData.nPicNo = 1;
			if (gData.PickerInfor[0][1]>0 && !m_pDX2->iMLPicker2CMCheck) gData.nPicNo = 2;
			if (gData.PickerInfor[0][2]>0 && !m_pDX2->iMLPicker3CMCheck) gData.nPicNo = 3;
			if (gData.PickerInfor[0][3]>0 && !m_pDX2->iMLPicker4CMCheck) gData.nPicNo = 4;
			if (gData.PickerInfor[0][4]>0 && !m_pDX2->iMLPicker5CMCheck) gData.nPicNo = 5;
			if (gData.PickerInfor[0][5]>0 && !m_pDX2->iMLPicker6CMCheck) gData.nPicNo = 6;
		}
		if (m_nLDPickerCase == 220) {
			if (!m_pDX2->iMLPicker1Open) gData.nPicNo = 1;
			if (!m_pDX2->iMLPicker2Open) gData.nPicNo = 2;
			if (!m_pDX2->iMLPicker3Open) gData.nPicNo = 3;
#ifdef PICKER_5
			if (!m_pDX2->iMLPicker4Open) gData.nPicNo = 4;
			if (!m_pDX2->iMLPicker5Open) gData.nPicNo = 5;
#endif
#ifdef PICKER_6
			if (!m_pDX2->iMLPicker4Open) gData.nPicNo = 4;
			if (!m_pDX2->iMLPicker5Open) gData.nPicNo = 5;
			if (!m_pDX2->iMLPicker6Open) gData.nPicNo = 6;
#endif
		}

		m_pCommon->Show_Error(7000 + m_nLDPickerCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 9.  (Error : 8100)
BOOL CSequenceMain::ULPicker_Run()
{
	switch (m_nULPickerCase) {
	case 100:
		if (m_nIndexTCase <= 200 && gData.IndexJob[5] == 0) {
			m_nULPickerCase = 110;
			pLogFile->Save_MCCLog("Unload Picker",100, "100");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		else if( gData.bUse100ScanRun && m_nInspectCase == 130)
		{
			gData.bDone100ScanMotions[3] = FALSE;
			m_nULPickerCase = 102;
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 102:
		m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 1);
		m_nULPickerCase = 103; m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		break;
	case 103:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 1)) 
		{
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 0);
			m_nULPickerCase = 104; m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 104:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 0)) 
		{
			if(m_nInspectCase == 200)
			{
				gData.bDone100ScanMotions[3] = TRUE;
				m_nULPickerCase = 100;m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
			}
			else
			{
				m_nULPickerCase = 102; m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
			}			
		}
		break;
	case 110:
		if (m_nIndexTCase <= 200 && gData.IndexJob[5] == 0) {
			if (Check_IndexEmpty(6) == TRUE) {
				gData.IndexJob[5] = 1;
				m_nULPickerCase = 100;
				pLogFile->Save_MCCLog("Unload Picker",110, "110_1");
			} else {
				m_nULPickerCase = 120;
				pLogFile->Save_MCCLog("Unload Picker",110, "110_2");
			}
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 0) ) {
			m_pDY2->oInspCMAlign4In = FALSE;
			m_pDY2->oInspCMAlign4Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nULPickerCase = 130;
			pLogFile->Save_MCCLog("Unload Picker",120, "120");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
		}
		break;
	case 130:
		if (!m_pDX2->iInspCMAlign4In && m_pDX2->iInspCMAlign4Out ) {
//			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 1);
			m_nULPickerCase = 140;
			pLogFile->Save_MCCLog("Unload Picker",130, "130");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 140:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 1) ) {
			m_pDY5->oMUPicker1Open = FALSE;
			m_pDY5->oMUPicker1Close = TRUE;
			m_pDY5->oMUPicker2Open = FALSE;
			m_pDY5->oMUPicker2Close = TRUE;
			m_pDY5->oMUPicker3Open = FALSE;
			m_pDY5->oMUPicker3Close = TRUE;
#ifdef PICKER_5
			m_pDY5->oMUPicker4Open = FALSE;
			m_pDY5->oMUPicker4Close = TRUE;
			m_pDY5->oMUPicker5Open = FALSE;
			m_pDY5->oMUPicker5Close = TRUE;
#endif
#ifdef PICKER_6
			m_pDY5->oMUPicker4Open = FALSE;
			m_pDY5->oMUPicker4Close = TRUE;
			m_pDY5->oMUPicker5Open = FALSE;
			m_pDY5->oMUPicker5Close = TRUE;
			m_pDY5->oMUPicker6Open = FALSE;
			m_pDY5->oMUPicker6Close = TRUE;
#endif
			m_pAJinAXL->Write_Output(5);
			m_nULPickerCase = 142;
			pLogFile->Save_MCCLog("Unload Picker",140, "140");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 141:
		if (!m_pDX2->iInspCMAlign4In && m_pDX2->iInspCMAlign4Out ) {
			m_nULPickerCase = 142;
			pLogFile->Save_MCCLog("Unload Picker",141, "141");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 10000);
		}
		break;
	case 142:
#ifdef PICKER_3
		if (!m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open )
#endif
#ifdef PICKER_5
		if (!m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open && 
			!m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open )
#endif
#ifdef PICKER_6
		if (!m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open && 
			!m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open && !m_pDX5->iMUPicker6Open)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 0);
			m_nULPickerCase = 150;
			pLogFile->Save_MCCLog("Unload Picker",142, "142");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 10000);
		}
		break;

	case 150:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
//			if((!m_pEquipData->bUseCMCheck) ||
			if((gData.bUseDryRun) ||
			   (((gData.IndexInfo[6][0]>0 && m_pDX5->iMUPicker1CMCheck) || gData.IndexInfo[6][0]==0) &&
			    ((gData.IndexInfo[6][1]>0 && m_pDX5->iMUPicker2CMCheck) || gData.IndexInfo[6][1]==0) &&
			    ((gData.IndexInfo[6][2]>0 && m_pDX5->iMUPicker3CMCheck) || gData.IndexInfo[6][2]==0) &&
			    ((gData.IndexInfo[6][3]>0 && m_pDX5->iMUPicker4CMCheck) || gData.IndexInfo[6][3]==0) &&
				((gData.IndexInfo[6][4]>0 && m_pDX5->iMUPicker5CMCheck) || gData.IndexInfo[6][4]==0)  &&
				((gData.IndexInfo[6][5]>0 && m_pDX5->iMUPicker6CMCheck) || gData.IndexInfo[6][5]==0)) ) {
				m_pDY2->oInspCMAlign4In = TRUE;
				m_pDY2->oInspCMAlign4Out = FALSE;
				m_pAJinAXL->Write_Output(2);

				for(int i=0; i<6; i++) {
					if (gData.IndexInfo[6][i] > 0) {
						gLot.nCmJigNo[gData.IDXTrayNo[6][i]-1][gData.IDXPoNo[6][i]-1][3] = i+1;
					}

					gData.PickerUnTrayNo[i] = gData.IDXTrayNo[6][i]; gData.IDXTrayNo[6][i] = 0;
					gData.PickerInfor[3][i] = gData.IndexInfo[6][i]; gData.IndexInfo[6][i] = 0;
					gData.PickerUnPoNo[i]   = gData.IDXPoNo[6][i];   gData.IDXPoNo[6][i] = 0;

					if (gData.PickerInfor[3][i] > 0) {
						gLot.nCmJigNo[gData.PickerUnTrayNo[i]-1][gData.PickerUnPoNo[i]-1][6] = i+1;
					}
				}
				gData.IDXLineNo[6] = gData.IDXInspec[6] = 0;
				gData.IndexJob[5] = 1;
				gData.nDoorInterlock67 = 0;

				if (m_nUnload2Case == 300)  m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 2);
				else						m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 1);
				m_pCommon->Move_Position(AX_UNLOAD_PICKER_X2, 1);
				m_nULPickerCase = 160;
				pLogFile->Save_MCCLog("Unload Picker",150, "150");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
			}
		}
		break;
	case 160:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 1) || m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 2) ) {
			if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X2, 1) ) {
				m_nULPickerCase = 170;
				pLogFile->Save_MCCLog("Unload Picker",160, "160");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
			}
		}
		break;
	case 170:
//		if ((!m_pEquipData->bUseCMCheck) ||
		if ((gData.bUseDryRun) ||
			(((gData.PickerInfor[3][0]>0 && m_pDX5->iMUPicker1CMCheck) || gData.PickerInfor[3][0]==0) &&
			 ((gData.PickerInfor[3][1]>0 && m_pDX5->iMUPicker2CMCheck) || gData.PickerInfor[3][1]==0) &&
			 ((gData.PickerInfor[3][2]>0 && m_pDX5->iMUPicker3CMCheck) || gData.PickerInfor[3][2]==0) &&
			 ((gData.PickerInfor[3][3]>0 && m_pDX5->iMUPicker4CMCheck) || gData.PickerInfor[3][3]==0) &&
			 ((gData.PickerInfor[3][4]>0 && m_pDX5->iMUPicker5CMCheck) || gData.PickerInfor[3][4]==0) &&
			 ((gData.PickerInfor[3][5]>0 && m_pDX5->iMUPicker6CMCheck) || gData.PickerInfor[3][5]==0)) ) {
			m_nULPickerCase = 200;
			pLogFile->Save_MCCLog("Unload Picker",170, "170");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
		}
		break;

	case 200:
		if (gData.nTrayPos[3] > 0 &&
		   (m_nUnload1Case == 300 || m_nUnload2Case == 300)) {
			m_nULPickerCase = 201;
			pLogFile->Save_MCCLog("Unload Picker",200, "200");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 10000);
		} else if ((m_nUnload1Case==100 && m_nUnload2Case==440) ||
				   (m_nUnload1Case==440 && m_nUnload2Case==100) ) {
					if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 1000)) break;
					m_pCommon->Show_Error(3600);
					return FALSE;
		}
		if (gData.nTrayPos[3] == 0 &&
		   (m_nUnload1Case == 300 || m_nUnload2Case == 300))
		{
			gData.nTrayPos[3] = 1;
		}
// 		if (m_nUnload1Case == 250 && (m_nUnload2Case == 200)) m_nUnload1Case = 260;
// 		if (m_nUnload2Case == 250 && (m_nUnload1Case == 200)) m_nUnload2Case = 260;
		// 작업 위치만 아니면 이동한다.
// 		if (m_nUnload1Case == 250 && (m_nUnload2Case <= 200 || m_nUnload2Case > 350)) {
// 				m_nUnload1Case = 260;
// 		}
// 		if (m_nUnload2Case == 250 && (m_nUnload1Case <= 200 || m_nUnload1Case > 350)) {
// 				 m_nUnload2Case = 260;
// 		}

		break;
	case 201:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			int nLine = gData.nTrayPos[3];	//Get_TrayLineConvert(gData.nTrayPos[3]);
			if (Check_GoodTrayEmpty(nLine)==TRUE) {
				if (m_nUnload1Case == 300) m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 1);
				if (m_nUnload2Case == 300) m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 2);
				m_nULPickerCase = 202;
				pLogFile->Save_MCCLog("Unload Picker",201, "201");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
			} else {
				if (m_nUnload1Case==300) m_nUnload1Case = 310;
				if (m_nUnload2Case==300) m_nUnload2Case = 310;
				m_nULPickerCase = 160;
				pLogFile->Save_MCCLog("Unload Picker",201, "201");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
			}
		}
		break;
	case 202:
		if((m_nUnload1Case == 300 && m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 1)) ||
		   (m_nUnload2Case == 300 && m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 2)) ) {
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 2);
			m_nULPickerCase = 203;
			pLogFile->Save_MCCLog("Unload Picker",202, "202");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
		}
		break;
	case 203:
#ifdef TRAY_CHECK2
		if((m_nUnload1Case == 300 && m_pDX6->iUS_Z1Check1 && m_pDX6->iUS_Z1Check2) ||
		   (m_nUnload2Case == 300 && m_pDX6->iUS_Z2Check1 && m_pDX6->iUS_Z2Check2) )
#else
		if((m_nUnload1Case == 300 && m_pDX6->iUS_Z1Check1) ||
		   (m_nUnload2Case == 300 && m_pDX6->iUS_Z2Check1) )
#endif
		{
			m_nULPickerCase = 210;
			pLogFile->Save_MCCLog("Unload Picker",203, "203");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
		}
		break;

	case 210:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 2) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pDY5->oMUPicker1Open = TRUE;
			m_pDY5->oMUPicker1Close = FALSE;
			m_pDY5->oMUPicker2Open = TRUE;
			m_pDY5->oMUPicker2Close = FALSE;
			m_pDY5->oMUPicker3Open = TRUE;
			m_pDY5->oMUPicker3Close = FALSE;
#ifdef PICKER_5
			m_pDY5->oMUPicker4Open = TRUE;
			m_pDY5->oMUPicker4Close = FALSE;
			m_pDY5->oMUPicker5Open = TRUE;
			m_pDY5->oMUPicker5Close = FALSE;
#endif
#ifdef PICKER_6
			m_pDY5->oMUPicker4Open = TRUE;
			m_pDY5->oMUPicker4Close = FALSE;
			m_pDY5->oMUPicker5Open = TRUE;
			m_pDY5->oMUPicker5Close = FALSE;
			m_pDY5->oMUPicker6Open = TRUE;
			m_pDY5->oMUPicker6Close = FALSE;
#endif
			m_pAJinAXL->Write_Output(5);

			m_nULPickerCase = 220;
			pLogFile->Save_MCCLog("Unload Picker",210, "210");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 220:
#ifdef PICKER_3
		if (m_pDX5->iMUPicker1Open && m_pDX5->iMUPicker2Open && m_pDX5->iMUPicker3Open )
#endif
#ifdef PICKER_5
		if (m_pDX5->iMUPicker1Open && m_pDX5->iMUPicker2Open && m_pDX5->iMUPicker3Open && 
			m_pDX5->iMUPicker4Open && m_pDX5->iMUPicker5Open )
#endif
#ifdef PICKER_6
		if (m_pDX5->iMUPicker1Open && m_pDX5->iMUPicker2Open && m_pDX5->iMUPicker3Open && 
			m_pDX5->iMUPicker4Open && m_pDX5->iMUPicker5Open && m_pDX5->iMUPicker6Open)
#endif
		{
			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 0);
			m_nULPickerCase = 221;
			pLogFile->Save_MCCLog("Unload Picker",220, "220");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 10000);
		}
		break;
	case 221:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			m_nULPickerCase = 230;
			pLogFile->Save_MCCLog("Unload Picker",221, "221");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 230:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {

			int nTNo, nPno, nGdPos;
			int w = gData.nTrayPos[3]-1;	//Get_TrayLineConvert(gData.nTrayPos[3])-1;

			
			for(int i=0; i<gData.nPickCnt; i++) {
				nTNo = (gData.PickerUnTrayNo[i] > 0 ? (gData.PickerUnTrayNo[i] - 1) : 0);
				gData.GoodTrayInfo[w][i] = gData.PickerInfor[3][i];	gData.PickerInfor[3][i] = 0;
				if (gData.GoodTrayInfo[w][i] > 0) {
					nGdPos = (gData.nPickCnt*w) + (gData.nPickCnt-i);
					nPno = (gData.PickerUnPoNo[i] > 0 ? (gData.PickerUnPoNo[i] - 1) : -1);
					//if (m_pEquipData->bUseInlineMode && nPno != -1) g_objCapAttachUDP.Set_BarcodeUpdate(1, gData.nGoodTrayCount+1, nGdPos, gLot.sBarLoad[nTNo][nPno]);
					if (nPno != -1) {
						g_objCapAttachUDP.Set_BarcodeUpdate(gData.nPortNo, gData.nGoodTrayCount+1, nGdPos, gLot.sBarLoad[nTNo][nPno]);
						//g_objCapAttachUDP.Set_BarcodeUpdate(1, gData.nGoodTrayCount+1, nGdPos, gLot.sBarLoad[nTNo][nPno]);
					}
					//g_objMES.Set_Result(gLot.sLotID, gLot.sBarLoad[nTNo][nPno], "OK", gLot.sNGCode[nTNo][nPno], gLot.sNGText[nTNo][nPno], nTNo+1, nPno+1, gData.nGoodTrayNo, nGdPos, 0,0);
					pLogFile->Save_CmTrackingLog("GOOD", gData.nGoodTrayCount+1, i+1, w+1, gData.PickerUnTrayNo[i], gData.PickerUnPoNo[i]);
					g_objMES.Save_ProcessedData(gLot.sLotID, gLot.sBarLoad[nTNo][nPno], "OK", gLot.sNGCode[nTNo][nPno], gLot.sNGText[nTNo][nPno], nTNo+1, nPno+1, gData.nGoodTrayNo, nGdPos, 0,0);
					m_nUnloadLotCmCnt++;
				}
			}

			if (Check_LotEnd()==TRUE && (m_nUnload1Case==300 || m_nUnload2Case==300)) {
				m_nULPickerCase = 300;
				pLogFile->Save_MCCLog("Unload Picker",230, "230");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);

			} else if (m_pEquipData->bUseContinueLot && m_nUnloadLotCmCnt == gData.nCmsUseCnt[m_nUnloadLotIndex] && (m_nUnload1Case==300 || m_nUnload2Case==300)) {
				gData.bContinueLotEnd[m_nUnloadLotIndex] = TRUE;
				m_nULPickerCase = 300;
				pLogFile->Save_MCCLog("Unload Picker",230, "230");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);

			} else {

				gData.nCUnloadLotIndex = m_nUnloadLotIndex;
				m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 0);
				m_pCommon->Move_Position(AX_UNLOAD_PICKER_X2, 0);
				if (m_nUnload1Case==300) m_nUnload1Case = 310;
				if (m_nUnload2Case==300) m_nUnload2Case = 310;

				m_nULPickerCase = 240;
				pLogFile->Save_MCCLog("Unload Picker",230, "230");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
			}
		}
		break;
	case 240:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 0) && m_pCommon->Check_Position(AX_UNLOAD_PICKER_X2, 0) ) {
			m_nULPickerCase = 250;
			pLogFile->Save_MCCLog("Unload Picker",240, "240");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 250:
//		if ((!m_pEquipData->bUseCMCheck) ||
#ifdef PICKER_3
		if ((gData.bUseDryRun) ||
			(!m_pDX5->iMUPicker1CMCheck && !m_pDX5->iMUPicker2CMCheck && !m_pDX5->iMUPicker3CMCheck) )
#endif
#ifdef PICKER_5
		if ((gData.bUseDryRun) ||
			(!m_pDX5->iMUPicker1CMCheck && !m_pDX5->iMUPicker2CMCheck && !m_pDX5->iMUPicker3CMCheck &&
			 !m_pDX5->iMUPicker4CMCheck && !m_pDX5->iMUPicker5CMCheck) )
#endif
#ifdef PICKER_6
		if ((gData.bUseDryRun) ||
			(!m_pDX5->iMUPicker1CMCheck && !m_pDX5->iMUPicker2CMCheck && !m_pDX5->iMUPicker3CMCheck &&
			 !m_pDX5->iMUPicker4CMCheck && !m_pDX5->iMUPicker5CMCheck && !m_pDX5->iMUPicker6CMCheck) )
#endif
		{

			m_nULPickerCase = 100;
			pLogFile->Save_MCCLog("Unload Picker",250, "250");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;

	case 300:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0)) {
			if( (m_nUnload1Case==300 && (m_nUnload2Case<300 || m_nUnload2Case>430)) ||
				(m_nUnload2Case==300 && (m_nUnload1Case<300 || m_nUnload1Case>430)))
			{
				if (Check_UnloadTraySort()==TRUE) {
					if (m_nUnload1Case == 300) m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 1);
					if (m_nUnload2Case == 300) m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 2);
					m_nULPickerCase = 310;
					pLogFile->Save_MCCLog("Unload Picker",300, "300");
					m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
				} else {
					gData.nCUnloadLotIndex = m_nUnloadLotIndex;
					if (gData.bContinueLotEnd[m_nUnloadLotIndex] == TRUE) {
						m_nUnloadLotCmCnt = 0;
						m_nUnloadLotIndex++;
					}
					gData.nTrayPos[3] = gData.nArrayW;
					m_pCommon->Move_Position(AX_UNLOAD_PICKER_X1, 0);
					m_pCommon->Move_Position(AX_UNLOAD_PICKER_X2, 0);
					if (m_nUnload1Case==300) m_nUnload1Case = 310;
					if (m_nUnload2Case==300) m_nUnload2Case = 310;

					m_nULPickerCase = 240;
					pLogFile->Save_MCCLog("Unload Picker",300, "300");
					m_pCommon->Set_LoopTime(AUTO_ULPICKER, 30000);
				}
			}
		}
		break;

	case 310:
		if((m_nUnload1Case == 300 && m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 1)) ||
		   (m_nUnload2Case == 300 && m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 2)) ) {
			gData.nUpNo = gData.nDownNo = 0;
			if (Check_UnloadTraySortJob()==TRUE) {
				m_nULPickerCase = 320;
				pLogFile->Save_MCCLog("Unload Picker",310, "310");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
			} else {
				m_nULPickerCase = 300;
				pLogFile->Save_MCCLog("Unload Picker",310, "310");
				m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
			}
		}
		break;
#pragma region UnloadTraySort
	case 320:	//Up위치이동
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			if (m_nUnload1Case == 300) {
				m_pCommon->PickerUnload_Move(3, Get_TrayLineConvert(gData.nUpNo), 1);
				m_dUnloadTrayY[0] = m_pCommon->m_dP2Y[0];
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0], m_pMoveData->dDUnloadTrayY1[2]);
			}
			if (m_nUnload2Case == 300) {
				m_pCommon->PickerUnload_Move(3, Get_TrayLineConvert(gData.nUpNo), 2);
				m_dUnloadTrayY[1] = m_pCommon->m_dP2Y[1];
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1], m_pMoveData->dDUnloadTrayY2[2]);
			}
			m_nULPickerCase = 330;
			pLogFile->Save_MCCLog("Unload Picker",320, "320");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 330:
		if((m_nUnload1Case == 300 && m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0])) ||
		   (m_nUnload2Case == 300 && m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1])) ) {
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 3);
			m_nULPickerCase = 340;
			pLogFile->Save_MCCLog("Unload Picker",330, "330");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 340:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 3) ) {
			Set_UnloadPickerOpen(0);	//Close
			m_nULPickerCase = 350;
			pLogFile->Save_MCCLog("Unload Picker",340, "340");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 350:
		if (Check_UnloadPickerOpen(0)==TRUE) {	//Close Check
			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 0);
			m_nULPickerCase = 360;
			pLogFile->Save_MCCLog("Unload Picker",350, "350");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 360:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			m_nULPickerCase = 370;
			pLogFile->Save_MCCLog("Unload Picker",360, "360");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 370:
//		if ((!m_pEquipData->bUseCMCheck) || (Check_UnloadPickerCM()==TRUE) ) {
		if ((gData.bUseDryRun) || (Check_UnloadPickerCM()==TRUE) ) {
			for(int i=gData.nDownX-1; i<gData.nPickCnt; i++) {
				gData.PickerInfor[3][i] = gData.GoodTrayInfo[gData.nUpNo-1][i];
				gData.GoodTrayInfo[gData.nUpNo-1][i] = 0;
			}
			m_nULPickerCase = 380;
			pLogFile->Save_MCCLog("Unload Picker",370, "370");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
		
	case 380:	//Down위치 move
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			if (m_nUnload1Case == 300) {
				m_pCommon->PickerUnload_Move(3, Get_TrayLineConvert(gData.nDownNo), 1);
				m_dUnloadTrayY[0] = m_pCommon->m_dP2Y[0];
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0], m_pMoveData->dDUnloadTrayY1[2]);
			}
			if (m_nUnload2Case == 300) {
				m_pCommon->PickerUnload_Move(3, Get_TrayLineConvert(gData.nDownNo), 2);
				m_dUnloadTrayY[1] = m_pCommon->m_dP2Y[1];
				m_pAJinAXL->Move_Abs_Accel(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1], m_pMoveData->dDUnloadTrayY2[2]);
			}
			m_nULPickerCase = 390;
			pLogFile->Save_MCCLog("Unload Picker",380, "380");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 390:
		if((m_nUnload1Case == 300 && m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y1, m_dUnloadTrayY[0])) ||
		   (m_nUnload2Case == 300 && m_pAJinAXL->Is_MoveDone(AX_UNLOAD_TRAY_Y2, m_dUnloadTrayY[1])) ) {
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 2);
			m_nULPickerCase = 400;
			pLogFile->Save_MCCLog("Unload Picker",390, "390");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 400:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 2) ) {
			Set_UnloadPickerOpen(1);	//Open
			m_nULPickerCase = 410;
			pLogFile->Save_MCCLog("Unload Picker",400, "400");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 410:
		if (Check_UnloadPickerOpen(1)==TRUE) {	//Open Check
			if (!m_pCommon->Delay_LoopTime(AUTO_ULPICKER, 100)) break;
			m_pCommon->Move_Position(AX_UNLOAD_PICKER_Z, 0);
			m_nULPickerCase = 420;
			pLogFile->Save_MCCLog("Unload Picker",410, "410");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 420:
		if (m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) ) {
			m_nULPickerCase = 430;
			pLogFile->Save_MCCLog("Unload Picker",420, "420");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		}
		break;
	case 430:
			for(int i=gData.nDownX-1; i<gData.nPickCnt; i++) {
				gData.GoodTrayInfo[gData.nDownNo-1][i] = gData.PickerInfor[3][i];
				gData.PickerInfor[3][i] = 0;
			}

			m_nULPickerCase = 300;
			pLogFile->Save_MCCLog("Unload Picker",430, "430");
			m_pCommon->Set_LoopTime(AUTO_ULPICKER, 5000);
		break;
#pragma endregion UnloadTraySort
	}

	// 9.  (Error : 8100)
	if (m_pCommon->Over_LoopTime(AUTO_ULPICKER)) {
		if (m_nULPickerCase == 100 || m_nULPickerCase == 200) return TRUE;

		if (m_nULPickerCase == 142) {
			if (m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
			if (m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
			if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#ifdef PICKER_5
			if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
			if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
#ifdef PICKER_6
			if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
			if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
			if (m_pDX5->iMUPicker6Open) gData.nPicNo = 6;
#endif
		}
		if (m_nULPickerCase == 150) {
			if (gData.IndexInfo[6][0]>0 && !m_pDX5->iMUPicker1CMCheck) gData.nPicNo = 1;
			if (gData.IndexInfo[6][1]>0 && !m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
			if (gData.IndexInfo[6][2]>0 && !m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
			if (gData.IndexInfo[6][3]>0 && !m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
			if (gData.IndexInfo[6][4]>0 && !m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
			if (gData.IndexInfo[6][5]>0 && !m_pDX5->iMUPicker6CMCheck) gData.nPicNo = 6;
		}
		if (m_nULPickerCase == 170) {
			if (gData.PickerInfor[3][0]>0 && !m_pDX5->iMUPicker1CMCheck) gData.nPicNo = 1;
			if (gData.PickerInfor[3][1]>0 && !m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
			if (gData.PickerInfor[3][2]>0 && !m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
			if (gData.PickerInfor[3][3]>0 && !m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
			if (gData.PickerInfor[3][4]>0 && !m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
			if (gData.PickerInfor[3][5]>0 && !m_pDX5->iMUPicker6CMCheck) gData.nPicNo = 6;
		}
		if (m_nULPickerCase == 220) {
			if (!m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
			if (!m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
			if (!m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#ifdef PICKER_5
			if (!m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
			if (!m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
#ifdef PICKER_6
			if (!m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
			if (!m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
			if (!m_pDX5->iMUPicker6Open) gData.nPicNo = 6;
#endif

		}
		if (m_nULPickerCase == 250) {
			if (m_pDX5->iMUPicker1CMCheck) gData.nPicNo = 1;
			if (m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
			if (m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
#ifdef PICKER_5
			if (m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
			if (m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
#endif
#ifdef PICKER_6
			if (m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
			if (m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
			if (m_pDX5->iMUPicker6CMCheck) gData.nPicNo = 6;
#endif
		}

		m_pCommon->Show_Error(8000 + m_nULPickerCase);
		return FALSE;
	} else {
		return TRUE;
	}
}

// 10. (Error : 9100)
BOOL CSequenceMain::IndexT_Run()
{
	if (m_nIndexTCase <=200) {
		if (gData.IndexJob[3] == 1 && m_pDX2->iInspCMAlign2Out) {
			m_pDY2->oInspCMAlign2In = TRUE;
			m_pDY2->oInspCMAlign2Out = FALSE;
			m_pAJinAXL->Write_Output(2);
		}
		if (gData.IndexJob[4] == 1 && m_pDX2->iInspCMAlign3Out) {
			m_pDY2->oInspCMAlign3In = TRUE;
			m_pDY2->oInspCMAlign3Out = FALSE;
			m_pAJinAXL->Write_Output(2);
		}
		if (gData.IndexJob[5] == 1 && m_pDX2->iInspCMAlign4Out) {
			m_pDY2->oInspCMAlign4In = TRUE;
			m_pDY2->oInspCMAlign4Out = FALSE;
			m_pAJinAXL->Write_Output(2);
		}
	}

	for(int i=0; i<6; i++) {
		if (gData.IndexJob[i]==1 && gLot.nLog[i]==0) {
			Set_TackLog(i);
			gLot.nLog[i]=1;
		}
	}

	switch (m_nIndexTCase) {
	case 100:
		if (Check_IndexEmpty(9)==FALSE || Check_PickerEmpty()==FALSE) {
//			if (gData.IndexJob[0] == 1) {
				m_nIndexTCase = 110;
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 30000);

				pLogFile->Save_MCCLog("Index",100, "Index Start");
//			}
		}
		break;

	case 110:
//		if (gData.IndexJob[0] == 1) {
		if (m_pCommon->Check_Run(AX_INDEX_R)==FALSE) {
			m_pDY2->oInspVacuumUp = TRUE;
			m_pDY2->oInspVacuumDown = FALSE;
			m_pAJinAXL->Write_Output(2);

			m_nIndexTCase = 120;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",110, "Inspection Vacuum Up");
		}
		break;
	case 120:
		if (m_pDX2->iInspVacuumUp && !m_pDX2->iInspVacuumDown) {
//			if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 50)) break;
			m_pDY3->oInspVacuumPad1On = TRUE;
			m_pDY3->oInspVacuumPad2On = TRUE;
			m_pDY3->oInspVacuumPad3On = TRUE;
			m_pDY3->oInspVacuumPad4On = TRUE;
			m_pDY3->oInspVacuumPad5On = TRUE;
			m_pDY3->oInspVacuumPad6On = TRUE;
			m_pAJinAXL->Write_Output(3);

			m_nIndexTCase = 130;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",120, "Vacuum On");
		}
		break;

	case 130:
			m_nIndexTCase = 131;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 60000);
		break;
	case 131:
		if (gData.IndexJob[0] == 1 || (gData.bCleanOutMode == TRUE) ) 
		{

			m_pDY3->oInspVacuumPad1On = FALSE;
			m_pDY3->oInspVacuumPad2On = FALSE;
			m_pDY3->oInspVacuumPad3On = FALSE;
			m_pDY3->oInspVacuumPad4On = FALSE;
			m_pDY3->oInspVacuumPad5On = FALSE;
			m_pDY3->oInspVacuumPad6On = FALSE;
			m_pAJinAXL->Write_Output(3);
			if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 200)) break;

			m_pDY2->oInspCMAlign1In = TRUE;
			m_pDY2->oInspCMAlign1Out = FALSE;
			m_pAJinAXL->Write_Output(2);

			m_nIndexTCase = 140;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 3000);

			pLogFile->Save_MCCLog("Index",131, "Inspection Aligner In");
		}
		break;
	case 140:
		if (m_pDX2->iInspCMAlign1In && !m_pDX2->iInspCMAlign1Out) {
//			if((!m_pEquipData->bUseVacuumCheck) ||
			if((gData.bUseDryRun) ||
			   (((gData.IndexInfo[0][0]>0 && m_pDX3->iInspVacuumPad1Check) || gData.IndexInfo[0][0]==0) &&
				((gData.IndexInfo[0][1]>0 && m_pDX3->iInspVacuumPad2Check) || gData.IndexInfo[0][1]==0) &&
				((gData.IndexInfo[0][2]>0 && m_pDX3->iInspVacuumPad3Check) || gData.IndexInfo[0][2]==0) &&
				((gData.IndexInfo[0][3]>0 && m_pDX3->iInspVacuumPad4Check) || gData.IndexInfo[0][3]==0) &&
				((gData.IndexInfo[0][4]>0 && m_pDX3->iInspVacuumPad5Check) || gData.IndexInfo[0][4]==0) &&
				((gData.IndexInfo[0][5]>0 && m_pDX3->iInspVacuumPad6Check) || gData.IndexInfo[0][5]==0))) {

				m_nIndexTCase = 150;
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

				pLogFile->Save_MCCLog("Index",140, "Vacuum Off");
			}
		}
		break;

	case 141:
			m_pDY2->oInspCMAlign1In = FALSE;
			m_pDY2->oInspCMAlign1Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nIndexTCase = 142;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",141, "Inspection Aligner Out");
		break;
	case 142:
		if (!m_pDX2->iInspCMAlign1In && m_pDX2->iInspCMAlign1Out) {

			m_pDY3->oInspVacuumPad1On = TRUE;
			m_pDY3->oInspVacuumPad2On = TRUE;
			m_pDY3->oInspVacuumPad3On = TRUE;
			m_pDY3->oInspVacuumPad4On = TRUE;
			m_pDY3->oInspVacuumPad5On = TRUE;
			m_pDY3->oInspVacuumPad6On = TRUE;
			m_pAJinAXL->Write_Output(3);

			if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 1000)) break;
			m_pDY2->oInspCMAlign1In = TRUE;
			m_pDY2->oInspCMAlign1Out = FALSE;
			m_pAJinAXL->Write_Output(2);
			m_nIndexTCase = 143;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",142, "Inspection Aligner In");
		}
		break;
	case 143:
		if (m_pDX2->iInspCMAlign1In && !m_pDX2->iInspCMAlign1Out) {
//			if((!m_pEquipData->bUseVacuumCheck) ||
			if((gData.bUseDryRun) ||
			   (((gData.IndexInfo[0][0]>0 && m_pDX3->iInspVacuumPad1Check) || gData.IndexInfo[0][0]==0) &&
				((gData.IndexInfo[0][1]>0 && m_pDX3->iInspVacuumPad2Check) || gData.IndexInfo[0][1]==0) &&
				((gData.IndexInfo[0][2]>0 && m_pDX3->iInspVacuumPad3Check) || gData.IndexInfo[0][2]==0) &&
				((gData.IndexInfo[0][3]>0 && m_pDX3->iInspVacuumPad4Check) || gData.IndexInfo[0][3]==0) &&
				((gData.IndexInfo[0][4]>0 && m_pDX3->iInspVacuumPad5Check) || gData.IndexInfo[0][4]==0) &&
				((gData.IndexInfo[0][5]>0 && m_pDX3->iInspVacuumPad6Check) || gData.IndexInfo[0][5]==0))) {

				m_pDY3->oInspVacuumPad1On = FALSE;
				m_pDY3->oInspVacuumPad2On = FALSE;
				m_pDY3->oInspVacuumPad3On = FALSE;
				m_pDY3->oInspVacuumPad4On = FALSE;
				m_pDY3->oInspVacuumPad5On = FALSE;
				m_pDY3->oInspVacuumPad6On = FALSE;
				m_pAJinAXL->Write_Output(3);

				m_nIndexTCase = 150;
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

				pLogFile->Save_MCCLog("Index",143, "Inspection Vacuum Off");
			}
		}
		break;

	case 150:
//		if((!m_pEquipData->bUseVacuumCheck) ||
//		if((gData.bUseDryRun) ||
//		   (!m_pDX3->iInspVacuumPad1Check && !m_pDX3->iInspVacuumPad2Check && !m_pDX3->iInspVacuumPad3Check &&
//			!m_pDX3->iInspVacuumPad4Check && !m_pDX3->iInspVacuumPad5Check && !m_pDX3->iInspVacuumPad6Check)) {
			m_pDY2->oInspVacuumUp = FALSE;
			m_pDY2->oInspVacuumDown = TRUE;
			m_pAJinAXL->Write_Output(2);

			m_nIndexTCase = 160;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 30000);

			pLogFile->Save_MCCLog("Index",150, "Inspection Vacuum Down");
//		}
		break;
	case 160:
		if((!m_pDX2->iInspVacuumUp && m_pDX2->iInspVacuumDown) &&
		   (gData.bUseNGVacuum==FALSE || (!m_pDX5->iNGVacuumUp   && m_pDX5->iNGVacuumDown)) &&
		   (gData.bUseGDVacuum==FALSE || (!m_pDX5->iGoodVacuumUp && m_pDX5->iGoodVacuumDown)) ) {
			m_nIndexTCase = 200;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 90000);

			pLogFile->Save_MCCLog("Index",160, "Inspection Vacuum Down");
		}
		break;

	case 200:
		if ((gData.IndexJob[0] == 1 && m_pDX2->iInspCMAlign1In) &&
			(gData.IndexJob[1] == 1) &&
			(gData.IndexJob[2] == 1) &&
			(gData.IndexJob[3] == 1 && m_pDX2->iInspCMAlign2In) &&
			(gData.IndexJob[4] == 1 && m_pDX2->iInspCMAlign3In) &&
			(gData.IndexJob[5] == 1 && m_pDX2->iInspCMAlign4In) ) {
			m_nIndexTCase = 210;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 10000);
			pLogFile->Save_MCCLog("Index",200, "Jobs Done Checked And Aligner In");
		}
		break;
	case 210:
		if ((gData.IndexJob[0] == 1 && m_pDX2->iInspCMAlign1In) &&
			(gData.IndexJob[1] == 1) &&
			(gData.IndexJob[2] == 1) &&
			(gData.IndexJob[3] == 1 && m_pDX2->iInspCMAlign2In) &&
			(gData.IndexJob[4] == 1 && m_pDX2->iInspCMAlign3In) &&
			(gData.IndexJob[5] == 1 && m_pDX2->iInspCMAlign4In) ) {
			if ((!m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 2)   || (m_pCommon->Check_Position(AX_LOAD_PICKER_Y1, 2) && m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0))) &&
				(!m_pCommon->Check_Position(AX_NG_PICKER_X, 0)      || (m_pCommon->Check_Position(AX_NG_PICKER_X, 0) && Check_NGPickerAllUp()==TRUE)) &&
				(Check_GdPickerAllUp()==TRUE) &&
				(!m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 0) || (m_pCommon->Check_Position(AX_UNLOAD_PICKER_X1, 0) && m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0))) ) {
				if((!m_pDX2->iInspVacuumUp && m_pDX2->iInspVacuumDown) &&
				   (gData.bUseNGVacuum==FALSE || (!m_pDX5->iNGVacuumUp   && m_pDX5->iNGVacuumDown)) &&
				   (gData.bUseGDVacuum==FALSE || (!m_pDX5->iGoodVacuumUp && m_pDX5->iGoodVacuumDown)) ) {
					if ((gData.bUseCMPress && m_pDX2->iCMPressUp && !m_pDX2->iCMPressDn) || !gData.bUseCMPress) {
						m_nIndexTCase = 211;
						m_pCommon->Set_LoopTime(AUTO_INDEXT, 30000);

						pLogFile->Save_MCCLog("Index",210, "Good Picker All Up Checked and Each Move Done");
					}
				}
			}
		}
		break;
	case 211:
			m_pCommon->Move_Position(AX_INDEX_R, 0);
			m_nIndexTCase = 220;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 10000);

			pLogFile->Save_MCCLog("Index",211, "Index Rotation");
		break;

	case 220:
		if (m_pCommon->Check_Position(AX_INDEX_R, 0) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 300)) break;
			m_pDY2->oInspCMAlign1In = FALSE;
			m_pDY2->oInspCMAlign1Out = TRUE;
//			m_pDY2->oInspCMAlign2In = FALSE;
//			m_pDY2->oInspCMAlign2Out = TRUE;
//			m_pDY2->oInspCMAlign3In = FALSE;
//			m_pDY2->oInspCMAlign3Out = TRUE;
//			m_pDY2->oInspCMAlign4In = FALSE;
//			m_pDY2->oInspCMAlign4Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nIndexTCase = 230;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",220, "Rotation Done and Aligner Out");
		}
		break;
	case 230:
		if (!m_pDX2->iInspCMAlign1In && m_pDX2->iInspCMAlign1Out ) {
//		if (!m_pDX2->iInspCMAlign1In && m_pDX2->iInspCMAlign1Out &&
//			!m_pDX2->iInspCMAlign2In && m_pDX2->iInspCMAlign2Out &&
//			!m_pDX2->iInspCMAlign3In && m_pDX2->iInspCMAlign3Out &&
//			!m_pDX2->iInspCMAlign4In && m_pDX2->iInspCMAlign4Out ) {
			Set_IndexEnd();
			m_nIndexTCase = 100;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",230, "Set_IndexEnd");
		}
		break;

	case 300:
		{
			double dIndexR = -1.0;
			AXIS_STATUS *pStatusR = m_pAJinAXL->Get_pStatus(AX_INDEX_R); dIndexR = pStatusR->dPos;
			if (m_pCommon->Check_Position(AX_INDEX_R, 0) || (dIndexR == 0.0)) {
				if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 500)) break;
				m_pDY2->oInspCMAlign1In = TRUE;
				m_pDY2->oInspCMAlign1Out = FALSE;
				m_pDY2->oInspCMAlign2In = TRUE;
				m_pDY2->oInspCMAlign2Out = FALSE;
				m_pDY2->oInspCMAlign3In = TRUE;
				m_pDY2->oInspCMAlign3Out = FALSE;
				m_pDY2->oInspCMAlign4In = TRUE;
				m_pDY2->oInspCMAlign4Out = FALSE;
				m_pAJinAXL->Write_Output(2);
				m_nIndexTCase = 310;
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

				pLogFile->Save_MCCLog("Index",300, "Inspection Aligner In");
			}
		}
		break;
	case 310:
		if (m_pDX2->iInspCMAlign1In && !m_pDX2->iInspCMAlign1Out && m_pDX2->iInspCMAlign2In && !m_pDX2->iInspCMAlign2Out && 
			m_pDX2->iInspCMAlign3In && !m_pDX2->iInspCMAlign3Out && m_pDX2->iInspCMAlign4In && !m_pDX2->iInspCMAlign4Out ) {
			m_nIndexTCase = 320;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",310, "Aligner In Checked");
		}
		break;
	case 320:
		if (m_pCommon->Check_Position(AX_LOAD_PICKER_Z, 0) && (m_pCommon->Check_Position(AX_NG_PICKER_Z, 0) || m_pCommon->Check_Position(AX_NG_PICKER_Z, 1)) &&
			m_pCommon->Check_Position(AX_GOOD_PICKER_Z, 0) && m_pCommon->Check_Position(AX_UNLOAD_PICKER_Z, 0) &&
			(m_pCommon->Check_Position(AX_INSPECTION_Z, 0) || m_pCommon->Check_Position(AX_INSPECTION_Z, 1)) ) {
			if ((gData.bUseCMPress && m_pDX2->iCMPressUp && !m_pDX2->iCMPressDn) || !gData.bUseCMPress) {
				m_pAJinAXL->Home_Search(AX_INDEX_R);
				m_nIndexTCase = 330;
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 30000);
				pLogFile->Save_MCCLog("Index",320, "Home Search");
			}
		}
		break;
	case 330:
		if (m_pAJinAXL->Is_Home(AX_INDEX_R) ) {
			if (!m_pCommon->Delay_LoopTime(AUTO_INDEXT, 500)) break;
			m_pDY2->oInspCMAlign1In = FALSE;
			m_pDY2->oInspCMAlign1Out = TRUE;
			m_pAJinAXL->Write_Output(2);
			m_nIndexTCase = 340;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",330, "Home Search");
		}
		break;
	case 340:
		if (!m_pDX2->iInspCMAlign1In && m_pDX2->iInspCMAlign1Out ) {
			m_nIndexTCase = 100;
			m_pCommon->Set_LoopTime(AUTO_INDEXT, 5000);

			pLogFile->Save_MCCLog("Index",340, "340");
		}
		break;

	}

	// 10. (Error : 9100)
	if (m_pCommon->Over_LoopTime(AUTO_INDEXT)) {
		if (m_nIndexTCase == 100) return TRUE;

		if (m_nIndexTCase == 140) {	//Vacuum Retray
			m_nIndexTCase = 141;
			return TRUE;
		}
		if (m_nIndexTCase == 143) {
			gData.nPicNo = 0;
			if (gData.IndexInfo[0][0]>0 && !m_pDX3->iInspVacuumPad1Check) gData.nPicNo = 1;
			if (gData.IndexInfo[0][1]>0 && !m_pDX3->iInspVacuumPad2Check) gData.nPicNo = 2;
			if (gData.IndexInfo[0][2]>0 && !m_pDX3->iInspVacuumPad3Check) gData.nPicNo = 3;
			if (gData.IndexInfo[0][3]>0 && !m_pDX3->iInspVacuumPad4Check) gData.nPicNo = 4;
			if (gData.IndexInfo[0][4]>0 && !m_pDX3->iInspVacuumPad5Check) gData.nPicNo = 5;
			if (gData.IndexInfo[0][5]>0 && !m_pDX3->iInspVacuumPad6Check) gData.nPicNo = 6;
		}
		if (m_nIndexTCase == 200) {
			gData.nPicNo = 0;
			if (!m_pDX2->iInspCMAlign1In) gData.nPicNo = 11;
			if (!m_pDX2->iInspCMAlign2In) gData.nPicNo = 12;
			if (!m_pDX2->iInspCMAlign3In) gData.nPicNo = 13;
			if (!m_pDX2->iInspCMAlign4In) gData.nPicNo = 14;
			if (gData.IndexJob[0] == 0) gData.nPicNo = 1;
			if (gData.IndexJob[1] == 0)	gData.nPicNo = 2;
			if (gData.IndexJob[2] == 0)	gData.nPicNo = 3;
			if (gData.IndexJob[3] == 0)	gData.nPicNo = 4;
			if (gData.IndexJob[4] == 0)	gData.nPicNo = 5;
			if (gData.IndexJob[5] == 0)	gData.nPicNo = 6;
			if (gData.nPicNo == 0) {
				m_pCommon->Set_LoopTime(AUTO_INDEXT, 60000);

				CString sLog;
			
				sLog.Format("[Sequence] IndexT_Run...  Seq[%d] LotID[%s] Data[%d]", m_nIndexTCase, gData.sLotID, gData.nPicNo);
				pLogFile->Save_HandlerLog(sLog);	
				return TRUE;
			}
		}

		m_pCommon->Show_Error(9000 + m_nIndexTCase);
		return FALSE;
	} else {
		return TRUE;
	}
}
// 11. (Error : 9600)
BOOL CSequenceMain::NGTray_Run()
{
//	if (gData.bCleanOutMode == TRUE && Check_LotEnd()==TRUE && Check_NGPickerAllUp()==TRUE && gData.nNGTrayPos != 3) {
//		m_pCommon->Move_Position(AX_NG_STAGE_Y, 0);
//		gData.nNGTrayPos = 3;
//	}
	if (m_nNGTrayCase < 290 && gData.nStatus>=2 && gData.nDoorInterlock18 != 1) {
		if (m_pAJinAXL->Is_Done(AX_NG_STAGE_Y) && Check_NGTrayJobEnd()==TRUE) {
			gData.nStatus = 3;	//NG Tray 후처리
			m_nNGTrayCase = 290;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 10000);		
			return TRUE;
		}
	}

	switch (m_nNGTrayCase) {
	case 100:
		if ((Check_NGTrayEmpty(1) == TRUE || Check_NGTrayEmpty(2) == TRUE) && gData.nDoorInterlock18 != 1 &&
			m_pDX3->iNGTray1Check && m_pDX3->iNGTray2Check && Check_NGPickerAllUp()==TRUE ) {
			m_nNGTrayCase = 110;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;
	case 110:
		if (Check_NGPickerAllUp()==TRUE) {
			m_pCommon->Move_Position(AX_NG_STAGE_Y, 1);
			gData.nNGTrayPos = 3;
			m_nNGTrayCase = 120;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 120:
		if (m_pCommon->Check_Position(AX_NG_STAGE_Y, 1)) {
			if (gData.bUseSeparationMode) {
				if (Check_NGTrayEmpty(1)) gData.nTrayPos[1] = 1;
				if (Check_NGTrayEmpty(2)) gData.nTrayPos[2] = 1;

				if ((!Check_NGTrayEmpty(2) || !Check_NGTrayEmpty(1)) &&  !gData.bTrayChangeClicked) break;
				gData.bTrayChangeClicked = FALSE;
				m_nNGTrayCase = 140;
			} else {
				gData.nTrayPos[1] = 1;
				m_nNGTrayCase = 150;
			}
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;
		// 분리 배출 대기
	case 140:
		m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		break;

	case 141:	// NG Tray1 Moving
		if (Check_NGPickerAllUp()==TRUE) {
			gData.nTrayPos[1]++;
			if (gData.nTrayPos[1] > gData.nArrayW) {
				gData.nTrayPos[1] = 0;
				m_pCommon->Move_Position(AX_NG_STAGE_Y, 0);
				gData.nNGTrayPos = 3;
				m_nNGTrayCase = 310;
			} else {
				m_pCommon->PickerNGStageMove(gData.nTrayPos[1], 1);
				m_dNGStageY = m_pCommon->m_dP3Y;

				//m_dNGStageY = m_pMoveData->dNGStageY[1] + m_pMoveData->dNGStageY[3] - ((gData.nTrayPos[1]-1) * gData.dTrayPitchW);
				//m_pAJinAXL->Move_Absolute(AX_NG_STAGE_Y, m_dNGStageY);
				m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

				m_nNGTrayCase = 142;
			}
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 142:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_STAGE_Y, m_dNGStageY) ) {
			m_nNGTrayCase = 140;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;

	case 143:	// NG Tray2 Moving
		if (Check_NGPickerAllUp()==TRUE) {
			gData.nTrayPos[2]++;
			if (gData.nTrayPos[2] > gData.nArrayW) {
				gData.nTrayPos[2] = 0;
				m_pCommon->Move_Position(AX_NG_STAGE_Y, 0);
				gData.nNGTrayPos = 3;
				m_nNGTrayCase = 310;
			} else {
				m_pCommon->PickerNGStageMove(gData.nTrayPos[2], 2);
				m_dNGStageY = m_pCommon->m_dP3Y;

				//m_dNGStageY = m_pMoveData->dNGStageY[2] + m_pMoveData->dNGStageY[3] + ((gData.nTrayPos[2]-1) * gData.dTrayPitchW);
				//m_pAJinAXL->Move_Absolute(AX_NG_STAGE_Y, m_dNGStageY);
				m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

				m_nNGTrayCase = 144;
			}
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 144:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_STAGE_Y, m_dNGStageY) ) {
			m_nNGTrayCase = 140;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;

		// 순차 배출 대기
	case 150:
			gData.nNGTrayPos = 1;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		break;
	case 160:
		if (Check_NGPickerAllUp()==TRUE) {
			gData.nTrayPos[1]++;
			if (gData.nTrayPos[1] > gData.nArrayW) {
				gData.nTrayPos[1] = 0;
				m_pCommon->Move_Position(AX_NG_STAGE_Y, 2);
				gData.nNGTrayPos = 3;
				m_nNGTrayCase = 220;
			} else {
				m_pCommon->PickerNGStageMove(gData.nTrayPos[1], 1);
				m_dNGStageY = m_pCommon->m_dP3Y;

				//m_dNGStageY = m_pMoveData->dNGStageY[1] + m_pMoveData->dNGStageY[3] - ((gData.nTrayPos[1]-1) * gData.dTrayPitchW);
				//m_pAJinAXL->Move_Absolute(AX_NG_STAGE_Y, m_dNGStageY);
				m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

				m_nNGTrayCase = 170;
			}
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 170:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_STAGE_Y, m_dNGStageY) ) {
			m_nNGTrayCase = 150;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;

	case 220:
		if (m_pCommon->Check_Position(AX_NG_STAGE_Y, 2)) {
			gData.nTrayPos[2] = 1;
			m_nNGTrayCase = 250;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;
	case 250:
			gData.nNGTrayPos = 2;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		break;
	case 260:
		if (Check_NGPickerAllUp()==TRUE) {
			gData.nTrayPos[2]++;
			if (gData.nTrayPos[2] > gData.nArrayW) {
				gData.nTrayPos[2] = 0;
				m_pCommon->Move_Position(AX_NG_STAGE_Y, 0);
				gData.nNGTrayPos = 3;
				m_nNGTrayCase = 310;
			} else {
				m_pCommon->PickerNGStageMove(gData.nTrayPos[2], 2);
				m_dNGStageY = m_pCommon->m_dP3Y;

				//m_dNGStageY = m_pMoveData->dNGStageY[2] + m_pMoveData->dNGStageY[3] + ((gData.nTrayPos[2]-1) * gData.dTrayPitchW);
				//m_pAJinAXL->Move_Absolute(AX_NG_STAGE_Y, m_dNGStageY);
				m_pAJinAXL->Move_Abs_Accel(AX_NG_STAGE_Y, m_dNGStageY, m_pMoveData->dDNGStageY[1]);

				m_nNGTrayCase = 270;
			}
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 270:
		if (m_pAJinAXL->Is_MoveDone(AX_NG_STAGE_Y, m_dNGStageY) ) {
			m_nNGTrayCase = 250;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;

	case 290:
		if (m_pAJinAXL->Is_Done(AX_NG_STAGE_Y) && Check_NGPickerAllUp()==TRUE) {
			m_pCommon->Move_Position(AX_NG_STAGE_Y, 0);
			gData.nNGTrayPos = 3;
			m_nNGTrayCase = 300;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 30000);
		}
		break;
	case 300:
		if (m_pCommon->Check_Position(AX_NG_STAGE_Y, 0)) {
			gData.nNGTrayPos = 0;
			if (gData.nDoorInterlock18==0) Beep_Post(500);
			gData.nDoorInterlock18 = 1;
			m_nNGTrayCase = 100;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 10000);
		}
		break;

	case 310:
		if (m_pCommon->Check_Position(AX_NG_STAGE_Y, 0)) {
			gData.nNGTrayPos = 0;
			if (Check_NGTrayJobEnd()==TRUE) 
			{
				m_nNGTrayCase = 290; gData.nStatus = 3;
			}
			else							{ m_nNGTrayCase = 320; }
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 1000);
		}
		break;
	case 320:	// Alarm 발생
		if (m_pCommon->Check_Position(AX_NG_STAGE_Y, 1)) {
			m_nNGTrayCase = 320;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
		}
		break;

	}

	// 11. (Error : 9600)
	if (m_pCommon->Over_LoopTime(AUTO_NGTRAY)) {
		if (m_nNGTrayCase == 100 || m_nNGTrayCase == 150 || m_nNGTrayCase == 250) return TRUE;
		if (m_nNGTrayCase == 300) {
			m_nNGTrayCase = 290;
			m_pCommon->Set_LoopTime(AUTO_NGTRAY, 5000);
			return TRUE;
		}

		if (m_nNGTrayCase == 320) {
			m_pCommon->Show_Error(9500);
		} else {
			m_pCommon->Show_Error(9500 + m_nNGTrayCase);
		}
		return FALSE;
	} else {
		return TRUE;
	}
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSequenceMain::Check_NGPickerAllUp()
{
#ifdef NG_PICKER_3
	if ( m_pDX3->iNGPicker1Up   &&  m_pDX3->iNGPicker2Up   &&  m_pDX3->iNGPicker3Up   &&
		!m_pDX3->iNGPicker1Down && !m_pDX3->iNGPicker2Down && !m_pDX3->iNGPicker3Down ) {
			return TRUE;
	}
#else
	if ( m_pDX3->iNGPicker1Up   &&  m_pDX3->iNGPicker2Up   &&  m_pDX3->iNGPicker3Up   &&  m_pDX3->iNGPicker4Up   &&  m_pDX3->iNGPicker5Up && 
		!m_pDX3->iNGPicker1Down && !m_pDX3->iNGPicker2Down && !m_pDX3->iNGPicker3Down && !m_pDX3->iNGPicker4Down && !m_pDX3->iNGPicker5Down ) {
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CSequenceMain::Check_GdPickerAllUp()
{
#ifdef GD_PICKER_3
	if (m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker3Up && 
		!m_pDX4->iGoodPicker1Down && !m_pDX4->iGoodPicker2Down && !m_pDX4->iGoodPicker3Down ) {
		return TRUE;
	}
#endif
#ifdef GD_PICKER_5
	if (m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker3Up && 
		m_pDX4->iGoodPicker4Up && m_pDX4->iGoodPicker5Up &&
		!m_pDX4->iGoodPicker1Down && !m_pDX4->iGoodPicker2Down && !m_pDX4->iGoodPicker3Down && 
		!m_pDX4->iGoodPicker4Down && !m_pDX4->iGoodPicker5Down ) {
		return TRUE;
	}
#endif
#ifdef GD_PICKER_6
	if (m_pDX4->iGoodPicker1Up && m_pDX4->iGoodPicker2Up && m_pDX4->iGoodPicker3Up && 
		m_pDX4->iGoodPicker4Up && m_pDX4->iGoodPicker5Up && m_pDX4->iGoodPicker6Up &&
		!m_pDX4->iGoodPicker1Down && !m_pDX4->iGoodPicker2Down && !m_pDX4->iGoodPicker3Down && 
		!m_pDX4->iGoodPicker4Down && !m_pDX4->iGoodPicker5Down && !m_pDX4->iGoodPicker6Down) {
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CSequenceMain::Check_UnloadPickerOpen(int nCmd)
{
	if (nCmd==1) {	// Open
#ifdef PICKER_3
		if (m_pDX5->iMUPicker1Open && m_pDX5->iMUPicker2Open && m_pDX5->iMUPicker3Open) return TRUE;
		if (!m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
		if (!m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#else
		if (m_pDX5->iMUPicker1Open && m_pDX5->iMUPicker2Open && m_pDX5->iMUPicker3Open && m_pDX5->iMUPicker4Open && m_pDX5->iMUPicker5Open) return TRUE;
		if (!m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
		if (!m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
		if (!m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
		if (!m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
		return FALSE;
	}

	// Close
	if (gData.nDownX==1) {
#ifdef PICKER_3
		if (!m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open) return TRUE;
		if (m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
		if (m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#else
		if (!m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open && !m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open) return TRUE;
		if (m_pDX5->iMUPicker1Open) gData.nPicNo = 1;
		if (m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
		if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
		if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==2) {
#ifdef PICKER_3
		if ( m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open) return TRUE;
		if (m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#else
		if ( m_pDX5->iMUPicker1Open && !m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open && !m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open) return TRUE;
		if (m_pDX5->iMUPicker2Open) gData.nPicNo = 2;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
		if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
		if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==3) {
#ifdef PICKER_3
		if ( m_pDX5->iMUPicker1Open &&  m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open) return TRUE;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
#else
		if ( m_pDX5->iMUPicker1Open &&  m_pDX5->iMUPicker2Open && !m_pDX5->iMUPicker3Open && !m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open) return TRUE;
		if (m_pDX5->iMUPicker3Open) gData.nPicNo = 3;
		if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
		if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==4) {
		if ( m_pDX5->iMUPicker1Open &&  m_pDX5->iMUPicker2Open &&  m_pDX5->iMUPicker3Open && !m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open) return TRUE;
		if (m_pDX5->iMUPicker4Open) gData.nPicNo = 4;
		if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
	}
	if (gData.nDownX==5) {
		if ( m_pDX5->iMUPicker1Open &&  m_pDX5->iMUPicker2Open &&  m_pDX5->iMUPicker3Open &&  m_pDX5->iMUPicker4Open && !m_pDX5->iMUPicker5Open) return TRUE;
		if (m_pDX5->iMUPicker5Open) gData.nPicNo = 5;
	}

	return FALSE;
}

BOOL CSequenceMain::Check_UnloadPickerCM()
{
#ifdef PICKER_3
	if(((gData.GoodTrayInfo[gData.nUpNo-1][0]>0 && m_pDX5->iMUPicker1CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][0]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][1]>0 && m_pDX5->iMUPicker2CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][1]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][2]>0 && m_pDX5->iMUPicker3CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][2]==0) ) return TRUE;
#else
	if(((gData.GoodTrayInfo[gData.nUpNo-1][0]>0 && m_pDX5->iMUPicker1CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][0]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][1]>0 && m_pDX5->iMUPicker2CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][1]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][2]>0 && m_pDX5->iMUPicker3CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][2]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][3]>0 && m_pDX5->iMUPicker4CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][3]==0) &&
	   ((gData.GoodTrayInfo[gData.nUpNo-1][4]>0 && m_pDX5->iMUPicker5CMCheck) || gData.GoodTrayInfo[gData.nUpNo-1][4]==0) ) return TRUE;
#endif

	if (gData.nDownX==1) {
#ifdef PICKER_3
		if (m_pDX5->iMUPicker1CMCheck && m_pDX5->iMUPicker2CMCheck && m_pDX5->iMUPicker3CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker1CMCheck) gData.nPicNo = 1;
		if (!m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
#else
		if (m_pDX5->iMUPicker1CMCheck && m_pDX5->iMUPicker2CMCheck && m_pDX5->iMUPicker3CMCheck && m_pDX5->iMUPicker4CMCheck && m_pDX5->iMUPicker5CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker1CMCheck) gData.nPicNo = 1;
		if (!m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
		if (!m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
		if (!m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==2) {
#ifdef PICKER_3
		if (m_pDX5->iMUPicker2CMCheck && m_pDX5->iMUPicker3CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
#else
		if (m_pDX5->iMUPicker2CMCheck && m_pDX5->iMUPicker3CMCheck && m_pDX5->iMUPicker4CMCheck && m_pDX5->iMUPicker5CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker2CMCheck) gData.nPicNo = 2;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
		if (!m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
		if (!m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==3) {
#ifdef PICKER_3
		if (m_pDX5->iMUPicker3CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
#else
		if (m_pDX5->iMUPicker3CMCheck && m_pDX5->iMUPicker4CMCheck && m_pDX5->iMUPicker5CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker3CMCheck) gData.nPicNo = 3;
		if (!m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
		if (!m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
#endif
	}
	if (gData.nDownX==4) {
		if (m_pDX5->iMUPicker4CMCheck && m_pDX5->iMUPicker5CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker4CMCheck) gData.nPicNo = 4;
		if (!m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
	}
	if (gData.nDownX==5) {
		if (m_pDX5->iMUPicker5CMCheck) return TRUE;
		if (!m_pDX5->iMUPicker5CMCheck) gData.nPicNo = 5;
	}
	return FALSE;
}

void CSequenceMain::Set_UnloadPickerOpen(int nCmd)
{
	BOOL bOpne, bClose;
	if (nCmd==0) {	//Close
		bOpne=FALSE;
		bClose=TRUE;
	}
	if (nCmd==1) {	//Open
		bOpne=TRUE;
		bClose=FALSE;
	}

	if (gData.nDownX==1) {
		m_pDY5->oMUPicker1Open = bOpne;
		m_pDY5->oMUPicker1Close = bClose;
		m_pDY5->oMUPicker2Open = bOpne;
		m_pDY5->oMUPicker2Close = bClose;
		m_pDY5->oMUPicker3Open = bOpne;
		m_pDY5->oMUPicker3Close = bClose;
#ifdef PICKER_5
		m_pDY5->oMUPicker4Open = bOpne;
		m_pDY5->oMUPicker4Close = bClose;
		m_pDY5->oMUPicker5Open = bOpne;
		m_pDY5->oMUPicker5Close = bClose;
#endif
		m_pAJinAXL->Write_Output(5);
	}
	if (gData.nDownX==2) {
		m_pDY5->oMUPicker2Open = bOpne;
		m_pDY5->oMUPicker2Close = bClose;
		m_pDY5->oMUPicker3Open = bOpne;
		m_pDY5->oMUPicker3Close = bClose;
#ifdef PICKER_5
		m_pDY5->oMUPicker4Open = bOpne;
		m_pDY5->oMUPicker4Close = bClose;
		m_pDY5->oMUPicker5Open = bOpne;
		m_pDY5->oMUPicker5Close = bClose;
#endif
		m_pAJinAXL->Write_Output(5);
	}
	if (gData.nDownX==3) {
		m_pDY5->oMUPicker3Open = bOpne;
		m_pDY5->oMUPicker3Close = bClose;
#ifdef PICKER_5
		m_pDY5->oMUPicker4Open = bOpne;
		m_pDY5->oMUPicker4Close = bClose;
		m_pDY5->oMUPicker5Open = bOpne;
		m_pDY5->oMUPicker5Close = bClose;
#endif
		m_pAJinAXL->Write_Output(5);
	}
	if (gData.nDownX==4) {
		m_pDY5->oMUPicker4Open = bOpne;
		m_pDY5->oMUPicker4Close = bClose;
		m_pDY5->oMUPicker5Open = bOpne;
		m_pDY5->oMUPicker5Close = bClose;
		m_pAJinAXL->Write_Output(5);
	}
	if (gData.nDownX==5) {
		m_pDY5->oMUPicker5Open = bOpne;
		m_pDY5->oMUPicker5Close = bClose;
		m_pAJinAXL->Write_Output(5);
	}
}

BOOL CSequenceMain::Check_NGTrayEmpty(int nNo)
{
	int nCMCnt = 0;
	for(int w=0; w<gData.nArrayW; w++) {
		for(int l=0; l<gData.nArrayL; l++) {

			if (nNo == 0 || nNo == 1) { if (gData.NG1TrayInfo[w][l] > 0) return FALSE; }
			if (nNo == 0 || nNo == 2) { if (gData.NG2TrayInfo[w][l] > 0) return FALSE; }

			nCMCnt++;
			if(nCMCnt == gData.nCMUseCount) break;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_NGTrayFull(int nNo)
{
#ifdef MODEL_SC
	int nCMCnt = 0;
	int nTrayX = gData.nArrayL - 1;
	int nTrayY = gData.nArrayW - 1;
	int nSETrayYEnd = gData.nArrayW / 2 - 2;	// F2122 5*10 1~4Line: Shiny Edge, 6~10Line: Lens Fiducial

	if (nNo == 0 || nNo == 2) {
		if (gData.NG2TrayInfo[nSETrayYEnd][nTrayX] > 0) return TRUE;	// Shiny Edge		1~4, 1
		if (gData.NG2TrayInfo[nTrayY     ][nTrayX] > 0) return TRUE;	// Lens Fiducial	6~10, 3~4
	}
	if (nNo == 0 || nNo == 1) {		
		if (gData.NG1TrayInfo[nTrayY][nTrayX] > 0) return TRUE;		// 렌즈배럴, 기타불량 
	}

	return FALSE;
#else
	int nCMCnt = 0;
	for(int w=0; w<gData.nArrayW; w++) {
		for(int l=0; l<gData.nArrayL; l++) {

			if (nNo == 0 || nNo == 1) { if (gData.NG1TrayInfo[w][l] == 0) return FALSE; }
			if (nNo == 0 || nNo == 2) { if (gData.NG2TrayInfo[w][l] == 0) return FALSE; }

			nCMCnt++;
			if(nCMCnt == gData.nCMUseCount) break;
		}
	}
#endif
	return TRUE;
}

BOOL CSequenceMain::Check_NGTrayMoving()
{
	if (gData.nNGTrayPos != 1 && gData.nNGTrayPos != 2) return FALSE;

	int nYY = gData.nTrayPos[gData.nNGTrayPos];
	if (nYY < 1 || nYY>gData.nArrayW) return FALSE;

	for(int i=0; i<gData.nPickCnt; i++) {
		if((gData.nNGTrayPos == 1 && gData.NG1TrayInfo[nYY-1][i] == 0) ||
		   (gData.nNGTrayPos == 2 && gData.NG2TrayInfo[nYY-1][i] == 0) ) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_GoodTrayEmpty(int nLine)
{
	int nCMCnt = 0;

	if (nLine > 0 && nLine <= gData.nArrayW) {
		for(int l=0; l<gData.nArrayL; l++) {
			if (gData.GoodTrayInfo[nLine-1][l] > 0) return FALSE;
		}
		return TRUE;
	}

	for(int w=0; w<gData.nArrayW; w++) {
		for(int l=0; l<gData.nArrayL; l++) {

			if (gData.GoodTrayInfo[w][l] > 0) return FALSE;

			nCMCnt++;
			if(nCMCnt == gData.nCMUseCount) break;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_LoadTrayCM()
{
	int w, nCMCnt = 0;

	if (gData.nTrayPos[0] < 1 || gData.nTrayPos[0] > gData.nArrayW) return FALSE;

	//w = Get_TrayLineConvert(gData.nTrayPos[0]) - 1;
	w = gData.nTrayPos[0] - 1;
	for(int l=0; l<gData.nArrayL; l++) {	//5,6
		if (gData.LoadTrayInfo[w][l] > 0) return TRUE;
	}

	return FALSE;
}

BOOL CSequenceMain::Check_InspecDone() // NG 피커 위치에 있는 모듈들이 모두 검사 완료 되었는지 확인
{
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[2][l] == 3) return FALSE;
	}
	return TRUE;
}

BOOL CSequenceMain::Check_InspecEnd(int nPos)
{
	if (nPos < 0 || nPos > 7) return FALSE;
	if (Check_InspecDone()) return TRUE;

	if (gData.nInsTrayNo != gData.IDXTrayNo[2][0] ||
	    gData.nInsLineNo != gData.IDXLineNo[2]) return FALSE;

	int nTrayNo = gData.nInsTrayNo;
	int cm = (gData.nInsLineNo - 1) * gData.nPickCnt;
	for(int i=0; i<gData.nPickCnt; i++) {
		if (m_pEquipData->bUseContinueLot) {
			if (gData.IndexInfo[2][i] > 0) gData.IndexInfo[2][i] = 1;

		} else {
			if (gData.IndexInfo[2][i] > 0) {
				gData.IndexInfo[2][i] = gLot.nInsResult[nTrayNo-1][cm+i];
				if (gData.IndexInfo[2][i] == 3) gData.IndexInfo[2][i] = 2;
			}
		}
	}
	return FALSE;
}

BOOL CSequenceMain::Check_IndexNGJobExist()
{
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[2][l] >= 2) return TRUE;
	}

	return FALSE;
}

BOOL CSequenceMain::Check_IndexGoodJobExist()
{
	int nCntPick, nCntIdx, nIndexCnt;

	nCntPick = nCntIdx = nIndexCnt = 0;
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[4][l] > 0 ) nCntIdx++;
		if (gData.PickerInfor[2][l] > 0 ) nCntPick++;
	}

	if (nCntIdx==0 && nCntPick==0) return FALSE;
	if (nCntIdx==gData.nPickCnt) return FALSE;

	if (m_pEquipData->bUseContinueLot) {	// 옵션처리
		// 과검랏 일때는 모두 양품이기 때문에 잔량이 있으면 모두 집은 다음에 바로 안쪽/바깥쪽으로 정렬할 수 있게 해준다.

		// Picker가 모듈을 가지고 있으면 마저 집거나 인덱스에 내려 놓아야한다. 
		if (nCntPick > 0) return TRUE;

		// Picker에 모듈이 없으면 Index에 모듈 정렬 상태 확인하여 모듈을 집을 수 있게 한다.
		int x = 0;
		if (gData.bGDPickerSort1) {
			// 인덱스 안쪽 1번부터 채워준다.
			for(int i=gData.nPickCnt-1; i>=0; i--) {
				if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
			}
		} else {
			// 인덱스 바깥쪽 5번부터 채워준다.
			for(int i=0; i<gData.nPickCnt; i++) {
				if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
			}
		}
		return FALSE;
	} else {
		for(int w=0; w<4; w++) {
			for(int l=0; l<gData.nPickCnt; l++) {
				if (gData.IndexInfo[w][l] > 0) nIndexCnt++;
			}
		}

		if (nIndexCnt==0) {
			int x = 0;
			if (gData.bGDPickerSort1) {
				// 인덱스 안쪽 1번부터 채워준다.
				for(int i=gData.nPickCnt-1; i>=0; i--) {
					if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
				}
			} else {
				// 인덱스 바깥쪽 5번부터 채워준다.
				for(int i=0; i<gData.nPickCnt; i++) {
					if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
				}
			}
		}

		if (nIndexCnt >0 && nCntIdx ==0) return FALSE;
		if (nIndexCnt==0 && nCntPick==0) return FALSE;
	}
	return TRUE;
}


BOOL CSequenceMain::Check_IndexEmpty(int nPos)	// (0:Load, 1:검사, 2:NG, 4:Good, 6:Unload) 9:all
{
	if (nPos >= 0 && nPos < 7) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[nPos][l] > 0) return FALSE;
		}
		return TRUE;
	}
	for(int w=0; w<8; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[w][l] > 0) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_LotEnd()
{
	for(int w=0; w<8; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[w][l] > 0) return FALSE;
		}
	}
	for(int w=0; w<4; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.PickerInfor[w][l] > 0) return FALSE;
		}
	}

	return TRUE;
}

BOOL CSequenceMain::Check_LastTray()
{
	int nCount = 0;

	for(int w=0; w<7; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[w][l]==2) continue;
			if (gData.IndexInfo[w][l] > 0) nCount++;
		}
	}
	for(int w=0; w<4; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.PickerInfor[w][l]==2) continue;
			if (gData.PickerInfor[w][l] > 0) nCount++;
		}
	}

	for(int w=0; w<gData.nArrayW; w++) {		//y
		for(int l=0; l<gData.nArrayL; l++) {	//x
			if (gData.LoadTrayInfo[w][l] == 3) nCount++;
		}
	}

	if(nCount <= gData.nCMUseCount) return TRUE;
	else							return FALSE;
}

BOOL CSequenceMain::Check_UnloadTraySort()
{
	int nSpace = 0;

	if (gData.bUnloadTraySort==FALSE) return FALSE;

	for(int w=gData.nArrayW-1; w>=0; w--) {		//y=8
		for(int l=gData.nArrayL-1; l>=0; l--) {	//x=5
			if (nSpace==1 && gData.GoodTrayInfo[w][l]==0) {
				nSpace = 9;
				break;
			}
			if (nSpace==0 && gData.GoodTrayInfo[w][l] > 0) {
				nSpace = 1;
			}
		}
		if (nSpace==9) break;
	}
	if (nSpace==9)	return TRUE;
	else			return FALSE;

}

BOOL CSequenceMain::Check_UnloadTraySortJob()
{
	for(int w=gData.nArrayW-1; w>=0; w--) {		//y=8
		if (gData.GoodTrayInfo[w][0] > 0) {
			gData.nUpNo = w+1;
			break;
		}
	}

	gData.nDownX = 0;
	for(int w=0; w<gData.nArrayW; w++) {		//y=8
		for(int l=0; l<gData.nArrayL; l++) {	//x=5
			if (gData.GoodTrayInfo[w][l] == 0) {
				gData.nDownNo = w+1;
				gData.nDownX = l+1;
				break;
			}
		}
		if (gData.nDownNo>0) break;
	}

	if (gData.nUpNo>0 && gData.nDownNo>0)	return TRUE;
	else									return FALSE;
}

BOOL CSequenceMain::Check_PickerEmpty()
{
	for(int w=0; w<4; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.PickerInfor[w][l] > 0 ) return FALSE;
		}
	}
	return TRUE;
}

BOOL CSequenceMain::Check_GoodPickJobUp()
{
	int nCntPick, nCntIdx, nCntTot, nIndexCnt;

	nCntPick = nCntIdx = nIndexCnt = 0;
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[4][l] > 0 ) nCntIdx++;
		if (gData.PickerInfor[2][l] > 0 ) nCntPick++;
	}
	nCntTot = nCntIdx + nCntPick;

	if (m_pEquipData->bUseContinueLot) {	// 옵션처리
		int x = 0;
		if (gData.bGDPickerSort1) {
			// 인덱스 안쪽 1번부터 채워준다.
			for(int i=gData.nPickCnt-1; i>=0; i--) {
				if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
			}
		} else {
			for(int i=0; i<gData.nPickCnt; i++) {
				// 인덱스 바깥쪽 5번부터 채워준다.
				if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
			}
		}		

	} else {
		for(int w=0; w<4; w++) {
			for(int l=0; l<gData.nPickCnt; l++) {
				if (gData.IndexInfo[w][l] > 0) nIndexCnt++;
			}
		}

		if (nIndexCnt==0) {
			if (nCntIdx==0 || nCntTot >= gData.nPickCnt) return FALSE;

			int x = 0;
			if (gData.bGDPickerSort1) {
				// 인덱스 안쪽 1번부터 채워준다.
				for(int i=gData.nPickCnt-1; i>=0; i--) {
					if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
				}
			} else {
				// 인덱스 바깥쪽 5번부터 채워준다.
				for(int i=0; i<gData.nPickCnt; i++) {
					if (x==1 && gData.IndexInfo[4][i]==0) return TRUE;
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=1;
				}
			}
			return FALSE;
		}

		if (nCntIdx > 0 && nCntTot < gData.nPickCnt) return TRUE;
	}
	return FALSE;
}

BOOL CSequenceMain::Check_GoodAllPick()
{
	int nCntPick, nCntIdx, nIndexCnt;

	if (!m_pEquipData->bUseAllPick) return FALSE;

	nCntPick = nCntIdx = nIndexCnt = 0;
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[4][l] > 0 ) nCntIdx++;
		if (gData.PickerInfor[2][l] > 0 ) nCntPick++;
	}
	if (nCntIdx==0 || nCntPick > 0) return FALSE;

	for(int w=0; w<4; w++) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[w][l] > 0) nIndexCnt++;
		}
	}
	if (nIndexCnt==0) return FALSE;
	
	return TRUE;
}

void CSequenceMain::Check_NGPickUpSelect()
{
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.IndexInfo[2][l] >= 2) {
			gData.NGIdxNo = l + 1;
			break;
		}
	}
#ifdef NG_PICKER_3
	for(int l=2; l>=0; l--) {
		if (gData.PickerInfor[1][l] == 0) {
			gData.NGPicNo = l + 1;
			break;
		}
	}
#else
	for(int l=4; l>=0; l--) {
		if (gData.PickerInfor[1][l] == 0) {
			gData.NGPicNo = l + 1;
			break;
		}
	}
#endif
}

void CSequenceMain::Check_GoodPickUpSelect()
{
	int nIndexCnt=0;
	int x = 0;

	if (m_pEquipData->bUseContinueLot) {	// 옵션처리
		gData.GDIdxNo = 0;
		x = 0;

		if (gData.bGDPickerSort1) {
			// 인덱스 안쪽 1번부터 채워준다.
			for(int i=gData.nPickCnt-1; i>=0; i--) {
				if (x>0  && gData.IndexInfo[4][i]==0) {
					gData.GDIdxNo = x;
					break;
				}
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=i+1;
			}
		} else {
			// 인덱스 바깥쪽 5번부터 채워준다.
			for(int i=0; i<gData.nPickCnt; i++) {
				if (x>0  && gData.IndexInfo[4][i]==0) {
					gData.GDIdxNo = x;
					break;
				}
				if (x==0 && gData.IndexInfo[4][i] > 0 ) x=i+1;
			}
		}		

		if (gData.GDIdxNo == 0) {
			for(int l=0; l<gData.nPickCnt; l++) {
				if (gData.IndexInfo[4][l] > 0) {
					gData.GDIdxNo = l + 1;
					break;
				}
			}
		}
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.PickerInfor[2][l] == 0) {
				gData.GDPicNo = l + 1;
				break;
			}
		}
		
	} else {
		for(int w=0; w<4; w++) {
			for(int l=0; l<gData.nPickCnt; l++) {
				if (gData.IndexInfo[w][l] > 0) nIndexCnt++;
			}
		}
		gData.GDIdxNo = 0;
		if (nIndexCnt==0) {
			x = 0;
			if (gData.bGDPickerSort1) {
				// 인덱스 안쪽 1번부터 채워준다.
				for(int i=gData.nPickCnt-1; i>=0; i--) {
					if (x>0  && gData.IndexInfo[4][i]==0) {
						gData.GDIdxNo = x;
						break;
					}
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=i+1;
				}
			} else {
				// 인덱스 바깥쪽 5번부터 채워준다.
				for(int i=0; i<gData.nPickCnt; i++) {
					if (x>0  && gData.IndexInfo[4][i]==0) {
						gData.GDIdxNo = x;
						break;
					}
					if (x==0 && gData.IndexInfo[4][i] > 0 ) x=i+1;
				}
			}
			
		}

		if (gData.GDIdxNo == 0) {
			for(int l=0; l<gData.nPickCnt; l++) {
				if (gData.IndexInfo[4][l] > 0) {
					gData.GDIdxNo = l + 1;
					break;
				}
			}
		}
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.PickerInfor[2][l] == 0) {
				gData.GDPicNo = l + 1;
				break;
			}
		}
	}
}

void CSequenceMain::Check_GoodPickDownSelect()
{
	int nGoodPickerCnt;
#ifdef GD_PICKER_3
	nGoodPickerCnt = 3;
#endif	
#ifdef GD_PICKER_5
	nGoodPickerCnt = 5;
#endif	
#ifdef GD_PICKER_6
	nGoodPickerCnt = 6;
#endif
	
	if (gData.bGDPickerSort1) {
		// 인덱스 안쪽 1번부터 채워준다.
		for(int l=0; l<gData.nPickCnt; l++) {
			if (gData.IndexInfo[4][l] == 0) {
				gData.GDIdxNo = l + 1;
				break;
			}
		}
		for(int l=0; l<nGoodPickerCnt; l++) {
			if (gData.PickerInfor[2][l] > 0) {
				gData.GDPicNo = l + 1;
				break;
			}
		}

	} else {
		// 인덱스 바깥쪽 5번부터 채워준다.
		for(int l=gData.nPickCnt-1; l>=0; l--) {
			if (gData.IndexInfo[4][l] == 0) {
				gData.GDIdxNo = l + 1;
				break;
			}
		}
		for(int l=nGoodPickerCnt-1; l>=0; l--) {
			if (gData.PickerInfor[2][l] > 0) {
				gData.GDPicNo = l + 1;
				break;
			}
		}
	}
}

void CSequenceMain::Check_NGPickDownSelect()
{
	gData.NGPicNo = gData.NGIdxNo = 0;
	if (gData.nNGTrayPos != 1 && gData.nNGTrayPos != 2) return;

	int w = gData.nTrayPos[gData.nNGTrayPos]-1;

	if (w==0 || w==2 || w==4) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if((gData.nNGTrayPos == 1 && gData.NG1TrayInfo[w][l] == 0) ||
			   (gData.nNGTrayPos == 2 && gData.NG2TrayInfo[w][l] == 0) ) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo = gData.NGIdxNo;
				break;
			}
		}
	} else {
		for(int l=gData.nPickCnt-1; l>=0; l--) {
			if((gData.nNGTrayPos == 1 && gData.NG1TrayInfo[w][l] == 0) ||
			   (gData.nNGTrayPos == 2 && gData.NG2TrayInfo[w][l] == 0) ) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo + gData.nPickCnt +gData.NGIdxNo;
				break;
			}
		}
	}
	if (gData.NGIdxNo == 0) return;

	for(int l=0; l<gData.nPickCnt-1; l++) {
		if (gData.PickerInfor[1][l] > 0) {
			gData.NGPicNo = l + 1;
			break;
		}
	}
}

BOOL CSequenceMain::Check_NGTray1PickDownSelect()
{
	gData.NGPicNo = gData.NGIdxNo = 0;

	int w = gData.nTrayPos[1]-1;

	if (w==0 || w==2 || w==4) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if(gData.NG1TrayInfo[w][l] == 0) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo = gData.NGIdxNo;
				break;
			}
		}
	} else {
		for(int l=gData.nPickCnt-1; l>=0; l--) {
			if(gData.NG1TrayInfo[w][l] == 0) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo + gData.nPickCnt +gData.NGIdxNo;
				break;
			}
		}
	}

#ifdef MODEL_SC	// 분리배출해야할 불량이 두가지이다. Fiducial(13), Shiny Edge(14)
	for(int l=0; l<gData.nPickCnt; l++) {
		if ( gData.PickerInfor[1][l] > 0 && 
			(gData.PickerInfor[1][l] != 13 &&
			 gData.PickerInfor[1][l] != 14)) {
			gData.NGPicNo = l + 1;
			break;
		}
	}
#else
	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.PickerInfor[1][l] > 0 && gData.PickerInfor[1][l] != gData.nSeparationNgCode) {
			gData.NGPicNo = l + 1;
			break;
		}
	}
#endif

	if (gData.NGPicNo == 0 || gData.NGIdxNo == 0) return FALSE;

	return TRUE;
}

BOOL CSequenceMain::Check_NGTray2PickDownSelect()
{
	gData.NGPicNo = gData.NGIdxNo = 0;
	//	if (gData.nNGTrayPos != 2) return FALSE;

	int w = gData.nTrayPos[2]-1;

#ifdef MODEL_SC
	int nLineStart = 0, nLineEnd = gData.nArrayW, nTrayX = 0, nTrayY = 0;	// Default
	int nTrayYhalf = gData.nArrayW / 2;			// F2122 5*10 1~4Line: Shiny Edge, 6~10Line: Lens Fiducial
	BOOL bBreak = FALSE;

	for (int i = 0; i < gData.nPickCnt; i++) { if (gData.PickerInfor[1][i] == 14) { gData.NGPicNo = i + 1; break; } }	// Picker 1번부터 Shiny Edge(코드14) 체크
	if (gData.NGPicNo == 0) {
		for (int i = 0; i < gData.nPickCnt; i++) { if (gData.PickerInfor[1][i] == 13) { gData.NGPicNo = i + 1; break; } }	// Picker 1번부터 Fiducial(코드13) 체크
		if (gData.NGPicNo == 0) return FALSE;
		nLineStart = nTrayYhalf;		// F2122 5*10 6~10Line (코드13, Fiducial)
	} else {
		nLineEnd = nTrayYhalf - 1;	// F2122 5*10 1~4Line (코드14, Shiny Edge)
	}

	for (int y = nLineStart; y < nLineEnd; y++) {
		for (int x = 0; x < gData.nArrayL; x++) {
			if (gData.NG2TrayInfo[y][x] == 0) { nTrayX = x + 1; nTrayY = y + 1;  bBreak = TRUE; break;}
		}
		if (bBreak) break;
	}
	gData.nTrayPos[2] = nTrayY;
	gData.NGIdxNo = nTrayX;

	if (gData.NGPicNo == 0 || gData.NGIdxNo == 0) return FALSE;

#else
	if (w==0 || w==2 || w==4) {
		for(int l=0; l<gData.nPickCnt; l++) {
			if(gData.NG2TrayInfo[w][l] == 0) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo = gData.NGIdxNo;
				break;
			}
		}
	} else {
		for(int l=gData.nPickCnt-1; l>=0; l--) {
			if(gData.NG2TrayInfo[w][l] == 0) {
				gData.NGIdxNo = l + 1;
				gData.nNGTrayCmNo + gData.nPickCnt +gData.NGIdxNo;
				break;
			}
		}
	}

	for(int l=0; l<gData.nPickCnt; l++) {
		if (gData.PickerInfor[1][l] == gData.nSeparationNgCode) {
			gData.NGPicNo = l + 1;
			break;
		}
	}

	if (gData.NGPicNo == 0 || gData.NGIdxNo == 0) return FALSE;
#endif

	return TRUE;
}

void CSequenceMain::Set_LoadTray()
{
	int nCMCnt, nCMxxx;

	nCMCnt = nCMxxx = 0;
	for(int w=0; w<20; w++) {
		for(int l=0; l<20; l++) {
			gData.LoadTrayInfo[w][l] = 0;
		}
	}

	if (m_pEquipData->bUseContinueLot) {
		int nTrayCnt, nTrayUse, nCmUse;
		nTrayCnt = gData.nLoadTrayCount;

		int nLotIndex = 0;
		for (int i = 0; i < 5; i++) {
			if (nTrayCnt > gData.nTraysUseCnt[i]) nTrayCnt -= gData.nTraysUseCnt[i];
			else { nLotIndex = i; break; }
		}
		nTrayUse = gData.nTraysUseCnt[nLotIndex];
		nCmUse   = gData.nCmsUseCnt[nLotIndex];

		gData.sLotID = gData.sLotsID[nLotIndex];	// Lot ID 교체..

		if (nTrayCnt < nTrayUse) {
			for(int w=0; w<gData.nArrayW; w++) {		//y(2)
				for(int l=0; l<gData.nArrayL; l++) {	//x(6)
					gData.LoadTrayInfo[w][l] = 3;
					nCMCnt++;
					if(nCMCnt == gData.nCMUseCount) break;
				}
			}
			return;
		}

		if (gData.nLoadTrayDir == 0) {	//Rear(1~)
			nCMxxx = nCmUse % gData.nCMUseCount;
			for(int w=0; w<gData.nArrayW; w++) {		//y(2)
				if (w==0 || w==2 || w==4) {
					for(int l=0; l<gData.nArrayL; l++) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				} else {
					for(int l=gData.nArrayL-1; l>=0; l--) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				}
				if(nCMCnt == nCMxxx) break;
			}
		}
		if (gData.nLoadTrayDir == 1) {	//Front(6~)
			nCMxxx = nCmUse % gData.nCMUseCount;
			for(int w=0; w<gData.nArrayW; w++) {		//y(2)
				if (w==0 || w==2 || w==4) {
					for(int l=gData.nArrayL-1; l>=0; l--) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				} else {
					for(int l=0; l<gData.nArrayL; l++) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				}
				if(nCMCnt == nCMxxx) break;
			}
		}

	} else {

		if (gLot.nTrayCount > gData.LoadTrayNo) {
			for(int w=0; w<gData.nArrayW; w++) {		//y
				for(int l=0; l<gData.nArrayL; l++) {	//x
					gData.LoadTrayInfo[w][l] = 3;
					nCMCnt++;
					if(nCMCnt == gData.nCMUseCount) break;
				}
			}
			CInspector *pInspector = CInspector::Get_Instance();
			pInspector->Set_LotCount(INSPECTOR_VISION);
			return;
		}

		if (gData.nLoadTrayDir == 0) {	//Rear(1~)
			nCMxxx = gLot.nCMCount % gData.nCMUseCount;
			for(int w=0; w<gData.nArrayW; w++) {		//y(2)
				if (w==0 || w==2 || w==4) {
					for(int l=0; l<gData.nArrayL; l++) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				} else {
					for(int l=gData.nArrayL-1; l>=0; l--) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				}
				if(nCMCnt == nCMxxx) break;
			}
		}
		if (gData.nLoadTrayDir == 1) {	//Front(6~)
			nCMxxx = gLot.nCMCount % gData.nCMUseCount;
			for(int w=0; w<gData.nArrayW; w++) {		//y(2)
				if (w==0 || w==2 || w==4) {
					for(int l=gData.nArrayL-1; l>=0; l--) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				} else {
					for(int l=0; l<gData.nArrayL; l++) {	//x(6)
						gData.LoadTrayInfo[w][l] = 3;
						nCMCnt++;
						if(nCMCnt == nCMxxx) break;
					}
				}
				if(nCMCnt == nCMxxx) break;
			}
		}

		CInspector *pInspector = CInspector::Get_Instance();
		pInspector->Set_LotCount(INSPECTOR_VISION);
	}
}

void CSequenceMain::Set_IndexEnd()
{
	for(int i=6; i>=0; i--) {
		gData.IDXLineNo[i+1] = gData.IDXLineNo[i];
		gData.IDXInspec[i+1] = gData.IDXInspec[i];

		for(int j=0; j<6; j++) {
			gData.IDXTrayNo[i+1][j] = gData.IDXTrayNo[i][j];
			gData.IndexInfo[i+1][j] = gData.IndexInfo[i][j];
			gData.IDXPoNo[i+1][j] = gData.IDXPoNo[i][j];
		}
	}
	gData.IDXLineNo[0] = gData.IDXInspec[0] = 0;
	for(int j=0; j<6; j++) {
		gData.IDXTrayNo[0][j] = 0;
		gData.IndexInfo[0][j] = 0;
		gData.IDXPoNo[0][j] = 0;
		gData.IndexJob[j] = 0;
	}

	Set_TackLog(9);
}

void CSequenceMain::Set_GoodTrayEmpty()
{
	gData.nGoodTrayNo++;
	for(int w=0; w<20; w++) {
		for(int l=0; l<20; l++) {
			gData.GoodTrayInfo[w][l] = 0;
		}
	}
}

int CSequenceMain::Get_GoodTrayCmCnt()
{
	int Cnt = 0;
	for(int w=0; w<20; w++) {
		for(int l=0; l<20; l++) {
			if (gData.GoodTrayInfo[w][l] > 0) Cnt++;
		}
	}
	return Cnt;
}

///////////////////////////////////////////////////////////////////////////////

void CSequenceMain::Job_LotStart()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	
	gData.nStatus = 1;	// Lot Start
	gLot.sLotID = gData.sLotID;
	gLot.sStartTime.Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	gLot.sMESStartTime.Format("%04d/%02d/%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	gLot.lLotStart = GetTickCount();
	gLot.nTrayCount = gData.nTrayJobCount;
	gLot.nCMCount = gData.nCMJobCount;
	gData.nDoorInterlock67 = 1;

	//Tray Data Clear..
	gLot.nGoodCnt = gLot.nNGCnt = 0;
	gLot.nLfNGCnt = gLot.nSeNGCnt = 0;
	gLot.nSideNgCnt = 0;
	gLot.nBsNGCnt = 0;
	for(int i = 0; i<20; i++){
		for(int j=0; j<20; j++){
			gData.NG1TrayInfo[i][j] = 0;
			gData.NG2TrayInfo[i][j] = 0;
		}
	}
	if (m_pEquipData->bUseContinueLot) {
		gLot.sLotID = gData.sLotsID[0];

		// Total Tray Count
		gLot.nTrayCount = 0;	// 전체 Tray 수량
		for (int i = 0; i < 5; i++) {
			if (gData.nCmsUseCnt[i] == 0) break;		// 중간에 0이 있으면 중단
			gLot.nTrayCount += gData.nTraysUseCnt[i];
		}

		// Total CM Count
		gLot.nCMCount = 0;	// 전체 CM 수량
		for (int i = 0; i < 5; i++) {
			if (gData.nCmsUseCnt[i] == 0) break;		// 중간에 0이 있으면 중단
			gLot.nCMCount += gData.nCmsUseCnt[i];
		}
	}
	CInspector *pInspector = CInspector::Get_Instance();
	pInspector->Set_LotStart(INSPECTOR_VISION);
	g_objCapAttachUDP.Set_LotStart(gData.nPortNo);
	g_objCapAttachUDP.Set_ContinueLotInfo();
	//g_objCapAttachUDP.Set_LotStart(1);

	//PC Information Log 파일 생성
	CString strLog, strCPU, strRAM, strHardDisk;

	CCommon *pCommon = CCommon::Get_Instance();
	pCommon->Get_CPUInfo(strCPU);
	pCommon->Get_RAMSize(strRAM);
	pCommon->Get_HardInfo(strHardDisk);

	strLog.Format("%s,%s,%s,%s,%s",gData.sLotID,gData.sRecipeName, strCPU,strRAM,strHardDisk);
	CLogFile *pLogFile = CLogFile::Get_Instance();
	pLogFile->Save_PCLog(strLog);


	strLog.Format("Lot Started, LotID: %s, StartTime : %s, Tray Count:%d, CM Count:%d", gLot.sLotID, gLot.sStartTime, gLot.nTrayCount, gLot.nCMCount);
	pLogFile->Save_HandlerLog(strLog);

	// AVI APD : Lot단위로 남겨준다.
	gData.bAviApdRequest = FALSE;
	gData.bAviApdReceive = FALSE;	
	gData.bAviApdSave = FALSE;
	gData.bAviApdFileChk = FALSE;

	gData.dNGSum = 0.0;
	gData.dNGCount = ((double(gData.nCMJobCount) * (double(gData.nNGPercent) / 100)) / double(gData.nCMJobCount));

	CWorkDlg *pWorkDlg = CWorkDlg::Get_Instance();
	pWorkDlg->SendMessage(UM_UPDATE_SHIFT_LIST, (WPARAM)OP_CLEAR, (LPARAM)NULL);

	pWorkDlg->PostMessage(UM_LOT_START_END, (WPARAM)1, NULL);	// LotStart

}

void CSequenceMain::Job_LotEnd()
{
	double	dTack, dMESTack;
	long	lTime;
	CString sLog;

	SYSTEMTIME time;
	GetLocalTime(&time);

	gLot.lLotEnd = GetTickCount();
	gLot.sEndTime.Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	if(gLot.nCMCount < 1) gLot.nCMCount = gLot.nTrayCount * gData.nCMUseCount;
	lTime = gLot.lLotEnd - gLot.lLotStart;
	dMESTack = double(lTime/1000);
	dTack = double(lTime/1000) / double(gLot.nCMCount);
	if(dTack > 0) gLot.dTackTime = dTack;

	gLot.nGoodCnt = gLot.nCMCount - gLot.nNGCnt;
	
	//if (!m_pEquipData->bUseInlineMode) {
#ifdef MODEL_SC
		sLog.Format("%s,%s,%s,%d,%02d,%04d,%0.7lf,%d,%d,%d,%d",
			gLot.sLotID, gLot.sStartTime, gLot.sEndTime, lTime, gLot.nTrayCount, gLot.nCMCount, gLot.dTackTime, gLot.nGoodCnt, gLot.nNGCnt, gLot.nLfNGCnt, gLot.nSeNGCnt);
#else
	sLog.Format("%s,%s,%s,%d,%02d,%04d,%0.7lf,%d,%d,%d",
		gLot.sLotID, gLot.sStartTime, gLot.sEndTime, lTime, gLot.nTrayCount, gLot.nCMCount, gLot.dTackTime, gLot.nGoodCnt, gLot.nNGCnt, gLot.nSideNgCnt);
#endif
		pLogFile->Save_JobListLog(sLog);
		pLogFile->Save_HandlerLog("Lot End History, "+sLog);
	//}
	if(gLot.sLotID.GetLength()>11) {
		int	i, j;
		gTD.dTackTime = gLot.dTackTime;
		if (time.wHour>=7 && time.wHour<=18) i=0;
		else								 i=1;
		gTD.dCount[i] = gTD.dCount[i] + gLot.nCMCount;
		i = time.wHour;
		j = (int)gTD.dTack[i][49];
		if(j<45) {
			gTD.dTack[i][j] = gLot.dTackTime;
			gTD.dTack[i][49] = j + 1;
		}

		CIniFileCS INI("System/Inspection.ini");
		INI.Set_Integer("COUNT", "COUNT_00", gTD.dCount[0]);
		INI.Set_Integer("COUNT", "COUNT_01", gTD.dCount[1]);
	}

	pLogFile->Save_LotLog("LotResult");

	g_objCapAttachUDP.Set_TrayEnd(gData.nPortNo);
	//g_objCapAttachUDP.Set_TrayEnd(1);
	Sleep(30);
	g_objCapAttachUDP.Set_LotEnd(gData.nPortNo);
	//g_objCapAttachUDP.Set_LotEnd(1);

	if (gLot.sLotID == gData.sLotID) gData.sLotID = "";
	if (gData.bCleanOutMode==FALSE && !m_pEquipData->bUseContinueLot) {
		// 캡 조립이 완료가 되면 완공하기 위해 정보 저장.
// 		if (m_pEquipData->bUseInlineMode) {
// 			gLot.sCALotID = gLot.sLotID;
// 			gLot.nCACmCount = gLot.nCMCount;
// 			gLot.nCAGoodCount = gLot.nGoodCnt;
// 			gLot.nCANgCount = gLot.nNGCnt;
// 			gLot.sCAStartTime = gLot.sStartTime;
// 			gLot.dwCALotStart = gLot.lLotStart;
// 
// 			gData.bCapAttachWork = FALSE;
// 			//gData.bCapAttachWork = TRUE;	// Inline 모드 일때  Cap Attach까지 LotEnd되어야 투입할수 있도록 해준다.
// 
// 
// 		} else {
			if (!m_pEquipData->bUseMesApd || !gData.bAPDResultErr) g_objMES.Set_LotEnd(gLot.sLotID, gLot.nCMCount, gData.sOperID, gLot.nGoodCnt, gLot.nNGCnt);
// 		}
	}

	if (m_pEquipData->bUseContinueLot) {
		if (m_pEquipData->bUseInlineMode) gData.bCapAttachWork = TRUE;	// 과검랏 모드 일때 Cap Attach까지 LotEnd되어야 다음 랏 투입할 수 있도록 해준다.
		gData.nPortNo = 1;

	} else {
		if (gData.nPortNo == 1) gData.nPortNo = 2;
		else					gData.nPortNo = 1;
	}
	CWorkDlg *pWorkDlg = CWorkDlg::Get_Instance();
	pWorkDlg->Clear_LotInfo();
	pWorkDlg->PostMessage(UM_LOT_START_END, (WPARAM)2, NULL);	// LotEnd
}

///////////////////////////////////////////////////////////////////////////////

void CSequenceMain::Set_TackLog(int nNo)
{
	long	lTack, curTime;
	CString	sLog;

	if (!m_pEquipData->bUseTackLog) return;
	curTime = GetTickCount();
	if (nNo < 6) {
		lTack = curTime - gLot.lTackData[nNo];
		gLot.lTackData[nNo] = lTack;
		return;
	}

	// Index tack
	if (gLot.lTackData[6] > 0)
		lTack = curTime - gLot.lTackData[6];
	else lTack = 0;


	sLog.Format("Tack=>,Index,%d,Load_Pick,%d,Inspect,%d,Barcode,%d,NG_Pick,%d,Good_Pick,%d,Unlaod_Pick,%d", lTack, gLot.lTackData[0], gLot.lTackData[1], gLot.lTackData[2], gLot.lTackData[3], gLot.lTackData[4], gLot.lTackData[5]);
	pLogFile->Save_HandlerLog(sLog);

	for(int i=0; i<10; i++) {
		gLot.lTackData[i] = curTime;
		gLot.nLog[i] = 0;
	}
}

int CSequenceMain::Get_TrayLineConvert(int nLine)
{
	int nTrayArray = 0;
	int nLineNo = nLine - 1;

// 	if (nLine==1) nTrayArray=8;
// 	if (nLine==2) nTrayArray=7;
// 	if (nLine==3) nTrayArray=6;
// 	if (nLine==4) nTrayArray=5;
// 	if (nLine==5) nTrayArray=4;
// 	if (nLine==6) nTrayArray=3;
// 	if (nLine==7) nTrayArray=2;
// 	if (nLine==8) nTrayArray=1;

	nTrayArray = gData.nArrayW - nLineNo;
	if(nTrayArray < 1) nTrayArray = 1;

	return nTrayArray;
}

int CSequenceMain::Select_PikerNo(int nPicker, int nNo)
{
/*
	for(int i=nNo; i<6; i++) {
		if (nPicker == 0 && gData.bNGPickerUse[i] == TRUE)  return i;
		if (nPicker == 1 && gData.bGoPickerUse[i] == TRUE)  return i;
	}
*/
	return -1;
}	

int CSequenceMain::Count_PikerNotUse(int nPicker)
{
	int nCount = 0;
	/*
	for(int i=0; i<6; i++) {
		if (nPicker == 0 && gData.bNGPickerUse[i] == FALSE)  nCount++;
		if (nPicker == 1 && gData.bGoPickerUse[i] == FALSE)  nCount++;
	}
*/
	return nCount;
}

BOOL CSequenceMain::Check_NGTrayJobEnd()
{
	if (Check_IndexEmpty(0) == FALSE) return FALSE;
	if (Check_IndexEmpty(1) == FALSE) return FALSE;
	if (Check_IndexEmpty(2) == FALSE) return FALSE;

	for(int i=0; i<gData.nPickCnt; i++) {
		if (gData.PickerInfor[1][i] > 0) return FALSE;
	}
	if (Check_NGPickerAllUp() == TRUE) return TRUE;
	return FALSE;
}

BOOL CSequenceMain::Run_Simulation()
{
#ifdef AJIN_BOARD_USE
	return TRUE;
#endif
	Sleep(SIM_WAITTIMES); 

	CAJinAXL *pAJinAXL = CAJinAXL::Get_Instance();
	DX_DATA_0 *pDX0 = pAJinAXL->Get_pDX0();
	DX_DATA_1 *pDX1 = pAJinAXL->Get_pDX1();
	DX_DATA_2 *pDX2 = pAJinAXL->Get_pDX2();
	DX_DATA_3 *pDX3 = pAJinAXL->Get_pDX3();
	DX_DATA_4 *pDX4 = pAJinAXL->Get_pDX4();
	DX_DATA_5 *pDX5 = pAJinAXL->Get_pDX5();
	DX_DATA_6 *pDX6 = pAJinAXL->Get_pDX6();
	DY_DATA_0 *pDY0 = pAJinAXL->Get_pDY0();
	DY_DATA_1 *pDY1 = pAJinAXL->Get_pDY1();
	DY_DATA_2 *pDY2 = pAJinAXL->Get_pDY2();
	DY_DATA_3 *pDY3 = pAJinAXL->Get_pDY3();
	DY_DATA_4 *pDY4 = pAJinAXL->Get_pDY4();
	DY_DATA_5 *pDY5 = pAJinAXL->Get_pDY5();
	DY_DATA_6 *pDY6 = pAJinAXL->Get_pDY6();

	if (m_nLoad1Case == 110) { pDX1->iLS_Z1Check1 = FALSE; pDX1->iLS_Z1Check2 = FALSE; }
	if (m_nLoad1Case == 170 || m_nLoad1Case == 507) { pDX1->iLS_Z1Check1 = TRUE; pDX1->iLS_Z1Check2 = TRUE; }
	if (m_nLoad1Case == 480) { pDX1->iLS_Z1Check1 = FALSE; pDX1->iLS_Z1Check2 = FALSE; pDX1->iLS_Z2Check1 = FALSE; pDX1->iLS_Z2Check2 = FALSE; }
	if (m_nLoad2Case == 110) { pDX1->iLS_Z2Check1 = FALSE; pDX1->iLS_Z2Check2 = FALSE; }
	if (m_nLoad2Case == 170) { pDX1->iLS_Z2Check1 = TRUE; pDX1->iLS_Z2Check2 = TRUE; }

	if (m_nUnload1Case == 110) { pDX6->iUS_Z1Check1 = FALSE; pDX6->iUS_Z1Check2 = FALSE;
								 pDX6->iUS_Z1AlignS12In = FALSE; pDX6->iUS_Z1AlignS12Out = TRUE; pDX6->iUS_Z1AlignM34In = FALSE; pDX6->iUS_Z1AlignM34Out = TRUE; }
	if (m_nUnload1Case == 170) { pDX6->iUS_Z1Check1 = TRUE; pDX6->iUS_Z1Check2 = TRUE; }
	if (m_nUnload1Case == 380) { gData.bCapTrayLoad = FALSE; }
	if (m_nUnload1Case == 413) { pDX6->iUS_Z1Check1 = FALSE; pDX6->iUS_Z1Check2 = FALSE; }
	if (m_nUnload2Case == 110) { pDX6->iUS_Z2Check1 = FALSE; pDX6->iUS_Z2Check2 = FALSE;
								 pDX6->iUS_Z2AlignS12In = FALSE; pDX6->iUS_Z2AlignS12Out = TRUE; pDX6->iUS_Z2AlignM34In = FALSE; pDX6->iUS_Z2AlignM34Out = TRUE; }
	if (m_nUnload2Case == 170) { pDX6->iUS_Z2Check1 = TRUE; pDX6->iUS_Z2Check2 = TRUE; }
	if (m_nUnload2Case == 380) { gData.bCapTrayLoad = FALSE; }
	if (m_nUnload2Case == 413) { pDX6->iUS_Z2Check1 = FALSE; pDX6->iUS_Z2Check2 = FALSE; }

	if (m_nInspectCase == 0) { }
	if (m_nNGPickerCase == 0) { }
	if (m_nGDPickerCase == 0) { }
	if (m_nLDPickerCase == 0) { }
	if (m_nULPickerCase == 0) { }
	if (m_nIndexTCase == 0) { }
	if (m_nNGTrayCase == 0) { }
	if (m_nBarcodeCase == 0) { }

	return TRUE;
}