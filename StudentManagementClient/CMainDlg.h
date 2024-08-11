#pragma once
#include "CStudentInfoDlg.h"
#include "CClassDlg.h"
#include "CCourseDlg.h"
#include "CRecordDlg.h"
// CMainDlg 对话框

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	CString GetIniValue(const CString& section, const CString& key, const CString& filePath);
	int InitTabCtrl();
private:
	CTabCtrl m_TabCtrl;
	CStudentInfoDlg m_stuDlg;
	CClassDlg m_classDlg;
	CCourseDlg m_courseDlg;
	CRecordDlg m_recordDlg;
	std::thread* m_pRecvThread;
public:
	afx_msg void OnDestroy();
	static void RecvThread(CMainDlg* pThis);  //后台线程，用来从服务器接收数据
	afx_msg void OnSelchangeTabMenu(NMHDR* pNMHDR, LRESULT* pResult);
protected:
//	afx_msg LRESULT OnClassAddOk(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnClassAddOk(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnClassAddErr(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnClassDelOk(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnClassDelErr(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseQueryNull(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseQueryErr(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseQueryResult(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseEditNull(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseEditOk(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseEditNull(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseEditOk(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnCourseQueryErr(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnIdok();
};
