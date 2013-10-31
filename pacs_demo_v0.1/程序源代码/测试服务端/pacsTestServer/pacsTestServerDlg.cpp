// pacsTestServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pacsTestServer.h"
#include "pacsTestServerDlg.h"	//主对话框头文件
#include "Protocol.h"		//协议头文件
#include "Shlwapi.h"	//创建目录相关头文件
#include <afxmt.h>	//CMutex类包含文件
#include <vector>
#include <iostream>
#include <ctime>	//时间做种子产生随机数
using namespace std;

//全局变量定义
vector<CString> fileNameVec;	//服务器目录下的DCM文件名列表
unsigned int countRow = 0;	//消息列表行计数
CMutex fileMutex(FALSE, NULL);	//MFC互斥变量：读写文件

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CpacsTestServerDlg 对话框




CpacsTestServerDlg::CpacsTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpacsTestServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strIP = _T("");
	m_pThread = NULL;
	strDCMPath = _T(".\\Server");
}

void CpacsTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_msgListCtrl);
}

BEGIN_MESSAGE_MAP(CpacsTestServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CpacsTestServerDlg::OnBnClickedBtnStart)
END_MESSAGE_MAP()


// CpacsTestServerDlg 消息处理程序

BOOL CpacsTestServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//设置端口号
	SetDlgItemInt(IDC_EDIT_PORT, PRE_AGREED_PORT);
	//获取本机(服务器)IP地址
	char serverHostName[100];
	gethostname(serverHostName, 100);
	LPHOSTENT hostent;
	hostent = gethostbyname(serverHostName);
	LPSTR ipAddr = inet_ntoa(*(LPIN_ADDR)*(hostent->h_addr_list));
	m_strIP = ipAddr;
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(m_strIP);

	//初始化消息列表控件
	//报表模式
	m_msgListCtrl.ModifyStyle( 0, LVS_REPORT );
	//间隔线+行选中
	m_msgListCtrl.SetExtendedStyle(m_msgListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_msgListCtrl.InsertColumn(1, _T("编号"), LVCFMT_CENTER);
	m_msgListCtrl.InsertColumn(2, _T("时间"), LVCFMT_CENTER);
	m_msgListCtrl.InsertColumn(3, _T("主体"), LVCFMT_CENTER);
	m_msgListCtrl.InsertColumn(4, _T("信息"), LVCFMT_CENTER);
	//获得当前客户区信息
	CRect rect;
	m_msgListCtrl.GetClientRect(rect);
	//设置列的宽度
	m_msgListCtrl.SetColumnWidth(0, rect.Width() / 8); 
	m_msgListCtrl.SetColumnWidth(1, rect.Width() / 6);
	m_msgListCtrl.SetColumnWidth(2, rect.Width() / 8);
	m_msgListCtrl.SetColumnWidth(3, 7 * rect.Width() / 12);

	//判断DCM文件目录是否存在
	if (!PathIsDirectory(strDCMPath))
		CreateDirectory(strDCMPath, NULL);	//不存在则创建

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpacsTestServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CpacsTestServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CpacsTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//禁止通过回车键关闭对话框
void CpacsTestServerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}
//禁止通过esc按键关闭对话框
BOOL CpacsTestServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == VK_ESCAPE)   
	{   
		pMsg->wParam = VK_RETURN;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CpacsTestServerDlg::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	//确保服务器线程没有在运行
	if ( m_pThread != NULL )
	{
		AfxMessageBox(_T("服务器已经在运行!"), MB_OK);
		return;
	}
	//获取服务端目录下所有文件名
	CFileFind finder;
	CString strDCMDir;
	strDCMDir.Format(_T("%s\\*.dcm"), strDCMPath);
	BOOL bWorking = finder.FindFile(strDCMDir);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if( !finder.IsDirectory() && !finder.IsDots() )//不为目录
		{
			//CString filePath = _T(".\\Server\\");
			//fileNameVec.push_back(filePath + finder.GetFileName());
			fileNameVec.push_back(finder.GetFilePath());
		}
	}
	finder.Close();
	//如果服务端目录下没有DCM文件可用于传输则报错
	if (fileNameVec.size() == 0)
	{
		AfxMessageBox(_T("服务端没有可用的DCM文件!"), MB_OK);
		return;
	}
	//禁止二次点击
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	//创建服务器线程
	m_pThread = AfxBeginThread(serverThreadFunc, (LPVOID)this);
	if (m_pThread == NULL)
	{
		CString strMsg;
		strMsg.Format(_T("服务器线程创建失败: "), GetLastError());
		AfxMessageBox(strMsg, MB_OK);

		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}
}

//服务器线程函数
UINT serverThreadFunc( LPVOID pParam )
{
	CpacsTestServerDlg* pThreadDlg = (CpacsTestServerDlg*)pParam;
	ASSERT(pThreadDlg);

	//套接字准备工作
	AfxSocketInit(NULL);	//必须初始化socket
	CSocket serverSocket;
	//创建socket
	if (!serverSocket.Socket())
	{
		CString str;
		str.Format(_T("服务端套接字创建失败: %d"), serverSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		serverSocket.Close();

		pThreadDlg->GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		return -1;
	}

	//设置socket的选项：解决10048错误必须的步骤
	//BOOL bOptVal = TRUE;
	//int bOptLen = sizeof(BOOL);
	//m_serverSocket.SetSockOpt(SO_REUSEADDR, (void *)&bOptVal, bOptLen, SOL_SOCKET);

	//绑定socket
	if (!serverSocket.Bind(PRE_AGREED_PORT))
	{
		CString str;
		str.Format(_T("服务端套接字绑定失败: %d"), serverSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		serverSocket.Close();

		pThreadDlg->GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		return -1;
	}

	//监听socket
	if(!serverSocket.Listen(10))//队列长度为10
	{    
		CString str;
		str.Format(_T("服务端套接字监听失败: %d"), serverSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		serverSocket.Close();

		pThreadDlg->GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		return -1;
	}

	//服务端成功启动提示
	CString strTime;
	strTime.Format(_T("%.3d"), countRow + 1);
	pThreadDlg->m_msgListCtrl.InsertItem(countRow, strTime);
	CTime CurrentTime = CTime::GetCurrentTime();
	strTime.Format(_T("%.2d:%.2d:%.2d"), CurrentTime.GetHour(),  CurrentTime.GetMinute(), CurrentTime.GetSecond());
	pThreadDlg->m_msgListCtrl.SetItemText(countRow, 1, strTime);
	pThreadDlg->m_msgListCtrl.SetItemText(countRow, 2, _T("服务器"));
	pThreadDlg->m_msgListCtrl.SetItemText(countRow, 3, _T("服务器启动成功!"));
	countRow++;

	//开始工作循环
	while(1)
	{
		//创建工作socket
		CSocket workSocket;
		//接收外部连接
		if(!serverSocket.Accept(workSocket))
		{
			continue;
		}
		else
		{
			//获取客户端IP
			CString strConnClient = _T("");
			UINT port = 0;
			workSocket.GetPeerName(strConnClient, port);
			//客户端连接提示
			CString strTime;
			strTime.Format(_T("%.3d"), countRow + 1);
			pThreadDlg->m_msgListCtrl.InsertItem(countRow, strTime);
			CString strConnMsg = _T("客户端 ") + strConnClient + _T("已连接!");
			CTime CurrentTime = CTime::GetCurrentTime();
			strTime.Format(_T("%.2d:%.2d:%.2d"), CurrentTime.GetHour(),  CurrentTime.GetMinute(), CurrentTime.GetSecond());
			pThreadDlg->m_msgListCtrl.SetItemText(countRow, 1, strTime);
			pThreadDlg->m_msgListCtrl.SetItemText(countRow, 2, _T("客户端"));
			pThreadDlg->m_msgListCtrl.SetItemText(countRow, 3, strConnMsg);
			countRow++;

			//为每个新的socket连接创建新的处理线程
			SOCKET newSocket = workSocket.Detach();		//线程间传递socket
			CWinThread* pSocketThread= AfxBeginThread(socketThreadFunc, (LPVOID)newSocket);
			if(NULL == pSocketThread)
			{
				//失败提示
				CString strMsg;
				strMsg.Format(_T("服务端套接字子线程创建失败: %d"), GetLastError());
				AfxMessageBox(strMsg, MB_OK);
			}
			else
			{
				//成功提示
				CString strTime;
				strTime.Format(_T("%.3d"), countRow + 1);
				pThreadDlg->m_msgListCtrl.InsertItem(countRow, strTime);
				CTime CurrentTime = CTime::GetCurrentTime();
				strTime.Format(_T("%.2d:%.2d:%.2d"), CurrentTime.GetHour(),  CurrentTime.GetMinute(), CurrentTime.GetSecond());
				pThreadDlg->m_msgListCtrl.SetItemText(countRow, 1, strTime);
				pThreadDlg->m_msgListCtrl.SetItemText(countRow, 2, _T("客户端"));
				pThreadDlg->m_msgListCtrl.SetItemText(countRow, 3, _T("客户端套接字子线程创建成功!"));
				countRow++;
				//释放句柄资源
				//CloseHandle(pSocketThread->m_hThread);
			}
			//关闭
			workSocket.Close();
		}
	}
	//return 0;
}

//为每个新的socket连接创建的线程处理函数
UINT socketThreadFunc(LPVOID pParam)
{
	AfxSocketInit(NULL);	//必须初始化
	//工作socket
	CSocket serverSocket;
	SOCKET newSocket = (SOCKET)pParam;
	serverSocket.Attach(newSocket);

	//响应客户端
	//接收客户端发来的消息
	TCHAR asscMsg[1024];
	memset(asscMsg, 0, sizeof(asscMsg));
	serverSocket.Receive(asscMsg, sizeof(asscMsg), 0);
	//检查是否为C-MOVE请求
	LPCTSTR c_move_rq = _T("C-MOVE");
	LPCTSTR actual_rq = asscMsg;
	if(_tcscmp(c_move_rq, actual_rq) == 0)//是
	{
		////传输测试////
		//获取文件互斥锁
		fileMutex.Lock();
		//随机选择DCM文件进行传输
		Sleep(100);
		srand((unsigned)time(NULL));
		int fileIndex = rand() % fileNameVec.size();

		//发送文件给客户端：阻塞式
		char databuf[1024];
		DWORD dwRead = 0;
		BOOL bRet = FALSE;
		HANDLE hFile=CreateFile(fileNameVec.at(fileIndex), GENERIC_READ, 0, 0, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
		while (1)
		{
			memset(databuf, 0, 1024); 
			bRet = ReadFile(hFile, databuf, 1024, &dwRead, NULL);
			if(bRet==FALSE)
			{   
				CString strMsg;
				strMsg.Format(_T("读取文件失败: %d"), GetLastError());
				AfxMessageBox(strMsg, MB_OK);
				break;
			}
			if (dwRead==0)
				break;
			serverSocket.Send(databuf, dwRead, 0 );
		}
		CloseHandle(hFile);
		//释放文件互斥锁
		fileMutex.Unlock();
	}
	//else
	//	AfxMessageBox(_T("请求类型错误!"));

	serverSocket.Close();
	return 0;
}