// CMainDlg.cpp: 实现文件
//
#define  _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "StudentManagement.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include "log.h"
#include <stdio.h>
#include "globalvar.h"
#include "CStudentInfoDlg.h"
#include <iostream>
#include <fstream>
#include < algorithm >
// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DLG, pParent)
{

}

CMainDlg::~CMainDlg()
{

}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MENU, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, &CMainDlg::OnSelchangeTabMenu)
//	ON_MESSAGE(CLASS_ADD_OK, &CMainDlg::OnClassAddOk)
//ON_MESSAGE(WM_CLASS_ADD_OK, &CMainDlg::OnClassAddOk)
//ON_MESSAGE(WM_CLASS_ADD_ERR, &CMainDlg::OnClassAddErr)
//ON_MESSAGE(WM_CLASS_DEL_OK, &CMainDlg::OnClassDelOk)
//ON_MESSAGE(WM_CLASS_DEL_ERR, &CMainDlg::OnClassDelErr)
//ON_MESSAGE(WM_COURSE_QUERY_NULL, &CMainDlg::OnCourseQueryNull)
//ON_MESSAGE(WM_COURSE_QUERY_ERR, &CMainDlg::OnCourseQueryErr)
//ON_MESSAGE(WM_COURSE_QUERY_RESULT, &CMainDlg::OnCourseQueryResult)
//ON_MESSAGE(WM_COURSE_EDIT_NULL, &CMainDlg::OnCourseEditNull)
//ON_MESSAGE(WM_COURSE_EDIT_OK, &CMainDlg::OnCourseEditOk)
//ON_MESSAGE(WM_COURSE_EDIT_NULL, &CMainDlg::OnCourseEditNull)
//ON_MESSAGE(WM_COURSE_EDIT_OK, &CMainDlg::OnCourseEditOk)
//ON_MESSAGE(WM_COURSE_QUERY_ERR, &CMainDlg::OnCourseQueryErr)
ON_COMMAND(IDOK, &CMainDlg::OnIdok)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitTabCtrl();
	//创建接收线程
	m_pRecvThread = new std::thread(CMainDlg::RecvThread, this);

	return TRUE;  // return TRUE unless you set the focus to a control
			  // 异常: OCX 属性页应返回 FALSE
}





int CMainDlg::InitTabCtrl()
{
	m_TabCtrl.InsertItem(0, _T("Student Info"));
	m_TabCtrl.InsertItem(1, _T("Class Info"));
	m_TabCtrl.InsertItem(2, _T("Course Info"));
	m_TabCtrl.InsertItem(3, _T("Course Selection"));


	CRect rc;
	m_TabCtrl.GetClientRect(rc);

	//创建页
	m_stuDlg.Create(IDD_STUDENTINFO_DLG, &m_TabCtrl);
	m_stuDlg.MoveWindow(30, 30, rc.Width() - 20, rc.Height() - 60);
	m_stuDlg.ShowWindow(SW_NORMAL);

	m_classDlg.Create(IDD_CLASS_DLG, &m_TabCtrl);
	m_classDlg.MoveWindow(30, 30, rc.Width() - 20, rc.Height() - 60);
	//m_classDlg.ShowWindow(SW_NORMAL);
	m_courseDlg.Create(IDD_COURSE_DLG, &m_TabCtrl);
	m_courseDlg.MoveWindow(30, 30, rc.Width() - 20, rc.Height() - 60);

	m_recordDlg.Create(IDD_RECORD_DLG, &m_TabCtrl);
	m_recordDlg.MoveWindow(30, 30, rc.Width() - 20, rc.Height() - 60);
	return 0;
}



void CMainDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	g_tcpSocket.Close();
	//mysql_close(&g_mysql);
}

void CMainDlg::RecvThread(CMainDlg* pThis)
{
	stPacketHdr recvHdr;
	while (true)
	{
		if (g_tcpSocket.Receive((char*)&recvHdr, sizeof(recvHdr)) < 0)
		{
			LOGE("Receive");
			continue;
		}
		switch (recvHdr.nCmd)
		{
		case STU_ADD_OK:
			pThis->m_stuDlg.PostMessage(WM_STU_ADD_OK, 0, 0);
			break;
		case STU_ADD_ERR:
			pThis->m_stuDlg.PostMessage(WM_STU_ADD_ERR, 0, 0);
			break;
		case STU_DEL_OK:
			pThis->m_stuDlg.PostMessage(WM_STU_DEL_OK, 0, 0);
			break;
		case STU_DEL_ERR:
			pThis->m_stuDlg.PostMessage(WM_STU_DEL_ERR, 0, 0);
			break;
		case STU_QUERY_RESULT:
		{
			// Receive and write the file data to a local file
			std::ofstream outputFile("received_file.txt", std::ios::binary);
			if (!outputFile) {
				LOGE("Error: Unable to open file for writing.");
				break;
			}

			std::streamsize totalBytesReceived = 0;
			const int bufferSize = 1024;
			char buffer[bufferSize];

			// Convert recvHdr.nLen to host byte order
			DWORD expectedFileSize = ntohl(recvHdr.nLen);

			while (totalBytesReceived < static_cast<std::streamsize>(expectedFileSize)) {
				std::streamsize bytesRead;
				if (bufferSize > expectedFileSize - totalBytesReceived) {
					bytesRead = g_tcpSocket.Receive(buffer, static_cast<int>(expectedFileSize - totalBytesReceived));
				}
				else {
					bytesRead = g_tcpSocket.Receive(buffer, bufferSize);
				}
				if (bytesRead < 0) {
					LOGE("Receive");
					break;
				}

				outputFile.write(buffer, bytesRead);
				totalBytesReceived += bytesRead;
			}

			outputFile.close();
			pThis->m_stuDlg.PostMessage(WM_STU_QUERY_RESULT, 0, 0);
		}
			break;
		case STU_QUERY_NULL:
			pThis->m_stuDlg.PostMessage(WM_STU_QUERY_NULL, 0, 0);
			break;		
		case STU_QUERY_ERR:
			pThis->m_stuDlg.PostMessage(WM_STU_QUERY_ERR,  0, 0);
			break;
		case STU_EDIT_ERR:
			pThis->m_stuDlg.PostMessage(WM_STU_EDIT_ERR, 0, 0);
			break;
		case STU_EDIT_NULL:
			pThis->m_stuDlg.PostMessage(WM_STU_EDIT_NULL, 0, 0);
			break;
		case STU_EDIT_OK:
			pThis->m_stuDlg.PostMessage(WM_STU_EDIT_OK, 0, 0);
			break;
		case CLASS_ADD_OK:
			pThis->m_classDlg.PostMessage(WM_CLASS_ADD_OK, 0, 0);
			break;
		case CLASS_ADD_ERR:
			pThis->m_classDlg.PostMessage(WM_CLASS_ADD_ERR, 0, 0);
			break;
		case CLASS_DEL_OK:
			pThis->m_classDlg.PostMessage(WM_CLASS_DEL_OK, 0, 0);
			break;
		case CLASS_DEL_ERR:
			pThis->m_classDlg.PostMessage(WM_CLASS_DEL_ERR, 0, 0);
			break;
		case CLASS_QUERY_RESULT:
		{
			// Receive and write the file data to a local file
			std::ofstream outputFile("received_file_class.txt", std::ios::binary);
			if (!outputFile) {
				LOGE("Error: Unable to open file for writing.");
				break;
			}

			std::streamsize totalBytesReceived = 0;
			const int bufferSize = 1024;
			char buffer[bufferSize];

			// Convert recvHdr.nLen to host byte order
			DWORD expectedFileSize = ntohl(recvHdr.nLen);

			while (totalBytesReceived < static_cast<std::streamsize>(expectedFileSize)) {
				std::streamsize bytesRead;
				if (bufferSize > expectedFileSize - totalBytesReceived) {
					bytesRead = g_tcpSocket.Receive(buffer, static_cast<int>(expectedFileSize - totalBytesReceived));
				}
				else {
					bytesRead = g_tcpSocket.Receive(buffer, bufferSize);
				}
				if (bytesRead < 0) {
					LOGE("Receive");
					break;
				}

				outputFile.write(buffer, bytesRead);
				totalBytesReceived += bytesRead;
			}

			outputFile.close();
			pThis->m_classDlg.PostMessage(WM_CLASS_QUERY_RESULT, 0, 0);
		}
		break;
		case CLASS_QUERY_NULL:
			pThis->m_classDlg.PostMessage(WM_CLASS_QUERY_NULL, 0, 0);
			break;
		case CLASS_QUERY_ERR:
			pThis->m_classDlg.PostMessage(WM_CLASS_QUERY_ERR, 0, 0);
			break;
		case CLASS_EDIT_ERR:
			pThis->m_classDlg.PostMessage(WM_CLASS_EDIT_ERR, 0, 0);
			break;
		case CLASS_EDIT_NULL:
			pThis->m_classDlg.PostMessage(WM_CLASS_EDIT_NULL,0, 0);
			break;
		case CLASS_EDIT_OK:
			pThis->m_stuDlg.PostMessage(WM_CLASS_EDIT_OK, 0, 0);
			break;
		case COURSE_ADD_OK:
			pThis->m_courseDlg.PostMessage(WM_COURSE_ADD_OK, 0, 0);
			break; 
		case COURSE_ADD_ERR:
			pThis->m_courseDlg.PostMessage(WM_COURSE_ADD_ERR, 0, 0);
			break;
		case COURSE_DEL_OK:
			pThis->m_courseDlg.PostMessage(WM_COURSE_DEL_OK, 0, 0);
			break;
		case COURSE_DEL_ERR:
			pThis->m_courseDlg.PostMessage(WM_COURSE_DEL_ERR, 0, 0);
			break;
		case COURSE_QUERY_RESULT:
		{
			// Receive and write the file data to a local file
			std::ofstream outputFile("received_file_course.txt", std::ios::binary);
			if (!outputFile) {
				LOGE("Error: Unable to open file for writing.");
				break;
			}

			std::streamsize totalBytesReceived = 0;
			const int bufferSize = 1024;
			char buffer[bufferSize];

			// Convert recvHdr.nLen to host byte order
			DWORD expectedFileSize = ntohl(recvHdr.nLen);

			while (totalBytesReceived < static_cast<std::streamsize>(expectedFileSize)) {
				std::streamsize bytesRead;
				if (bufferSize > expectedFileSize - totalBytesReceived) {
					bytesRead = g_tcpSocket.Receive(buffer, static_cast<int>(expectedFileSize - totalBytesReceived));
				}
				else {
					bytesRead = g_tcpSocket.Receive(buffer, bufferSize);
				}
				if (bytesRead < 0) {
					LOGE("Receive");
					break;
				}

				outputFile.write(buffer, bytesRead);
				totalBytesReceived += bytesRead;
			}

			outputFile.close();
			pThis->m_courseDlg.PostMessage(WM_COURSE_QUERY_RESULT, 0, 0);
		}
		break;
		case COURSE_QUERY_NULL:
			pThis->m_courseDlg.PostMessage(WM_COURSE_QUERY_NULL, 0, 0);
			break;
		case COURSE_QUERY_ERR:
			pThis->m_courseDlg.PostMessage(WM_COURSE_QUERY_ERR, 0, 0);
			break;
		case COURSE_EDIT_ERR:
			pThis->m_courseDlg.PostMessage(WM_COURSE_EDIT_ERR, 0, 0);
			break;
		case COURSE_EDIT_NULL:
			pThis->m_courseDlg.PostMessage(WM_COURSE_EDIT_NULL, 0, 0);
			break;
		case COURSE_EDIT_OK:
			pThis->m_courseDlg.PostMessage(WM_COURSE_EDIT_OK, 0, 0);
			break;
		case RECORD_ADD_OK:
			pThis->m_recordDlg.PostMessage(WM_RECORD_ADD_OK, 0, 0);
			break;
		case RECORD_ADD_ERR:
			pThis->m_recordDlg.PostMessage(WM_RECORD_ADD_ERR, 0, 0);
			break;
		case RECORD_DEL_OK:
			pThis->m_recordDlg.PostMessage(WM_RECORD_DEL_OK, 0, 0);
			break;
		case RECORD_DEL_ERR:
			pThis->m_recordDlg.PostMessage(WM_RECORD_DEL_ERR, 0, 0);
			break;
		case RECORD_QUERY_RESULT:
		{
			// Receive and write the file data to a local file
			std::ofstream outputFile("received_file_record.txt", std::ios::binary);
			if (!outputFile) {
				LOGE("Error: Unable to open file for writing.");
				break;
			}

			std::streamsize totalBytesReceived = 0;
			const int bufferSize = 1024;
			char buffer[bufferSize];

			// Convert recvHdr.nLen to host byte order
			DWORD expectedFileSize = ntohl(recvHdr.nLen);

			while (totalBytesReceived < static_cast<std::streamsize>(expectedFileSize)) {
				std::streamsize bytesRead;
				if (bufferSize > expectedFileSize - totalBytesReceived) {
					bytesRead = g_tcpSocket.Receive(buffer, static_cast<int>(expectedFileSize - totalBytesReceived));
				}
				else {
					bytesRead = g_tcpSocket.Receive(buffer, bufferSize);
				}
				if (bytesRead < 0) {
					LOGE("Receive");
					break;
				}

				outputFile.write(buffer, bytesRead);
				totalBytesReceived += bytesRead;
			}

			outputFile.close();
			pThis->m_recordDlg.PostMessage(WM_RECORD_QUERY_RESULT, 0, 0);
		}
		break;
		case RECORD_QUERY_NULL:
			pThis->m_recordDlg.PostMessage(WM_RECORD_QUERY_NULL, 0, 0);
			break;
		case RECORD_QUERY_ERR:
			pThis->m_recordDlg.PostMessage(WM_RECORD_QUERY_ERR, 0, 0);
			break;
		case RECORD_EDIT_ERR:
			pThis->m_recordDlg.PostMessage(WM_RECORD_EDIT_ERR, 0, 0);
			break;
		case RECORD_EDIT_NULL:
			pThis->m_recordDlg.PostMessage(WM_RECORD_EDIT_NULL, 0, 0);
			break;
		case RECORD_EDIT_OK:
			pThis->m_recordDlg.PostMessage(WM_RECORD_EDIT_OK, 0, 0);
			break;
		default:
			break;
		}
	}
}


void CMainDlg::OnSelchangeTabMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nIndex = m_TabCtrl.GetCurSel();

	switch (nIndex) {
	case 0: {
		m_stuDlg.ShowWindow(SW_NORMAL);
		m_classDlg.ShowWindow(SW_HIDE);
		m_courseDlg.ShowWindow(SW_HIDE);
		m_recordDlg.ShowWindow(SW_HIDE);
		break;
	}
	case 1: {
		m_stuDlg.ShowWindow(SW_HIDE);
		m_classDlg.ShowWindow(SW_NORMAL);
		m_courseDlg.ShowWindow(SW_HIDE);
		m_recordDlg.ShowWindow(SW_HIDE);
		break;
	}
	case 2: {
		m_stuDlg.ShowWindow(SW_HIDE);
		m_classDlg.ShowWindow(SW_HIDE);
		m_courseDlg.ShowWindow(SW_NORMAL);
		m_recordDlg.ShowWindow(SW_HIDE);
		break;
	}
	case 3: {
		m_stuDlg.ShowWindow(SW_HIDE);
		m_classDlg.ShowWindow(SW_HIDE);
		m_courseDlg.ShowWindow(SW_HIDE);
		m_recordDlg.ShowWindow(SW_NORMAL);
		break;
	}
		  *pResult = 0;
	}
}







void CMainDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}
