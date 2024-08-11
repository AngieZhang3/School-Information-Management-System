// CClassDlg.cpp: 实现文件
//

#include "pch.h"
#include "StudentManagement.h"
#include "CClassDlg.h"
#include "afxdialogex.h"
#include "StudentManagement.h"
#include "CStudentInfoDlg.h"
#include "afxdialogex.h"
#include "globalvar.h"
#include "log.h"
#include <fstream>
#include <vector>

// CClassDlg 对话框

IMPLEMENT_DYNAMIC(CClassDlg, CDialogEx)

CClassDlg::CClassDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLASS_DLG, pParent)
{

}

CClassDlg::~CClassDlg()
{
}

void CClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLASS, m_listClass);
	DDX_Control(pDX, IDC_EDIT_CID, m_editClassId);
	DDX_Control(pDX, IDC_EDIT_CNAME, m_editClassName);
}


BEGIN_MESSAGE_MAP(CClassDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CClassDlg::OnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CClassDlg::OnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_EDIT, &CClassDlg::OnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CClassDlg::OnClickedBtnQuery)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CLASS, &CClassDlg::OnClickListClass)
	ON_MESSAGE(WM_CLASS_ADD_OK, &CClassDlg::OnClassAddOk)
	ON_MESSAGE(WM_CLASS_ADD_ERR, &CClassDlg::OnClassAddErr)
	ON_MESSAGE(WM_CLASS_DEL_OK, &CClassDlg::OnClassDelOk)
	ON_MESSAGE(WM_CLASS_DEL_ERR, &CClassDlg::OnClassDelErr)
	ON_MESSAGE(WM_CLASS_QUERY_RESULT, &CClassDlg::OnClassQueryResult)
	ON_MESSAGE(WM_CLASS_QUERY_NULL, &CClassDlg::OnClassQueryNull)
	ON_MESSAGE(WM_CLASS_QUERY_ERR, &CClassDlg::OnClassQueryErr)
	ON_MESSAGE(WM_CLASS_EDIT_ERR, &CClassDlg::OnClassEditErr)
	ON_MESSAGE(WM_CLASS_EDIT_NULL, &CClassDlg::OnClassEditNull)
	ON_MESSAGE(WM_CLASS_EDIT_OK, &CClassDlg::OnClassEditOk)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK, &CClassDlg::OnIdok)
END_MESSAGE_MAP()


// CClassDlg 消息处理程序


int CClassDlg::InitList()
{
	int nIdx = 0;
	m_listClass.InsertColumn(nIdx++, "Class ID");
	m_listClass.InsertColumn(nIdx++, "Class Name");



	//自动调整列宽
	int nColCount = m_listClass.GetHeaderCtrl()->GetItemCount();
	for (size_t i = 0; i < nColCount; i++)
	{
		m_listClass.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	//设置风格
	m_listClass.SetExtendedStyle(
		m_listClass.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);
	return 0;
}


BOOL CClassDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitList();
	m_pThread = new std::thread(CClassDlg::BackgroundThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CClassDlg::BackgroundThread(CClassDlg* pThis)
{
	stPacketHdr sentHdr;


	while (true)
	{
		//检查查询队列
		std::unique_lock<std::mutex> lock(pThis->m_QueryMutex);
		// 使用 lambda 函数检查队列是否为空
		pThis->m_QueryCondition.wait(lock, [pThis] { return !(pThis->m_QueryQueue.empty()); });
		stQueryRequest request = pThis->m_QueryQueue.front();
		pThis->m_QueryQueue.pop();
		if (g_tcpSocket.Sent((char*)&request.hdr, sizeof(request.hdr)) < 0)
		{
			LOGE("sent");
			continue;
		}
		if (g_tcpSocket.Sent((char*)request.query.GetBuffer(), request.hdr.nLen) < 0)
		{
			LOGE("sent");
			continue;
		}

	}
}


void CClassDlg::OnClickedBtnAdd()
{
	CString CID;
	m_editClassId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
	}

	CString cName;
	m_editClassName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter class name");
	}

	//fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = CLASS_ADD;
	queryRequest.query.Format(_T("INSERT INTO class VALUES('%s', '%s')"), CID, cName);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//通知后台线程有新请求
	m_QueryCondition.notify_one();
}


void CClassDlg::OnClickedBtnDel()
{
	CString CID;
	m_editClassId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
	}

	CString cName;
	m_editClassName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter class name");
	}


	//fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = CLASS_DEL;
	queryRequest.query.Format(_T("DELETE FROM class WHERE id = '%s' AND name = '%s'"), CID, cName);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CClassDlg::OnClickedBtnEdit()
{
	CString CID;
	m_editClassId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
	}

	CString cName;
	m_editClassName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter class name");
	}
	

	//First check whether the student ID exists 
	//(if it exists, it can be changed. If it does not exist, SQL will not change it, 
	//but it will not report an error)

	stQueryRequest queryRequest_checkId;
	queryRequest_checkId.hdr.nCmd = CLASS_EDIT_CHECK;
	queryRequest_checkId.query.Format(_T("SELECT COUNT(*) FROM class WHERE id = '%s'"), CID);
	queryRequest_checkId.hdr.nLen = queryRequest_checkId.query.GetLength();

	//fill in stQueryRequest structure
	//modified sql statement
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = CLASS_EDIT;
	queryRequest.query.Format(_T("UPDATE class SET name = '%s' WHERE id = '%s'"), cName, CID);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest_checkId);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();

}


void CClassDlg::OnClickedBtnQuery()
{
	stQueryRequest queryRequest;
	CString CID;
	m_editClassId.GetWindowText(CID);
	CString cName;
	m_editClassName.GetWindowTextA(cName);


	queryRequest.query.Format(_T("SELECT * FROM class where 1= 1 "));

	CString strAdd;
	if (!CID.IsEmpty())
	{
		strAdd.Format("AND id = '%s' ", CID);
		queryRequest.query += strAdd;
		strAdd = "";
	}
	if (!cName.IsEmpty())
	{
		strAdd.Format("AND name='%s' ", cName);
		queryRequest.query += strAdd;
		strAdd = "";
	}


	//fill in stQueryRequest structure

	queryRequest.hdr.nCmd = CLASS_QUERY;
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CClassDlg::OnClickListClass(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nSelected = m_listClass.GetSelectionMark();
	if (nSelected != -1)
	{
		CString cId = m_listClass.GetItemText(nSelected, 0);
		CString cName = m_listClass.GetItemText(nSelected, 1);
		m_editClassId.SetWindowText(cId);
		m_editClassName.SetWindowText(cName);

	}
	*pResult = 0;
}


afx_msg LRESULT CClassDlg::OnClassAddOk(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Added class information successfully");
	m_editClassId.SetWindowText(_T(""));
	m_editClassName.SetWindowText(_T(""));

	return 0;
}


afx_msg LRESULT CClassDlg::OnClassAddErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to add class information");
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassDelOk(WPARAM wParam, LPARAM lParam)
{
	int nSelected = m_listClass.GetNextItem(-1, LVNI_SELECTED);
	m_listClass.DeleteItem(nSelected);
	m_editClassId.SetWindowText(_T(""));
	m_editClassName.SetWindowText(_T(""));


	return 0;
}


afx_msg LRESULT CClassDlg::OnClassDelErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to delete class information");
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassQueryResult(WPARAM wParam, LPARAM lParam)
{
	//delete existing data
	m_listClass.DeleteAllItems();

	//Read file and display on control list
	ReadFileAndDisplay(m_listClass);

	m_editClassId.SetWindowText("");
	m_editClassName.SetWindowText("");


	return 0;
}


afx_msg LRESULT CClassDlg::OnClassQueryNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("No results found");
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassQueryErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Query error");
	return 0;
}


int CClassDlg::ReadFileAndDisplay(CListCtrl& lstControl)
{
	std::ifstream file("received_file_class.txt");
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::vector<std::string> fields;
			std::istringstream iss(line);
			std::string field;

			while (std::getline(iss, field, '\t'))
			{
				fields.push_back(field);
			}

			if (fields.size() >= 2)
			{
				int index = lstControl.InsertItem(lstControl.GetItemCount(), fields[0].c_str()); // convert to CString
				lstControl.SetItemText(index, 1, CString(fields[1].c_str())); // Convert to CString
			

			}
		}
		file.close();
	}
	else {
		LOGE("is_open");
	}
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassEditErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Error modifying data");
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassEditNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Unable to modify a non-existent class number, please add it first");
	return 0;
}


afx_msg LRESULT CClassDlg::OnClassEditOk(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//void CClassDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//	g_tcpSocket.Close();
//	// TODO: 在此处添加消息处理程序代码
//}


void CClassDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}
