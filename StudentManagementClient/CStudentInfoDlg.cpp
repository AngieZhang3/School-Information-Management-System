// CStudentInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include "StudentManagement.h"
#include "CStudentInfoDlg.h"
#include "afxdialogex.h"
#include "globalvar.h"
#include "log.h"
#include <fstream>
#include <vector>
//#define SERVER_IP "127.0.0.1"
//#define SERVER_PORT 16666

// CStudentInfoDlg 对话框

IMPLEMENT_DYNAMIC(CStudentInfoDlg, CDialogEx)

CStudentInfoDlg::CStudentInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STUDENTINFO_DLG, pParent)
{

}

CStudentInfoDlg::~CStudentInfoDlg()
{
}



void CStudentInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STUINFO, m_listStuInfo);
	DDX_Control(pDX, IDC_EDIT_SID, m_editSID);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_CLASSID, m_editClassID);
}


BEGIN_MESSAGE_MAP(CStudentInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CStudentInfoDlg::OnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CStudentInfoDlg::OnClickedBtnQuery)
	//ON_EN_CHANGE(IDC_EDIT_CLASSID, &CStudentInfoDlg::OnEnChangeEditClassid)
	ON_NOTIFY(NM_CLICK, IDC_LIST_STUINFO, &CStudentInfoDlg::OnClickListStuinfo)
	ON_BN_CLICKED(IDC_BTN_DEL, &CStudentInfoDlg::OnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_EDIT, &CStudentInfoDlg::OnClickedBtnEdit)
	ON_COMMAND(IDOK, &CStudentInfoDlg::OnIdok)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_STU_ADD_OK, &CStudentInfoDlg::OnAddOk)
	ON_MESSAGE(WM_STU_ADD_ERR, &CStudentInfoDlg::OnAddErr)
	ON_MESSAGE(WM_STU_DEL_OK, &CStudentInfoDlg::OnStuDelOk)
	ON_MESSAGE(WM_STU_DEL_ERR, &CStudentInfoDlg::OnStuDelErr)
	ON_MESSAGE(WM_STU_QUERY_RESULT, &CStudentInfoDlg::OnStuQueryResult)
	ON_MESSAGE(WM_STU_QUERY_NULL, &CStudentInfoDlg::OnStuQueryNull)
	ON_MESSAGE(WM_STU_QUERY_ERR, &CStudentInfoDlg::OnStuQueryErr)
	ON_MESSAGE(WM_STU_EDIT_ERR, &CStudentInfoDlg::OnStuEditErr)
	ON_MESSAGE(WM_STU_EDIT_NULL, &CStudentInfoDlg::OnStuEditNull)
	ON_MESSAGE(WM_STU_EDIT_OK, &CStudentInfoDlg::OnStuEditOk)
END_MESSAGE_MAP()


// CStudentInfoDlg 消息处理程序


BOOL CStudentInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//if (g_tcpSocket.CreateSocket() < 0)
	//{
	//	LOGE("CreateSocket");
	//	return -1;
	//}
	//if (g_tcpSocket.Connect(SERVER_IP, SERVER_PORT) < 0)
	//{
	//	LOGE("connect");
	//	g_tcpSocket.Close();
	//	return -1;
	//}

	InitList();
	//创建负责发送数据的线程
	m_pThread = new std::thread(CStudentInfoDlg::BackgroundThread, this);
	//创建负责收消息的线程
	//m_pRecvThread = new std::thread(CStudentInfoDlg::RecvThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



int CStudentInfoDlg::InitList()
{
	int nIdx = 0;
	m_listStuInfo.InsertColumn(nIdx++, "Student ID");
	m_listStuInfo.InsertColumn(nIdx++, "Name");
	m_listStuInfo.InsertColumn(nIdx++, "Class ID");


	//Automatically adjust column width
	int nColCount = m_listStuInfo.GetHeaderCtrl()->GetItemCount();
	for (size_t i = 0; i < nColCount; i++)
	{
		m_listStuInfo.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	//Set style
	m_listStuInfo.SetExtendedStyle(
		m_listStuInfo.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);
	return 0;
}


void CStudentInfoDlg::OnClickedBtnAdd()
{
	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter ID");
	}
	int sId = atoi(SID);
	CString sName;
	m_editName.GetWindowText(sName);
	if (sName.IsEmpty())
	{
		AfxMessageBox("Please enter name");
	}

	CString classID;
	m_editClassID.GetWindowText(classID);
	if (classID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
	}

	// fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = STU_ADD;
	queryRequest.query.Format(_T("INSERT INTO student VALUES(%d, '%s', '%s')"), sId, sName, classID);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();

}




void CStudentInfoDlg::OnClickedBtnQuery()
{

	stQueryRequest queryRequest;
	CString SID;
	m_editSID.GetWindowText(SID);
	CString name;
	m_editName.GetWindowTextA(name);
	CString classID;
	m_editClassID.GetWindowTextA(classID);

	queryRequest.query.Format(_T("SELECT * FROM student where 1= 1 "));

	CString strAdd;
	if (!SID.IsEmpty())
	{
		strAdd.Format("AND sid = %d ", atoi(SID));
		queryRequest.query += strAdd;
		strAdd = "";
	}
	if (!name.IsEmpty())
	{
		strAdd.Format("AND name='%s' ", name);
		queryRequest.query += strAdd;
		strAdd = "";
	}
	if (!classID.IsEmpty())
	{
		strAdd.Format("AND class_id='%s' ", classID);
		queryRequest.query += strAdd;
		strAdd = "";
	}


	//fill in stQueryRequest structure

	queryRequest.hdr.nCmd = STU_QUERY;
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();


}


void CStudentInfoDlg::OnClickListStuinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nSelected = m_listStuInfo.GetSelectionMark();
	if (nSelected != -1)
	{
		CString sid = m_listStuInfo.GetItemText(nSelected, 0);
		CString name = m_listStuInfo.GetItemText(nSelected, 1);
		CString classID = m_listStuInfo.GetItemText(nSelected, 2);
		m_editSID.SetWindowText(sid);
		m_editName.SetWindowText(name);
		m_editClassID.SetWindowText(classID);

	}
	*pResult = 0;
}


void CStudentInfoDlg::OnClickedBtnDel()
{
	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter ID");
	}
	int sId = atoi(SID);
	CString sName;
	m_editName.GetWindowText(sName);
	if (sName.IsEmpty())
	{
		AfxMessageBox("Please enter name");
	}

	CString classID;
	m_editClassID.GetWindowText(classID);
	if (classID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
	}

	//fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = STU_DEL;
	queryRequest.query.Format(_T("DELETE FROM student WHERE sid = %d AND name = '%s' AND class_id = '%s'"), sId, sName, classID);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();

	//int nSelected = m_listStuInfo.GetNextItem(-1, LVNI_SELECTED);
	//m_listStuInfo.DeleteItem(nSelected);
	//m_editSID.SetWindowText(_T(""));
	//m_editName.SetWindowText(_T(""));
	//m_editClassID.SetWindowTextA(_T(""));

}


void CStudentInfoDlg::OnClickedBtnEdit()
{
	////Save the original ID of the selected student first
	//int nSelected = m_listStuInfo.GetNextItem(-1, LVNI_SELECTED);
	//CString oldSID = m_listStuInfo.GetItemText(nSelected, 0);
	//int oldSId = _ttoi(oldSID);

	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter the ID to be modified");
		return;  
	}
	int sId = atoi(SID);

	CString name;
	m_editName.GetWindowText(name);
	if (name.IsEmpty())
	{
		AfxMessageBox("Please enter name");
		return;  
	}

	CString classID;
	m_editClassID.GetWindowText(classID);
	if (classID.IsEmpty())
	{
		AfxMessageBox("Please enter class ID");
		return;
	}

	//First check whether the student ID exists 
	//(if it exists, it can be changed.
	//If it does not exist, SQL will not change it, but it will not report an error)


	stQueryRequest queryRequest_checkId;
	queryRequest_checkId.hdr.nCmd = STU_EDIT_CHECK;
	queryRequest_checkId.query.Format(_T("SELECT COUNT(*) FROM student WHERE sid = % d"), sId);
	queryRequest_checkId.hdr.nLen = queryRequest_checkId.query.GetLength();

	//fill in stQueryRequest structure
	//modified sql statement
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = STU_EDIT;
	queryRequest.query.Format(_T("UPDATE student SET name = '%s', class_id = '%s' WHERE sid = %d"), name, classID, sId);
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


void CStudentInfoDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


//void CStudentInfoDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//	g_tcpSocket.Close();
//	// TODO: 在此处添加消息处理程序代码
//}

void CStudentInfoDlg::BackgroundThread(CStudentInfoDlg* pThis)
{
	stPacketHdr sentHdr;


	while (true)
	{
		//check query queue
		std::unique_lock<std::mutex> lock(pThis->m_QueryMutex);
		// use lambda function to check if the queue is empty
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




afx_msg LRESULT CStudentInfoDlg::OnAddOk(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Add student information successfully");
	m_editSID.SetWindowText(_T(""));
	m_editName.SetWindowText(_T(""));
	m_editClassID.SetWindowTextA(_T(""));
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnAddErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to add student information");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuDelOk(WPARAM wParam, LPARAM lParam)
{
	int nSelected = m_listStuInfo.GetNextItem(-1, LVNI_SELECTED);
	m_listStuInfo.DeleteItem(nSelected);
	m_editSID.SetWindowText(_T(""));
	m_editName.SetWindowText(_T(""));
	m_editClassID.SetWindowTextA(_T(""));
	//AfxMessageBox("删除学生信息成功");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuDelErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to delete student information");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuQueryResult(WPARAM wParam, LPARAM lParam)
{
	// remove all existing data
	m_listStuInfo.DeleteAllItems();

	//read file and displayed the data in list control
	ReadFileAndDisplay(m_listStuInfo);

	m_editSID.SetWindowText("");
	m_editName.SetWindowText("");
	m_editClassID.SetWindowText("");

	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuQueryNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("No result");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuQueryErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Query error");
	return 0;
}


int CStudentInfoDlg::ReadFileAndDisplay(CListCtrl& lstControl)
{
	std::ifstream file("received_file.txt");
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

			if (fields.size() >= 3)
			{
				int index = lstControl.InsertItem(lstControl.GetItemCount(), fields[0].c_str()); // convert to CString
				lstControl.SetItemText(index, 1, CString(fields[1].c_str())); // convert to CString
				lstControl.SetItemText(index, 2, CString(fields[2].c_str())); // convert to CString

			}
		}
		file.close();
	}
	else {
		LOGE("is_open");
	}
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuEditErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Error modifying data");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuEditNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Unable to modify a non-existent student ID, please add it first");
	return 0;
}


afx_msg LRESULT CStudentInfoDlg::OnStuEditOk(WPARAM wParam, LPARAM lParam)
{
//	AfxMessageBox("修改成功");
	return 0;
}
