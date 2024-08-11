
// StudentManagementDlg.h: 头文件
//

#pragma once
#include <thread>
#include "CTcpSocket.h"
// CStudentManagementDlg 对话框
class CStudentManagementDlg : public CDialogEx
{
// 构造
public:
	CStudentManagementDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CStudentManagementDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDENTMANAGEMENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedButton1();
	CEdit m_editUser;
	CEdit m_editPwd;
	afx_msg void OnClickedBtnLogin();
	afx_msg void OnIdok();
	CString GetIniValue(const CString& section, const CString& key, const CString& filePath);
public:

//	int InitSocket();
	int ConnectServer();
	static int WorkThread(CStudentManagementDlg* pThis);


protected:
	afx_msg LRESULT OnCloselogin(WPARAM wParam, LPARAM lParam);
private:
	std::thread* m_pThread;
//	CTcpSocket m_socket;
	bool m_bThreadExit = false;
protected:
	afx_msg LRESULT OnLoginErr(WPARAM wParam, LPARAM lParam);
};
