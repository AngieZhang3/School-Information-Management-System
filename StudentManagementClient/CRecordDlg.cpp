// CRecordDlg.cpp: 实现文件
//

#include "pch.h"
#include "StudentManagement.h"
#include "CRecordDlg.h"
#include "afxdialogex.h"
#include "StudentManagement.h"
#include "globalvar.h"
#include "log.h"
#include <fstream>
#include <vector>
// CRecordDlg 对话框

IMPLEMENT_DYNAMIC(CRecordDlg, CDialogEx)

CRecordDlg::CRecordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RECORD_DLG, pParent)
{

}

CRecordDlg::~CRecordDlg()
{
}

void CRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECORD, m_listRecord);
	DDX_Control(pDX, IDC_EDIT_SID, m_editSID);
	DDX_Control(pDX, IDC_EDIT_CID, m_editCID);
}


BEGIN_MESSAGE_MAP(CRecordDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RECORD, &CRecordDlg::OnClickListRecord)
	ON_BN_CLICKED(IDC_BTN_ADD, &CRecordDlg::OnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CRecordDlg::OnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_EDIT, &CRecordDlg::OnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CRecordDlg::OnClickedBtnQuery)
	ON_MESSAGE(WM_RECORD_ADD_OK, &CRecordDlg::OnRecordAddOk)
	ON_MESSAGE(WM_RECORD_ADD_ERR, &CRecordDlg::OnRecordAddErr)
	ON_MESSAGE(WM_RECORD_DEL_OK, &CRecordDlg::OnRecordDelOk)
	ON_MESSAGE(WM_RECORD_DEL_ERR, &CRecordDlg::OnRecordDelErr)
	ON_MESSAGE(WM_RECORD_QUERY_RESULT, &CRecordDlg::OnRecordQueryResult)
	ON_MESSAGE(WM_RECORD_QUERY_NULL, &CRecordDlg::OnRecordQueryNull)
	ON_MESSAGE(WM_RECORD_QUERY_ERR, &CRecordDlg::OnRecordQueryErr)
	ON_MESSAGE(WM_RECORD_EDIT_NULL, &CRecordDlg::OnRecordEditNull)
	ON_MESSAGE(WM_RECORD_EDIT_ERR, &CRecordDlg::OnRecordEditErr)
	ON_MESSAGE(WM_RECORD_EDIT_OK, &CRecordDlg::OnRecordEditOk)
	ON_COMMAND(IDOK, &CRecordDlg::OnIdok)
END_MESSAGE_MAP()


// CRecordDlg 消息处理程序


BOOL CRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitList();
	m_pThread = new std::thread(CRecordDlg::BackgroundThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

int CRecordDlg::InitList()
{
	int nIdx = 0;
	m_listRecord.InsertColumn(nIdx++, "Student ID");
	m_listRecord.InsertColumn(nIdx++, "Course ID");



	//Automatically adjust column width
	int nColCount = m_listRecord.GetHeaderCtrl()->GetItemCount();
	for (size_t i = 0; i < nColCount; i++)
	{
		m_listRecord.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	//Set style
	m_listRecord.SetExtendedStyle(
		m_listRecord.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);
	return 0;
}

void CRecordDlg::BackgroundThread(CRecordDlg* pThis)
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

int CRecordDlg::ReadFileAndDisplay(CListCtrl& lstControl)
{
	std::ifstream file("received_file_record.txt");
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
				int index = lstControl.InsertItem(lstControl.GetItemCount(), fields[0].c_str()); // Convert to CString
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


void CRecordDlg::OnClickListRecord(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int nSelected = m_listRecord.GetSelectionMark();
	if (nSelected != -1)
	{
		CString sId = m_listRecord.GetItemText(nSelected, 0);
		CString cId = m_listRecord.GetItemText(nSelected, 1);
		m_editSID.SetWindowText(sId);
		m_editCID.SetWindowText(cId);

	}
	*pResult = 0;
}


void CRecordDlg::OnClickedBtnAdd()
{
	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter student ID");
	}
	int sId = atoi(SID);
	CString CID;
	m_editCID.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}


	//fill in stQueryRequest struct
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = RECORD_ADD;
	queryRequest.query.Format(_T("INSERT INTO record VALUES( %d, '%s')"), sId, CID);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CRecordDlg::OnClickedBtnDel()
{
	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter student ID");
	}
	int sId = atoi(SID);
	CString CID;
	m_editCID.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}

	//fill in stQueryRequest struct
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = RECORD_DEL;
	queryRequest.query.Format(_T("DELETE FROM record WHERE record_sid = %d AND record_cid = '%s'"), sId, CID);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CRecordDlg::OnClickedBtnEdit()
{
	
	CString SID;
	m_editSID.GetWindowText(SID);
	if (SID.IsEmpty())
	{
		AfxMessageBox("Please enter student ID");
	}
	int sId = atoi(SID);
	CString CID;
	m_editCID.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}

	//First check whether the student ID exists (if it exists, it can be changed. If it does not exist, SQL will not change it, but it will not report an error)

	stQueryRequest queryRequest_checkId;
	queryRequest_checkId.hdr.nCmd = RECORD_EDIT_CHECK;
	queryRequest_checkId.query.Format(_T("SELECT COUNT(*) FROM record WHERE record_sid = % d"), sId);
	queryRequest_checkId.hdr.nLen = queryRequest_checkId.query.GetLength();

	//fill in stQueryRequest struct
	//modify sql statement
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = RECORD_EDIT;
	queryRequest.query.Format(_T("UPDATE record SET record_cid = '%s' WHERE record_sid = %d"), CID,  sId);
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


void CRecordDlg::OnClickedBtnQuery()
{
	stQueryRequest queryRequest;
	CString SID;
	m_editSID.GetWindowText(SID);
	CString CID;
	m_editCID.GetWindowTextA(CID);

	queryRequest.query.Format(_T("SELECT * FROM record where 1= 1 "));

	CString strAdd;
	if (!SID.IsEmpty())
	{
		strAdd.Format("AND record_sid = %d ", atoi(SID));
		queryRequest.query += strAdd;
		strAdd = "";
	}
	if (!CID.IsEmpty())
	{
		strAdd.Format("AND record_cid='%s' ", CID);
		queryRequest.query += strAdd;
		strAdd = "";
	}


	//fill in stQueryRequest struct

	queryRequest.hdr.nCmd = RECORD_QUERY;
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


afx_msg LRESULT CRecordDlg::OnRecordAddOk(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Added course selection information successfully");
	m_editSID.SetWindowText(_T(""));
	m_editCID.SetWindowText(_T(""));

	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordAddErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to add course selection information");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordDelOk(WPARAM wParam, LPARAM lParam)
{
	int nSelected = m_listRecord.GetNextItem(-1, LVNI_SELECTED);
	m_listRecord.DeleteItem(nSelected);
	m_editSID.SetWindowText(_T(""));
	m_editCID.SetWindowText(_T(""));

	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordDelErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to delete course selection information");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordQueryResult(WPARAM wParam, LPARAM lParam)
{
	// clear all existing data
	m_listRecord.DeleteAllItems();

	//read the file and display data in list control
	ReadFileAndDisplay(m_listRecord);

	m_editSID.SetWindowText("");
	m_editCID.SetWindowText("");

	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordQueryNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("No result");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordQueryErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Query error");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordEditNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Unable to modify a non-existent student ID, please add it first");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordEditErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Error modifying data");
	return 0;
}


afx_msg LRESULT CRecordDlg::OnRecordEditOk(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CRecordDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}
