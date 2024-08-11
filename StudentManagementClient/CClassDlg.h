#pragma once
#include <queue>
#include "proto.h"
#include <thread>
#include <mutex>
#define WM_CLASS_ADD_OK (WM_USER + 13)
#define WM_CLASS_ADD_ERR (WM_USER + 14)
#define WM_CLASS_DEL_OK (WM_USER + 15)
#define WM_CLASS_DEL_ERR (WM_USER + 16)
#define WM_CLASS_QUERY_RESULT (WM_USER + 17)
#define WM_CLASS_QUERY_NULL (WM_USER + 18)
#define WM_CLASS_QUERY_ERR (WM_USER + 19)
#define WM_CLASS_EDIT_ERR (WM_USER + 20)
#define WM_CLASS_EDIT_NULL (WM_USER + 21)
#define WM_CLASS_EDIT_OK (WM_USER + 22)
// CClassDlg 对话框

class CClassDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClassDlg)

public:
	CClassDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CClassDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLASS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listClass;
	CEdit m_editClassId;
	CEdit m_editClassName;


public:
	int InitList();
	virtual BOOL OnInitDialog();
	static void BackgroundThread(CClassDlg* pThis);  //后台线程，用来和服务器发数据
private:
	std::thread* m_pThread;
	std::queue<stQueryRequest> m_QueryQueue;
	std::mutex m_QueryMutex;
	std::condition_variable m_QueryCondition;
public:
	afx_msg void OnClickedBtnAdd();
	afx_msg void OnClickedBtnDel();
	afx_msg void OnClickedBtnEdit();
	afx_msg void OnClickedBtnQuery();
	afx_msg void OnClickListClass(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	afx_msg LRESULT OnClassAddOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassAddErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassDelOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassDelErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassQueryResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassQueryNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassQueryErr(WPARAM wParam, LPARAM lParam);
public:
	int ReadFileAndDisplay(CListCtrl& lstControl);
protected:
	afx_msg LRESULT OnClassEditErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassEditNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClassEditOk(WPARAM wParam, LPARAM lParam);
public:
//	afx_msg void OnDestroy();
	afx_msg void OnIdok();
};
