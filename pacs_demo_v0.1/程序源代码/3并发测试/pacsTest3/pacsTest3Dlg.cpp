// pacsTest3Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pacsTest3.h"			//应用程序头文件
#include "pacsTest3Dlg.h"	//主对话框头文件
#include "ConfDlg.h"				//网络配置对话框头文件

#include "Protocol.h"				//协议头文件
#include "afxmt.h"					//CMutex类头文件
#include "Shlwapi.h"				//创建目录相关头文件

//全局常量定义
const unsigned int clientEachTime = 5;	//每次增加的客户端个数
const unsigned int maxClientNum = 100;	//最大客户端数目
//全局变量定义
unsigned int threadID = 0;	//线程编号：1,2,3，……
unsigned int totalClientNum = 0;	//总共的客户端个数：1,2,3，……
unsigned int sucClientNum = 0;	//已结束的线程数目：1,2,3，……
//全局互斥变量定义
CMutex dataMutex(FALSE, NULL);	//MFC互斥变量：读写共享数据

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


// CpacsTest3Dlg 对话框




CpacsTest3Dlg::CpacsTest3Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpacsTest3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strIP = _T("");
	m_strDCMPath = _T(".\\Client");
	m_graphThread = NULL;
	m_transDataSet.clear();
	m_graphDataSet.clear();
}

void CpacsTest3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CpacsTest3Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CONFIGURE, &CpacsTest3Dlg::OnBnClickedBtnConfigure)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CpacsTest3Dlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_ADDCLIENT, &CpacsTest3Dlg::OnBnClickedBtnAddclient)
END_MESSAGE_MAP()


// CpacsTest3Dlg 消息处理程序

BOOL CpacsTest3Dlg::OnInitDialog()
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

	//新建子线程用于绘图
	m_graphThread = AfxBeginThread(graphThreadFunc, NULL);
	if (NULL != m_graphThread)
	{
		m_graphThread->SuspendThread();		//暂时挂起
		isSuspent = TRUE;
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("绘图线程创建失败: %d"), GetLastError());
		AfxMessageBox(strMsg, MB_OK);
	}

	//判断DCM文件目录是否存在
	if (!PathIsDirectory(m_strDCMPath))
		CreateDirectory(m_strDCMPath, NULL);	//不存在则创建

	//配置网络前无法测试
	GetDlgItem(IDC_BTN_ADDCLIENT)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpacsTest3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CpacsTest3Dlg::OnPaint()
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
HCURSOR CpacsTest3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//禁止通过esc按键关闭对话框
BOOL CpacsTest3Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == VK_ESCAPE)   
	{   
		pMsg->wParam = 0;
	}    
	return CDialog::PreTranslateMessage(pMsg);
}

//禁止通过回车键关闭对话框
void CpacsTest3Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}

//配置网络参数
void CpacsTest3Dlg::OnBnClickedBtnConfigure()
{
	// TODO: 在此添加控件通知处理程序代码
	CConfDlg confDlg;
	if (IDOK == confDlg.DoModal())
	{
		m_strIP = confDlg.m_strIPConf;
		GetDlgItem(IDC_BTN_ADDCLIENT)->EnableWindow(TRUE);
	}
}

//退出按钮
void CpacsTest3Dlg::OnBnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}

void CpacsTest3Dlg::OnBnClickedBtnAddclient()
{
	// TODO: 在此添加控件通知处理程序代码
	//如果已经达到最大客户端数目则不予增加
	if (totalClientNum >= maxClientNum)
	{
		GetDlgItem(IDC_BTN_ADDCLIENT)->EnableWindow(FALSE);
		AfxMessageBox(_T("已达到最多客户端数目！"), MB_OK);
		return;
	}
	GetDlgItem(IDC_BTN_EXIT)->EnableWindow(FALSE);
	totalClientNum += clientEachTime;
	sucClientNum = 0;

	//唤醒绘图进程
	if(isSuspent)
	{
		m_graphThread->ResumeThread();
		isSuspent = FALSE;
	}

	//以多个线程模拟多个客户端
	for (int i = 1; i <= clientEachTime; i++)
	{
		//创建客户端线程
		CWinThread* m_pThread = AfxBeginThread(socketThreadFunc, (LPVOID)this);
		if(NULL == m_pThread)	//创建失败
		{
			CString strMsg;
			strMsg.Format(_T("Socket线程#%d创建失败: %d"), i, GetLastError());
			AfxMessageBox(strMsg, MB_OK);
		}
		else
		{
			//CloseHandle(m_pThread);
		}
	}
}

//绘图进程处理函数
UINT graphThreadFunc(LPVOID pParam)
{
	CpacsTest3Dlg* pMainWnd = (CpacsTest3Dlg*)(AfxGetApp()->GetMainWnd());	//主窗口指针
	DrawGraph dg;
	dg.drawFrame();	//绘制初始框架
	while (1)
	{
		//队列为空则一直等待
		while (pMainWnd->m_graphDataSet.size() == 0);
		//获取数据互斥锁
		dataMutex.Lock();
		//绘图并更新数据队列
		dg.drawGraph(pMainWnd->m_graphDataSet.front());
		pMainWnd->m_graphDataSet.pop_front();
		//释放数据互斥锁
		dataMutex.Unlock();
	}
	return 0;
}

//为每个新的socket连接创建的线程处理函数
UINT socketThreadFunc(LPVOID pParam)
{
	CpacsTest3Dlg* pThreadDlg = (CpacsTest3Dlg*)pParam;
	ASSERT(pThreadDlg);

	//step 1：创建客户端套接字
	AfxSocketInit(NULL);	//必须初始化socket
	CSocket clientSocket;
	if(!clientSocket.Create(0, SOCK_STREAM))	//非0则成功
	{
		CString str;
		str.Format(_T("客户端Socket创建失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		return -1;
	}
	//提出连接请求
	if(!clientSocket.Connect(pThreadDlg->m_strIP, PRE_AGREED_PORT)) //非0则成功
	{
		CString str;
		str.Format(_T("客户端Socket连接失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		return -1;
	}
	//发送请求
	time_t start = time(NULL);	//开始时间
	LPCTSTR c_move_req = _T("C-MOVE");
	if(!clientSocket.Send(c_move_req, _tcslen(c_move_req) * 2, 0))	//注意字节长度
	{
		CString str;
		str.Format(_T("客户端Socket发送失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		return -1;
	}

	//step 2：确定写入文件名和更新绘图数据
	CString fileName;			//文件名
	GraphData newData;	//绘图数据

	//获取数据互斥锁
	dataMutex.Lock();
	//线程数自增
	threadID ++;
	//写入文件名
	fileName.Format(_T("%s\\retrieval%d.dcm"), pThreadDlg->m_strDCMPath, threadID);
	//更新绘图数据
	newData.clientID = threadID;
	//释放数据互斥锁
	dataMutex.Unlock();

	//step 3：接收服务端发回消息
	TCHAR databuf[1024];
	DWORD dwTotalWrite = 0;	//总的写入字符个数
	DWORD dwWrite = 0;
	BOOL bRet = FALSE;
	int read = 0;
	HANDLE hFile = CreateFile(fileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,0);
	while (1)
	{
		memset(databuf,0,1024); 
		read = clientSocket.Receive(databuf, 1024, 0 );
		bRet = WriteFile(hFile,databuf,read,&dwWrite,NULL);
		dwTotalWrite += dwWrite;	//总的写入字符个数
		if(read <= 0)
			break;
		if(bRet == FALSE)
		{
			CString strMsg;
			strMsg.Format(_T("写入文件 %s 失败: %d"), fileName, GetLastError());
			AfxMessageBox(strMsg, MB_OK);
			break;
		} 
	}
	//关闭文件句柄
	CloseHandle(hFile);
	//网络通信结束，关闭socket
	clientSocket.Close();
	time_t end = time(NULL);	//结束时间

	//step4：计算绘图数据
	//获取数据互斥锁
	dataMutex.Lock();
	sucClientNum ++;
	if (sucClientNum == clientEachTime)
		pThreadDlg->GetDlgItem(IDC_BTN_EXIT)->EnableWindow(TRUE);
	//更新绘图数据
	newData.efficiency = difftime(end, start);	//时间差
	newData.transRate = (double)dwTotalWrite;
	pThreadDlg->m_transDataSet.push_back(newData);
	//计算绘图用的传输数据
	double sumSpentTime = 0.0;	//总的传输时间
	double sumTransData = 0.0;		//总的传输数据量
	unsigned int sumThread = pThreadDlg->m_transDataSet.size();	//总的线程（client）个数
	for(list<GraphData>::iterator iter = pThreadDlg->m_transDataSet.begin();
		iter != pThreadDlg->m_transDataSet.end(); iter++)
	{
		sumSpentTime += (*iter).efficiency;
		sumTransData += (*iter).transRate;
	}
	newData.clientID = sumThread;
	newData.efficiency = sumSpentTime / sumThread;
	newData.transRate = sumTransData / sumSpentTime;
	//插入绘图数据队列
	pThreadDlg->m_graphDataSet.push_back(newData);
	//释放数据互斥锁
	dataMutex.Unlock();

	return 0;
}