// CCourseDlg.cpp: 实现文件
//

#include "pch.h"
#include "StudentManagement.h"
#include "CClassDlg.h"
#include "CCourseDlg.h"
#include "afxdialogex.h"
#include "StudentManagement.h"
#include "globalvar.h"
#include "log.h"
#include <fstream>
#include <vector>

// CCourseDlg 对话框

IMPLEMENT_DYNAMIC(CCourseDlg, CDialogEx)

CCourseDlg::CCourseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COURSE_DLG, pParent)
{

}

CCourseDlg::~CCourseDlg()
{
}

void CCourseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COURSE, m_listCourse);
	DDX_Control(pDX, IDC_EDIT_COURSEID, m_editCourseId);
	DDX_Control(pDX, IDC_EDIT_COURSENAME, m_editCourseName);
}


BEGIN_MESSAGE_MAP(CCourseDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CCourseDlg::OnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CCourseDlg::OnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_EDIT, &CCourseDlg::OnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CCourseDlg::OnClickedBtnQuery)
	ON_MESSAGE(WM_COURSE_ADD_OK, &CCourseDlg::OnCourseAddOk)
	ON_MESSAGE(WM_COURSE_ADD_ERR, &CCourseDlg::OnCourseAddErr)
	ON_MESSAGE(WM_COURSE_DEL_OK, &CCourseDlg::OnCourseDelOk)
	ON_MESSAGE(WM_COURSE_DEL_ERR, &CCourseDlg::OnCourseDelErr)
	ON_MESSAGE(WM_COURSE_QUERY_RESULT, &CCourseDlg::OnCourseQueryResult)
	ON_MESSAGE(WM_COURSE_QUERY_NULL, &CCourseDlg::OnCourseQueryNull)
	ON_MESSAGE(WM_COURSE_QUERY_ERR, &CCourseDlg::OnCourseQueryErr)
	ON_MESSAGE(WM_COURSE_EDIT_NULL, &CCourseDlg::OnCourseEditNull)
	ON_MESSAGE(WM_COURSE_EDIT_OK, &CCourseDlg::OnCourseEditOk)
	ON_MESSAGE(WM_COURSE_EDIT_ERR, &CCourseDlg::OnCourseEditErr)
	ON_NOTIFY(NM_CLICK, IDC_LIST_COURSE, &CCourseDlg::OnClickListCourse)
	ON_COMMAND(IDOK, &CCourseDlg::OnIdok)
END_MESSAGE_MAP()


// CCourseDlg 消息处理程序


BOOL CCourseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitList();
	m_pThread = new std::thread(CCourseDlg::BackgroundThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
int CCourseDlg::InitList()
{
	int nIdx = 0;
	m_listCourse.InsertColumn(nIdx++, "Course ID");
	m_listCourse.InsertColumn(nIdx++, "Course Name");



	//Automatically adjust column width
	int nColCount = m_listCourse.GetHeaderCtrl()->GetItemCount();
	for (size_t i = 0; i < nColCount; i++)
	{
		m_listCourse.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	//set styles
	m_listCourse.SetExtendedStyle(
		m_listCourse.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);
	return 0;
}

void CCourseDlg::OnClickedBtnAdd()
{
	CString CID;
	m_editCourseId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}

	CString cName;
	m_editCourseName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter course name");
	}

	// fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = COURSE_ADD;
	queryRequest.query.Format(_T("INSERT INTO course VALUES('%s', '%s')"), CID, cName);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CCourseDlg::OnClickedBtnDel()
{
	CString CID;
	m_editCourseId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}

	CString cName;
	m_editCourseName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter course name");
	}


	//fill in stQueryRequest structure
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = COURSE_DEL;
	queryRequest.query.Format(_T("DELETE FROM course WHERE id = '%s' AND name = '%s'"), CID, cName);
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}


void CCourseDlg::OnClickedBtnEdit()
{
	CString CID;
	m_editCourseId.GetWindowText(CID);
	if (CID.IsEmpty())
	{
		AfxMessageBox("Please enter course ID");
	}

	CString cName;
	m_editCourseName.GetWindowText(cName);
	if (cName.IsEmpty())
	{
		AfxMessageBox("Please enter course name");
	}


	

	stQueryRequest queryRequest_checkId;
	queryRequest_checkId.hdr.nCmd = COURSE_EDIT_CHECK;
	queryRequest_checkId.query.Format(_T("SELECT COUNT(*) FROM course WHERE id = '%s'"), CID);
	queryRequest_checkId.hdr.nLen = queryRequest_checkId.query.GetLength();

	//fill in stQueryRequest structure
	//modified sql statement
	stQueryRequest queryRequest;
	queryRequest.hdr.nCmd = COURSE_EDIT;
	queryRequest.query.Format(_T("UPDATE course SET name = '%s' WHERE id = '%s'"), cName, CID);
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


void CCourseDlg::OnClickedBtnQuery()
{
	stQueryRequest queryRequest;
	CString CID;
	m_editCourseId.GetWindowText(CID);
	CString cName;
	m_editCourseName.GetWindowTextA(cName);


	queryRequest.query.Format(_T("SELECT * FROM course where 1= 1 "));

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

	queryRequest.hdr.nCmd = COURSE_QUERY;
	queryRequest.hdr.nLen = queryRequest.query.GetLength();

	//Add the query request to the query request queue
	{
		std::lock_guard<std::mutex> lock(m_QueryMutex);
		m_QueryQueue.push(queryRequest);
	}

	//Notify the background thread of new requests
	m_QueryCondition.notify_one();
}
void CCourseDlg::BackgroundThread(CCourseDlg* pThis)
{
	stPacketHdr sentHdr;

	while (true)
	{
		//check the query queue
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

int CCourseDlg::ReadFileAndDisplay(CListCtrl& lstControl)
{
	std::ifstream file("received_file_course.txt");
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


afx_msg LRESULT CCourseDlg::OnCourseAddOk(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Successfully added course info");
	m_editCourseId.SetWindowText(_T(""));
	m_editCourseName.SetWindowText(_T(""));

	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseAddErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to add course information");
	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseDelOk(WPARAM wParam, LPARAM lParam)
{
	int nSelected = m_listCourse.GetNextItem(-1, LVNI_SELECTED);
	m_listCourse.DeleteItem(nSelected);
	m_editCourseId.SetWindowText(_T(""));
	m_editCourseName.SetWindowText(_T(""));


	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseDelErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Failed to delete course information");
	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseQueryResult(WPARAM wParam, LPARAM lParam)
{
	// delete existing data
	m_listCourse.DeleteAllItems();

	//Read the file and display it in the list control
	ReadFileAndDisplay(m_listCourse);

	m_editCourseId.SetWindowText("");
	m_editCourseName.SetWindowText("");

	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseQueryNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("No result");
	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseQueryErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Query error");
	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseEditNull(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Unable to modify a non-existent course number, please add it first");
	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseEditOk(WPARAM wParam, LPARAM lParam)
{

	return 0;
}


afx_msg LRESULT CCourseDlg::OnCourseEditErr(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("Error modifying data");
	return 0;
}


void CCourseDlg::OnClickListCourse(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int nSelected = m_listCourse.GetSelectionMark();
	if (nSelected != -1)
	{
		CString cId = m_listCourse.GetItemText(nSelected, 0);
		CString cName = m_listCourse.GetItemText(nSelected, 1);
		m_editCourseId.SetWindowText(cId);
		m_editCourseName.SetWindowText(cName);

	}
	*pResult = 0;
}


void CCourseDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}
