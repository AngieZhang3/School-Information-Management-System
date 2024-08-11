#pragma once

#include <queue>
#include "proto.h"
#include <thread>
#include <mutex>
//#include "CTcpSocket.h"
// CStudentInfoDlg 对话框
#define WM_STU_ADD_OK (WM_USER + 3)
#define WM_STU_ADD_ERR (WM_USER + 4)
#define WM_STU_DEL_OK (WM_USER + 5)
#define WM_STU_DEL_ERR (WM_USER + 6)
#define WM_STU_QUERY_RESULT (WM_USER + 7)
#define WM_STU_QUERY_NULL (WM_USER + 8)
#define WM_STU_QUERY_ERR (WM_USER + 9)
#define WM_STU_EDIT_ERR (WM_USER + 10)
#define WM_STU_EDIT_NULL (WM_USER + 11)
#define WM_STU_EDIT_OK (WM_USER + 12)


class CStudentInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStudentInfoDlg)

public:
	CStudentInfoDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStudentInfoDlg();


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDENTINFO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listStuInfo;
	virtual BOOL OnInitDialog();
	int InitList();
	CEdit m_editSID;
	CEdit m_editName;
	CEdit m_editClassID;
	afx_msg void OnClickedBtnAdd();

private:
	std::thread* m_pThread;
	std::thread* m_pRecvThread;
	
	//CTcpSocket m_socket;
public:
	afx_msg void OnClickedBtnQuery();
//	afx_msg void OnEnChangeEditClassid();
	afx_msg void OnClickListStuinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickedBtnDel();
	afx_msg void OnClickedBtnEdit();
	afx_msg void OnIdok();
//	afx_msg void OnDestroy();
	// Declare a thread-safe queue to store query requests
	std::queue<stQueryRequest> m_QueryQueue;
	std::mutex m_QueryMutex;
	std::condition_variable m_QueryCondition;
	static void BackgroundThread(CStudentInfoDlg *pThis);  //Background thread, used to send data to the server
//	static void RecvThread(CStudentInfoDlg *pThis);  //Background thread, used to receive data from the server

protected:
	afx_msg LRESULT OnAddOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuDelOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuDelErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuQueryResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuQueryNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuQueryErr(WPARAM wParam, LPARAM lParam);
public:
	int ReadFileAndDisplay(CListCtrl &lstControl);
protected:
	afx_msg LRESULT OnStuEditErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuEditNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStuEditOk(WPARAM wParam, LPARAM lParam);
};
