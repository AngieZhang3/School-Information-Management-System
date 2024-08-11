#pragma once
#include <queue>
#include "proto.h"
#include <thread>
#include <mutex>
#define WM_RECORD_ADD_OK (WM_USER + 33)
#define WM_RECORD_ADD_ERR (WM_USER + 34)
#define WM_RECORD_DEL_OK (WM_USER + 35)
#define WM_RECORD_DEL_ERR (WM_USER + 36)
#define WM_RECORD_QUERY_RESULT (WM_USER + 37)
#define WM_RECORD_QUERY_NULL (WM_USER + 38)
#define WM_RECORD_QUERY_ERR (WM_USER + 39)
#define WM_RECORD_EDIT_ERR (WM_USER + 40)
#define WM_RECORD_EDIT_NULL (WM_USER + 41)
#define WM_RECORD_EDIT_OK (WM_USER + 42)

// CRecordDlg 对话框

class CRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRecordDlg)

public:
	CRecordDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRecordDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RECORD_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int InitList();
private:
	std::thread* m_pThread;
	std::queue<stQueryRequest> m_QueryQueue;
	std::mutex m_QueryMutex;
	std::condition_variable m_QueryCondition;
public:
	static void BackgroundThread(CRecordDlg* pThis);  //后台线程，用来和服务器发数据
	int ReadFileAndDisplay(CListCtrl& lstControl);
	CListCtrl m_listRecord;
	CEdit m_editSID;
	CEdit m_editCID;
	afx_msg void OnClickListRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickedBtnAdd();
	afx_msg void OnClickedBtnDel();
	afx_msg void OnClickedBtnEdit();
	afx_msg void OnClickedBtnQuery();
protected:
	afx_msg LRESULT OnRecordAddOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordAddErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordDelOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordDelErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordQueryResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordQueryNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordQueryErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordEditNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordEditErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecordEditOk(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnIdok();
};
