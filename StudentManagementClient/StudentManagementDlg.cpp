
// StudentManagementDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "StudentManagement.h"
#include "StudentManagementDlg.h"
#include "afxdialogex.h"
#include "CMainDlg.h"
#include "log.h"
#include "globalvar.h"
#include "proto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_CLOSELOGIN (WM_USER + 1)
#define  WM_LOGIN_ERR (WM_USER + 2)
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 16666


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
//	afx_msg LRESULT OnLoginErr(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_MESSAGE(WM_LOGIN_ERR, &CAboutDlg::OnLoginErr)
END_MESSAGE_MAP()


// CStudentManagementDlg 对话框



CStudentManagementDlg::CStudentManagementDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STUDENTMANAGEMENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bThreadExit = false;
}

CStudentManagementDlg::~CStudentManagementDlg()
{
	//m_bThreadExit = true;
	//if (m_pThread != nullptr && m_pThread->joinable()) {
	//	m_pThread->join();
	//	delete m_pThread;
	//}
}

void CStudentManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_PWD, m_editPwd);
}

BEGIN_MESSAGE_MAP(CStudentManagementDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CStudentManagementDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CStudentManagementDlg::OnClickedBtnLogin)
	ON_COMMAND(IDOK, &CStudentManagementDlg::OnIdok)
	ON_MESSAGE(WM_CLOSELOGIN, &CStudentManagementDlg::OnCloselogin)
	ON_MESSAGE(WM_LOGIN_ERR, &CStudentManagementDlg::OnLoginErr)
END_MESSAGE_MAP()


// CStudentManagementDlg 消息处理程序

BOOL CStudentManagementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

// TODO: Add additional initialization code here
//Set the default username and password for easy testing
	CString defaultName = _T("admin");
	CString defaultPwd = _T("123456");
	m_editUser.SetWindowText(defaultName);
	m_editPwd.SetWindowText(defaultPwd);

	//ConnectServer();
	//
	////创建工作线程
	//m_pThread = new std::thread(CStudentManagementDlg::WorkThread, this);

	//注意要写退出线程的代码

#if 0
	if (mysql_library_init(0, NULL, NULL)) {
		LOGE("mysql_library_init");
		//m_staticStats.Create(_T("could not initialize MySQL client library\r\n"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_LEFTNOWORDWRAP, CRect(10, 10, 300, 100),
		//	this);
		exit(1);
	}

	mysql_init(&g_mysql);

	//Setting the Character Set
	mysql_options(&g_mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	//reading Configuration Files
	CString configFile = _T(".\\config.ini");
	CString dbHost = GetIniValue(_T("Database"), _T("Host"), configFile);
	CString dbUser = GetIniValue(_T("Database"), _T("User"), configFile);
	CString dbPwd = GetIniValue(_T("Database"), _T("Password"), configFile);
	CString dbName = GetIniValue(_T("Database"), _T("DBName"), configFile);
	CString strPort = GetIniValue(_T("Database"), _T("Port"), configFile);
	int dbPort = atoi(CStringA(strPort));

	// First try to connect to the MySQL server without specifying a database
	if (!mysql_real_connect(&g_mysql, dbHost, dbUser, dbPwd, NULL, 0, NULL, 0)) {
		CString errorMessage;
		errorMessage.Format(_T("Failed to connect to MySQL server: Error: %s\r\n"), mysql_error(&g_mysql));
		LOGE_A("mysql_real_connect", errorMessage);
		exit(1); // Or handle errors
	}
	else {
		// Check if the database exists
		if (mysql_select_db(&g_mysql, dbName) != 0) {
			// If the database does not exist, create it
			CString createQuery;
			createQuery.Format("CREATE DATABASE %s", dbName);
			if (mysql_query(&g_mysql, createQuery)) {
				CString errorMessage;
				errorMessage.Format(_T("Failed to create database: Error: %s\r\n"), mysql_error(&g_mysql));
				LOGE_A("CREATE DATABASE", errorMessage);
				exit(1); // Or handle errors
			}

			// Now connect to the newly created database
			if (mysql_select_db(&g_mysql, dbName) != 0) {
				CString errorMessage;
				errorMessage.Format(_T("Failed to select the newly created database: Error: %s\r\n"), mysql_error(&g_mysql));
				LOGE_A("mysql_select_db", errorMessage);
				exit(1); // Or handle errors
			}
		}

		// connected to the database successfully
		CString strText = _T("Get connected to database Ok\r\n");
		LOGI(strText);
	}
#endif // 0

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CStudentManagementDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStudentManagementDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStudentManagementDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CStudentManagementDlg::OnBnClickedButton1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CStudentManagementDlg::OnClickedBtnLogin()
{
	ConnectServer();

	//Create working thread
	m_pThread = new std::thread(CStudentManagementDlg::WorkThread, this);
	CString inputName;
	m_editUser.GetWindowText(inputName);
	CString inputPwd;
	m_editPwd.GetWindowText(inputPwd);

	//MYSQL_RES* res;
	//MYSQL_ROW row;
	// 转义输入
	/*char escapedName[1024];
	mysql_real_escape_string(&g_mysql, escapedName, (LPCTSTR)inputName, inputName.GetLength());
	char escapedPwd[1024];
	mysql_real_escape_string(&g_mysql, escapedPwd, (LPCTSTR)inputPwd, inputPwd.GetLength());*/

	// prepare the query string for login
	CString query;
	query.Format("SELECT COUNT(*) FROM account WHERE user_name = '%s' AND password = '%s'", inputName, inputPwd);

	//send login query to the server
	stPacketHdr hdr;
	hdr.nCmd = MYSQL_LOGIN;
	hdr.nLen = query.GetLength();

	if (g_tcpSocket.Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("Sent header");
		return;
	}
	if (g_tcpSocket.Sent((char*)query.GetBuffer(), hdr.nLen) < 0)
	{
		LOGE("Sent query");
		return;
	}
	//if (g_socket.Send(g_pPcb, (char*)&hdr, sizeof(hdr)) < 0)
	//{
	//	LOGE("Login Send");
	//	return;
	//}
	//if (g_socket.Send(g_pPcb, query.GetBuffer(), hdr.nLen) < 0)
	//{
	//	LOGE("Login Send");
	//	return;
	//}

	//int nCurrent = GetTickCount();
	//g_socket.Update(g_pPcb, nCurrent);
	// 执行查询
#if 0
	if (mysql_query(&g_mysql, query.GetBuffer()) != 0) {

		CString errorMessage;
		errorMessage.Format(_T("Failed to query account: Error: %s\r\n"), mysql_error(&g_mysql));
		LOGE_A("mysql_real_query", errorMessage);
		AfxMessageBox("Query failed！");
		mysql_close(&g_mysql);
		return;
	}

	res = mysql_store_result(&g_mysql);

	// Get the result
	if ((row = mysql_fetch_row(res)) != NULL) {
		if (atoi(row[0]) > 0) {
			// Username and password verification successful
			EndDialog(IDCANCEL); // Close the login page
			CMainDlg cmDlg;
			cmDlg.DoModal(); // Pop up main page
		}
		else {
			//Wrong username or password
			AfxMessageBox("Wrong username or password!");
		}
	}

	// Release the result set and close the connection
	while ((res = mysql_store_result(&g_mysql)) != NULL)
	{
		mysql_free_result(res);
		if (mysql_next_result(&g_mysql) > 0) {
			break;
		}
	}
	//mysql_close(&g_mysql);

#endif // 0

}



void CStudentManagementDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}

CString CStudentManagementDlg::GetIniValue(const CString& section, const CString& key, const CString& filePath)
{
	TCHAR buffer[1024];
	GetPrivateProfileString(section, key, _T(""), buffer, 1024, filePath);
	return CString(buffer);
}



//int CStudentManagementDlg::InitSocket()
//{
//	TcpPcb* pPcb = g_socket.AllocPcb();
//	g_socket.InitSocket(pPcb, AF_INET, IPPROTO_UDP);
//	if(InitSock)
//
//	return 0;
//}


int CStudentManagementDlg::ConnectServer()
{
	if (g_tcpSocket.CreateSocket() < 0)
	{
		LOGE("CreateSocket");
		return -1;
	}
	if (g_tcpSocket.Connect(SERVER_IP, SERVER_PORT) < 0)
	{
		LOGE("connect");
		g_tcpSocket.Close();
		return -1;
	}

		//UDP 代码
	//g_pPcb = g_socket.AllocPcb();
	//if (g_pPcb == NULL)
	//{
	//	LOGE("AllocPcb");
	//	return -1;
	//}
	//if (g_socket.InitSocket(g_pPcb, AF_INET, IPPROTO_UDP) != 0)
	//{
	//	LOGE("InitSocket");
	//	g_socket.Close(g_pPcb);
	//	g_socket.FreePcb(g_pPcb);
	//	return -1;
	//}
	//if (g_socket.BindRemote(g_pPcb, "127.0.0.1", 16666) < 0)
	//{
	//	LOGE("Bind Remote");

	//	return -1;
	//}
	return 0;
}

int CStudentManagementDlg::WorkThread(CStudentManagementDlg* pThis)
{
	//Used to receive the server's reply
	//DWORD dwBegin = GetTickCount();
	//TcpPcb* pPcb = pThis->g_pPcb;
	char szBuf[50];
	int nBytes;
	while (!pThis->m_bThreadExit)
	{
		//int nCurrent = GetTickCount();
	//	g_socket.Update(g_pPcb, nCurrent);
		//g_socket.Input(g_pPcb);
		//nBytes = g_socket.Recv(g_pPcb, szBuf, sizeof(szBuf));
		//if (nBytes < 0)
		//{
		//	LOGE("Recv");
		//	return -1;
		//}
		//{
		//	std::lock_guard<std::mutex> lock(g_tcpSocketMutex); // 在此处锁定互斥锁
		//	nBytes = g_tcpSocket.Receive(szBuf, sizeof(szBuf));
		//} // 在这里离开作用域时，互斥锁自动解锁

		nBytes = g_tcpSocket.Receive(szBuf, sizeof(szBuf));
		if (nBytes < 0)
		{
			LOGE("Receive");
			return -1;
		}
		szBuf[nBytes] = '\0';

		if (strcmp(szBuf, "LOGIN_OK") == 0)
		{
			::PostMessage(pThis->m_hWnd, WM_CLOSELOGIN, 0, 0);
			break;
		} else if (strcmp(szBuf, "LOGIN_ERR") == 0)
		{
			::PostMessage(pThis->m_hWnd, WM_LOGIN_ERR, 0, 0);
		}
		else {
			continue;
		}
	}
	return 0;
}


afx_msg LRESULT CStudentManagementDlg::OnCloselogin(WPARAM wParam, LPARAM lParam)
{
	m_bThreadExit = true;
	EndDialog(IDCANCEL); // close login page
	CMainDlg cmDlg;
	cmDlg.DoModal(); // pop up main page
	
	return 0;
}


//afx_msg LRESULT CAboutDlg::OnLoginErr(WPARAM wParam, LPARAM lParam)
//{
//	AfxMessageBox("用户名或密码输入错误！");
//}


afx_msg LRESULT CStudentManagementDlg::OnLoginErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Wrong username or password!");
	return 0;
}
