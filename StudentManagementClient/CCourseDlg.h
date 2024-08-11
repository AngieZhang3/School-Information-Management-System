#pragma once
#include <queue>
#include "proto.h"
#include <thread>
#include <mutex>
#define WM_COURSE_ADD_OK (WM_USER + 23)
#define WM_COURSE_ADD_ERR (WM_USER + 24)
#define WM_COURSE_DEL_OK (WM_USER + 25)
#define WM_COURSE_DEL_ERR (WM_USER + 26)
#define WM_COURSE_QUERY_RESULT (WM_USER + 27)
#define WM_COURSE_QUERY_NULL (WM_USER + 28)
#define WM_COURSE_QUERY_ERR (WM_USER + 29)
#define WM_COURSE_EDIT_ERR (WM_USER + 30)
#define WM_COURSE_EDIT_NULL (WM_USER + 31)
#define WM_COURSE_EDIT_OK (WM_USER + 32)

// CCourseDlg 对话框

class CCourseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCourseDlg)

public:
	CCourseDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCourseDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COURSE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCourse;
	CEdit m_editCourseId;
	CEdit m_editCourseName;
	virtual BOOL OnInitDialog();
	int InitList();
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
	static void BackgroundThread(CCourseDlg* pThis);  //后台线程，用来和服务器发数据
	int ReadFileAndDisplay(CListCtrl& lstControl);
protected:
	afx_msg LRESULT OnCourseAddOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseAddErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseDelOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseDelErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseQueryResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseQueryNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseQueryErr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseEditNull(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseEditOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCourseEditErr(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClickListCourse(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIdok();
};
