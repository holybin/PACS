// pacsTest1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pacsTest1.h"
#include "pacsTest1Dlg.h"	//主对话框头文件
#include "ConfDlg.h"	//网络配置对话框头文件
#include "Protocol.h"	//协议头文件
#include "afxmt.h"	//CMutex类包含文件
#include "Shlwapi.h"	//创建目录相关头文件

//DCMTK头文件
#include "dcmtk\\dcmdata\\dctk.h"
#include "dcmtk\\dcmimgle\\dcmimage.h"

//DCMTK静态链接库
#pragma comment(lib, "ofstd.lib") 
#pragma comment(lib, "dcmdata.lib") 
#pragma comment(lib, "netapi32.lib") 
#pragma comment(lib, "wsock32.lib") 
#pragma comment(lib, "dcmimgle.lib") 
#pragma comment(lib, "dcmimage.lib")

//全局变量定义
unsigned int totalImgNum = 0;	//传输的图片总数目
unsigned int succImgNum = 0;	//传输成功的图片数目
unsigned int succThreadNum = 0;	//已结束的线程数目
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


// CpacsTest1Dlg 对话框




CpacsTest1Dlg::CpacsTest1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpacsTest1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strDCMPath = _T(".\\Client");
	m_strIP = _T("");
}

void CpacsTest1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_LIST1, m_infoListCtrl1);
	DDX_Control(pDX, IDC_INFO_LIST2, m_infoListCtrl2);
	DDX_Control(pDX, IDC_INFO_LIST3, m_infoListCtrl3);
}

BEGIN_MESSAGE_MAP(CpacsTest1Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CONFIGURE, &CpacsTest1Dlg::OnBnClickedBtnConfigure)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CpacsTest1Dlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_TEST, &CpacsTest1Dlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CpacsTest1Dlg 消息处理程序

BOOL CpacsTest1Dlg::OnInitDialog()
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

	//初始化信息显示列表控件
	initListCtrl(&m_infoListCtrl1);
	initListCtrl(&m_infoListCtrl2);
	initListCtrl(&m_infoListCtrl3);
	//重置图像显示控件和信息列表控件
	rstImgAndListCtrl();

	//判断DCM文件目录是否存在
	if (!PathIsDirectory(m_strDCMPath))
		CreateDirectory(m_strDCMPath, NULL);	//不存在则创建

	//配置网络前无法测试
	GetDlgItem(IDC_BTN_TEST)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpacsTest1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CpacsTest1Dlg::OnPaint()
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
HCURSOR CpacsTest1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//初始化图像信息列表控件
void CpacsTest1Dlg::initListCtrl(CListCtrl* listCtrl)
{
	//报表模式
	listCtrl->ModifyStyle( 0, LVS_REPORT );
	//间隔线+行选中
	listCtrl->SetExtendedStyle(listCtrl->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	listCtrl->InsertColumn(1, _T("[组号, 元素号]"), LVCFMT_CENTER);
	listCtrl->InsertColumn(2, _T("属性描述"), LVCFMT_CENTER);
	listCtrl->InsertColumn(3, _T("值"), LVCFMT_CENTER);
	//获得当前客户区信息
	CRect rect;
	listCtrl->GetClientRect(rect);
	//设置列的宽度
	listCtrl->SetColumnWidth(0, rect.Width() / 3 + 5); 
	listCtrl->SetColumnWidth(1, rect.Width() / 3);
	listCtrl->SetColumnWidth(2, rect.Width() / 3);
}

//重置图像显示控件和信息列表控件
void CpacsTest1Dlg::rstImgAndListCtrl()
{
	//清除绘图控件内容
	GetDlgItem(IDC_SHOW_IMG1)->Invalidate();
	GetDlgItem(IDC_SHOW_IMG2)->Invalidate();
	GetDlgItem(IDC_SHOW_IMG3)->Invalidate();
	//清除列表控件内容
	m_infoListCtrl1.DeleteAllItems();
	m_infoListCtrl2.DeleteAllItems();
	m_infoListCtrl3.DeleteAllItems();
}

//配置网络参数
void CpacsTest1Dlg::OnBnClickedBtnConfigure()
{
	// TODO: 在此添加控件通知处理程序代码
	CConfDlg confDlg;
	if (IDOK == confDlg.DoModal())
	{
		m_strIP = confDlg.m_strIPConf;
		GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
	}
}

//禁止通过esc按键关闭对话框
BOOL CpacsTest1Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == VK_ESCAPE)   
	{   
		pMsg->wParam = 0;
	}    
	return CDialog::PreTranslateMessage(pMsg);
}
//禁止通过回车键关闭对话框
void CpacsTest1Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialog::OnOK();
}
//退出按钮
void CpacsTest1Dlg::OnBnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}

//传输测试
void CpacsTest1Dlg::OnBnClickedBtnTest()
{
	// TODO: 在此添加控件通知处理程序代码
	//创建客户端线程
	CWinThread* m_pThread1 = AfxBeginThread(socketThreadFunc1, (LPVOID)this);
	CWinThread* m_pThread2 = AfxBeginThread(socketThreadFunc2, (LPVOID)this);
	CWinThread* m_pThread3 = AfxBeginThread(socketThreadFunc3, (LPVOID)this);
	//失败提示
	unsigned int countFail = 0;
	if(NULL == m_pThread1)
	{
		AfxMessageBox(_T("客户端套接字子线程#1创建失败!"), MB_OK);
		countFail ++;
	}
	if(NULL == m_pThread2)
	{
		AfxMessageBox(_T("客户端套接字子线程#2创建失败!"), MB_OK);
		countFail ++;
	}
	if(NULL == m_pThread3)
	{
		AfxMessageBox(_T("客户端套接字子线程#3创建失败!"), MB_OK);
		countFail ++;
	}
	//所有线程创建均失败
	if (countFail == DCM_NUM)
		return;
	////等待线程结束：10s超时
	//WaitForSingleObject(m_pThread1->m_hThread, 10000);
	//WaitForSingleObject(m_pThread2->m_hThread, 10000);
	//WaitForSingleObject(m_pThread3->m_hThread, 10000);
	////释放句柄资源
	//CloseHandle(m_pThread1->m_hThread);
	//CloseHandle(m_pThread2->m_hThread);
	//CloseHandle(m_pThread3->m_hThread);
	////释放内存
	//delete m_pThread1;
	//m_pThread1 = NULL;
	//delete m_pThread2;
	//m_pThread2 = NULL;
	//delete m_pThread3;
	//m_pThread3 = NULL;

	//重置图像和列表
	rstImgAndListCtrl();
	//已完成线程数归0
	succThreadNum = 0;
	//禁止二次点击
	GetDlgItem(IDC_BTN_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(FALSE);
}

//客户端socket线程函数1
UINT socketThreadFunc1(LPVOID pParam)
{
	//获取主对话框指针
	CpacsTest1Dlg* pThreadDlg = (CpacsTest1Dlg*)pParam;
	ASSERT(pThreadDlg);
	//初始化socket：必须的步骤
	AfxSocketInit(NULL);

	//标记变量（判断接收的文件是否同时满足：1.传输成功；2.存储兼容DICOM标准）
	unsigned int uiSucc = 0;

	//step 1：创建客户端套接字
	CSocket clientSocket;
	//本机测试：同一个端口
	if(!clientSocket.Create(0, SOCK_STREAM))	//非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字创建失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//提出连接请求
	if(!clientSocket.Connect(pThreadDlg->m_strIP, PRE_AGREED_PORT)) //非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字连接失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//发送请求
	LPCTSTR c_move_req = _T("C-MOVE");
	if(!clientSocket.Send(c_move_req, _tcslen(c_move_req) * 2, 0))	//字节长度
	{
		CString str;
		str.Format(_T("客户端套接字发送失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}

	//step 2：确定写入文件名
	CString fileName = _T("");
	//获取数据互斥锁
	dataMutex.Lock();
	//图片数自增
	totalImgNum++;
	//写入文件名
	fileName.Format(_T("%s\\retrieval%d.dcm"), pThreadDlg->m_strDCMPath, totalImgNum);
	//释放数据互斥锁
	dataMutex.Unlock();

	//step 3：接收服务端发回消息
	TCHAR databuf[1024];
	//DWORD dwTotalWrite = 0;	//总的写入字符个数
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
		//dwTotalWrite += dwWrite;	//总的写入字符个数
		if(read <= 0)
		{
			//标记变量：传输成功加1
			uiSucc ++;	
			break;
		}
		if(bRet == FALSE)
		{
			CString strMsg;
			strMsg.Format(_T("写入文件失败: %d"), GetLastError());
			AfxMessageBox(strMsg, MB_OK);
			break;
		} 
	}
	//关闭文件句柄
	CloseHandle(hFile);
	//网络通信结束，关闭socket
	clientSocket.Close();

	//文件名从CString转化为（const）char*
	const size_t strSize = (fileName.GetLength() + 1)*2;	//宽字符的长度
	char *pStr = new char[strSize];	//分配空间
	size_t size = 0;
	wcstombs_s(&size, pStr, strSize, fileName, _TRUNCATE);

	//step 4：读取DCM文件信息
	OFCondition status;
	DcmFileFormat fileformat;
	status = fileformat.loadFile((const char*)pStr, EXS_Unknown, EGL_noChange, DCM_MaxReadLength);
	int countRow = 0;	//列表控件行计数
	if (status.good())
	{
		//文件头的metainfo
		DcmMetaInfo *metainfo = fileformat.getMetaInfo();
		DcmObject* pDO = metainfo->nextInContainer(NULL);
		if (pDO != NULL)
		{
			//标记变量：存储兼容DICOM标准加1
			uiSucc ++;
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				metainfo->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl1.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = metainfo->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
		else	//metainfo为空
		{
			pThreadDlg->m_infoListCtrl1.InsertItem(countRow, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 1, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 2, _T("NULL"));	//NULL
			countRow ++;
		}

		//文件头的dataset
		DcmDataset *dataset = fileformat.getDataset();
		pDO = dataset->nextInContainer(NULL);
		if (pDO != NULL)
		{	
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				dataset->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl1.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = dataset->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
	}
	else	//无法打开文件
	{
		pThreadDlg->m_infoListCtrl1.InsertItem(countRow, _T("无法"));
		pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 1, _T("打开"));
		pThreadDlg->m_infoListCtrl1.SetItemText(countRow, 2, _T("文件！"));
		countRow ++;
	}

	//step 5：显示DCM文件
	//是否同时满足：1.传输成功；2.存储兼容DICOM标准
	if (uiSucc == 2)	//满足则绘图
	{
		//获取图像数据
		DicomImage* pDicomImg = new DicomImage((const char*)pStr);
		LPBITMAPINFOHEADER m_lpBMIH;
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
		m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
		m_lpBMIH->biWidth = pDicomImg->getWidth();
		m_lpBMIH->biHeight = pDicomImg->getHeight();
		m_lpBMIH->biPlanes = 1;
		m_lpBMIH->biBitCount = 24;
		m_lpBMIH->biCompression = BI_RGB;
		m_lpBMIH->biSizeImage = 0;
		m_lpBMIH->biXPelsPerMeter = 0;
		m_lpBMIH->biYPelsPerMeter = 0;
		void* pDicomDibits;
		pDicomImg->setMinMaxWindow();
		pDicomImg->createWindowsDIB(pDicomDibits, 0, 0, 24, 1, 1);
		//获得图像控件的句柄
		CWnd *pWnd = pThreadDlg->GetDlgItem(IDC_SHOW_IMG1);
		//获得图像控件的DC
		CDC *pDC = pWnd->GetDC();
		//获取图片控件的宽和高
		CRect rect;
		pWnd->GetClientRect(&rect);
		pDC->SetStretchBltMode(HALFTONE);	//防止图片失真
		//画图
		StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), 0, 0,
			m_lpBMIH->biWidth,m_lpBMIH->biHeight, pDicomDibits, (LPBITMAPINFO) m_lpBMIH,
			DIB_RGB_COLORS, SRCCOPY);
	}
	delete [] pStr;

	//step 6：判断是否可以进行下一次传输测试
	//获取数据互斥锁
	dataMutex.Lock();
	//测试成功的图片数目自增
	if (uiSucc == 2)
		succImgNum ++;	
	//已完成线程数目自增
	succThreadNum ++;
	//任务均完成
	if (succThreadNum == DCM_NUM)
	{
		//允许按钮点击
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		//设置显示
		pThreadDlg->SetDlgItemInt(IDC_EDIT_SUCCESS, succImgNum);
		pThreadDlg->SetDlgItemInt(IDC_EDIT_FAIL, totalImgNum - succImgNum);
		CString strRatio;
		strRatio.Format(_T("%.2f %%"), (double)succImgNum / totalImgNum * 100);
		pThreadDlg->SetDlgItemText(IDC_EDIT_RATIO, strRatio);
	}
	//释放数据互斥锁
	dataMutex.Unlock();
	return 0;
}

//客户端socket线程函数2
UINT socketThreadFunc2(LPVOID pParam)
{
	//获取主对话框指针
	CpacsTest1Dlg* pThreadDlg = (CpacsTest1Dlg*)pParam;
	ASSERT(pThreadDlg);
	//初始化socket：必须的步骤
	AfxSocketInit(NULL);

	//标记变量（判断接收的文件是否同时满足：1.传输成功；2.存储兼容DICOM标准）
	unsigned int uiSucc = 0;

	//step 1：创建客户端套接字
	CSocket clientSocket;
	//本机测试：同一个端口
	if(!clientSocket.Create(0, SOCK_STREAM))	//非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字创建失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//提出连接请求
	if(!clientSocket.Connect(pThreadDlg->m_strIP, PRE_AGREED_PORT)) //非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字连接失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//发送请求
	LPCTSTR c_move_req = _T("C-MOVE");
	if(!clientSocket.Send(c_move_req, _tcslen(c_move_req) * 2, 0))	//字节长度
	{
		CString str;
		str.Format(_T("客户端套接字发送失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}

	//step 2：确定写入文件名
	CString fileName = _T("");
	//获取数据互斥锁
	dataMutex.Lock();
	//图片数自增
	totalImgNum++;
	//写入文件名
	fileName.Format(_T("%s\\retrieval%d.dcm"), pThreadDlg->m_strDCMPath, totalImgNum);
	//释放数据互斥锁
	dataMutex.Unlock();

	//step 3：接收服务端发回消息
	TCHAR databuf[1024];
	//DWORD dwTotalWrite = 0;	//总的写入字符个数
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
		//dwTotalWrite += dwWrite;	//总的写入字符个数
		if(read <= 0)
		{
			//标记变量：传输成功加1
			uiSucc ++;	
			break;
		}
		if(bRet == FALSE)
		{
			CString strMsg;
			strMsg.Format(_T("写入文件失败: %d"), GetLastError());
			AfxMessageBox(strMsg, MB_OK);
			break;
		} 
	}
	//关闭文件句柄
	CloseHandle(hFile);
	//网络通信结束，关闭socket
	clientSocket.Close();

	//文件名从CString转化为（const）char*
	const size_t strSize = (fileName.GetLength() + 1)*2;	//宽字符的长度
	char *pStr = new char[strSize];	//分配空间
	size_t size = 0;
	wcstombs_s(&size, pStr, strSize, fileName, _TRUNCATE);

	//step 4：读取DCM文件信息
	OFCondition status;
	DcmFileFormat fileformat;
	status = fileformat.loadFile((const char*)pStr, EXS_Unknown, EGL_noChange, DCM_MaxReadLength);
	int countRow = 0;	//列表控件行计数
	if (status.good())
	{
		//文件头的metainfo
		DcmMetaInfo *metainfo = fileformat.getMetaInfo();
		DcmObject* pDO = metainfo->nextInContainer(NULL);
		if (pDO != NULL)
		{
			//标记变量：存储兼容DICOM标准加1
			uiSucc ++;
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				metainfo->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl2.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = metainfo->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
		else	//metainfo为空
		{
			pThreadDlg->m_infoListCtrl2.InsertItem(countRow, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 1, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 2, _T("NULL"));	//NULL
			countRow ++;
		}

		//文件头的dataset
		DcmDataset *dataset = fileformat.getDataset();
		pDO = dataset->nextInContainer(NULL);
		if (pDO != NULL)
		{	
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				dataset->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl2.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = dataset->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
	}
	else	//无法打开文件
	{
		pThreadDlg->m_infoListCtrl2.InsertItem(countRow, _T("无法"));
		pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 1, _T("打开"));
		pThreadDlg->m_infoListCtrl2.SetItemText(countRow, 2, _T("文件！"));
		countRow ++;
	}

	//step 5：显示DCM文件
	//是否同时满足：1.传输成功；2.存储兼容DICOM标准
	if (uiSucc == 2)	//满足则绘图
	{
		//获取图像数据
		DicomImage* pDicomImg = new DicomImage((const char*)pStr);
		LPBITMAPINFOHEADER m_lpBMIH;
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
		m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
		m_lpBMIH->biWidth = pDicomImg->getWidth();
		m_lpBMIH->biHeight = pDicomImg->getHeight();
		m_lpBMIH->biPlanes = 1;
		m_lpBMIH->biBitCount = 24;
		m_lpBMIH->biCompression = BI_RGB;
		m_lpBMIH->biSizeImage = 0;
		m_lpBMIH->biXPelsPerMeter = 0;
		m_lpBMIH->biYPelsPerMeter = 0;
		void* pDicomDibits;
		pDicomImg->setMinMaxWindow();
		pDicomImg->createWindowsDIB(pDicomDibits, 0, 0, 24, 1, 1);
		//获得图像控件的句柄
		CWnd *pWnd = pThreadDlg->GetDlgItem(IDC_SHOW_IMG2);
		//获得图像控件的DC
		CDC *pDC = pWnd->GetDC();
		//获取图片控件的宽和高
		CRect rect;
		pWnd->GetClientRect(&rect);
		pDC->SetStretchBltMode(HALFTONE);	//防止图片失真
		//画图
		StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), 0, 0,
			m_lpBMIH->biWidth,m_lpBMIH->biHeight, pDicomDibits, (LPBITMAPINFO) m_lpBMIH,
			DIB_RGB_COLORS, SRCCOPY);
	}
	delete [] pStr;

	//step 6：判断是否可以进行下一次传输测试
	//获取数据互斥锁
	dataMutex.Lock();
	//测试成功的图片数目自增
	if (uiSucc == 2)
		succImgNum ++;	
	//已完成线程数目自增
	succThreadNum ++;
	//任务均完成
	if (succThreadNum == DCM_NUM)
	{
		//允许按钮点击
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		//设置显示
		pThreadDlg->SetDlgItemInt(IDC_EDIT_SUCCESS, succImgNum);
		pThreadDlg->SetDlgItemInt(IDC_EDIT_FAIL, totalImgNum - succImgNum);
		CString strRatio;
		strRatio.Format(_T("%.2f %%"), (double)succImgNum / totalImgNum * 100);
		pThreadDlg->SetDlgItemText(IDC_EDIT_RATIO, strRatio);
	}
	//释放数据互斥锁
	dataMutex.Unlock();
	return 0;
}

//客户端socket线程函数3
UINT socketThreadFunc3(LPVOID pParam)
{
	//获取主对话框指针
	CpacsTest1Dlg* pThreadDlg = (CpacsTest1Dlg*)pParam;
	ASSERT(pThreadDlg);
	//初始化socket：必须的步骤
	AfxSocketInit(NULL);

	//标记变量（判断接收的文件是否同时满足：1.传输成功；2.存储兼容DICOM标准）
	unsigned int uiSucc = 0;

	//step 1：创建客户端套接字
	CSocket clientSocket;
	//本机测试：同一个端口
	if(!clientSocket.Create(0, SOCK_STREAM))	//非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字创建失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//提出连接请求
	if(!clientSocket.Connect(pThreadDlg->m_strIP, PRE_AGREED_PORT)) //非0则成功
	{
		CString str;
		str.Format(_T("客户端套接字连接失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}
	//发送请求
	LPCTSTR c_move_req = _T("C-MOVE");
	if(!clientSocket.Send(c_move_req, _tcslen(c_move_req) * 2, 0))	//字节长度
	{
		CString str;
		str.Format(_T("客户端套接字发送失败: %d"), clientSocket.GetLastError());
		AfxMessageBox(str, MB_OK);
		clientSocket.Close();
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		return -1;
	}

	//step 2：确定写入文件名
	CString fileName = _T("");
	//获取数据互斥锁
	dataMutex.Lock();
	//图片数自增
	totalImgNum++;
	//写入文件名
	fileName.Format(_T("%s\\retrieval%d.dcm"), pThreadDlg->m_strDCMPath, totalImgNum);
	//释放数据互斥锁
	dataMutex.Unlock();

	//step 3：接收服务端发回消息
	TCHAR databuf[1024];
	//DWORD dwTotalWrite = 0;	//总的写入字符个数
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
		//dwTotalWrite += dwWrite;	//总的写入字符个数
		if(read <= 0)
		{
			//标记变量：传输成功加1
			uiSucc ++;	
			break;
		}
		if(bRet == FALSE)
		{
			CString strMsg;
			strMsg.Format(_T("写入文件失败: %d"), GetLastError());
			AfxMessageBox(strMsg, MB_OK);
			break;
		} 
	}
	//关闭文件句柄
	CloseHandle(hFile);
	//网络通信结束，关闭socket
	clientSocket.Close();

	//文件名从CString转化为（const）char*
	const size_t strSize = (fileName.GetLength() + 1)*2;	//宽字符的长度
	char *pStr = new char[strSize];	//分配空间
	size_t size = 0;
	wcstombs_s(&size, pStr, strSize, fileName, _TRUNCATE);

	//step 4：读取DCM文件信息
	OFCondition status;
	DcmFileFormat fileformat;
	status = fileformat.loadFile((const char*)pStr, EXS_Unknown, EGL_noChange, DCM_MaxReadLength);
	int countRow = 0;	//列表控件行计数
	if (status.good())
	{
		//文件头的metainfo
		DcmMetaInfo *metainfo = fileformat.getMetaInfo();
		DcmObject* pDO = metainfo->nextInContainer(NULL);
		if (pDO != NULL)
		{
			//标记变量：存储兼容DICOM标准加1
			uiSucc ++;
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				metainfo->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl3.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = metainfo->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
		else	//metainfo为空
		{
			pThreadDlg->m_infoListCtrl3.InsertItem(countRow, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 1, _T("NULL"));	//NULL
			pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 2, _T("NULL"));	//NULL
			countRow ++;
		}

		//文件头的dataset
		DcmDataset *dataset = fileformat.getDataset();
		pDO = dataset->nextInContainer(NULL);
		if (pDO != NULL)
		{	
			do 
			{
				DcmTag dt = pDO->getTag();	//获取tag
				OFString value;
				dataset->findAndGetOFString(dt.getXTag(), value);	//根据tagkey获取tag的value
				//显示到list control
				CString strTag;
				strTag.Format(_T("[%.4x, %.4x]"), dt.getGTag(), dt.getETag());
				pThreadDlg->m_infoListCtrl3.InsertItem(countRow, strTag);	//（组号，元素号）
				CString strTagName(dt.getTagName());
				pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 1, strTagName);	//tag的名称
				CString strTagValue(value.c_str());
				pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 2, strTagValue);	//tag的值域
				//下一个
				pDO = dataset->nextInContainer(pDO);
				countRow ++;
			} while (pDO != NULL);
		}
	}
	else	//无法打开文件
	{
		pThreadDlg->m_infoListCtrl3.InsertItem(countRow, _T("无法"));
		pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 1, _T("打开"));
		pThreadDlg->m_infoListCtrl3.SetItemText(countRow, 2, _T("文件！"));
		countRow ++;
	}

	//step 5：显示DCM文件
	//是否同时满足：1.传输成功；2.存储兼容DICOM标准
	if (uiSucc == 2)	//满足则绘图
	{
		//获取图像数据
		DicomImage* pDicomImg = new DicomImage((const char*)pStr);
		LPBITMAPINFOHEADER m_lpBMIH;
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
		m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
		m_lpBMIH->biWidth = pDicomImg->getWidth();
		m_lpBMIH->biHeight = pDicomImg->getHeight();
		m_lpBMIH->biPlanes = 1;
		m_lpBMIH->biBitCount = 24;
		m_lpBMIH->biCompression = BI_RGB;
		m_lpBMIH->biSizeImage = 0;
		m_lpBMIH->biXPelsPerMeter = 0;
		m_lpBMIH->biYPelsPerMeter = 0;
		void* pDicomDibits;
		pDicomImg->setMinMaxWindow();
		pDicomImg->createWindowsDIB(pDicomDibits, 0, 0, 24, 1, 1);
		//获得图像控件的句柄
		CWnd *pWnd = pThreadDlg->GetDlgItem(IDC_SHOW_IMG3);
		//获得图像控件的DC
		CDC *pDC = pWnd->GetDC();
		//获取图片控件的宽和高
		CRect rect;
		pWnd->GetClientRect(&rect);
		pDC->SetStretchBltMode(HALFTONE);	//防止图片失真
		//画图
		StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), 0, 0,
			m_lpBMIH->biWidth,m_lpBMIH->biHeight, pDicomDibits, (LPBITMAPINFO) m_lpBMIH,
			DIB_RGB_COLORS, SRCCOPY);
	}
	delete [] pStr;

	//step 6：判断是否可以进行下一次传输测试
	//获取数据互斥锁
	dataMutex.Lock();
	//测试成功的图片数目自增
	if (uiSucc == 2)
		succImgNum ++;	
	//已完成线程数目自增
	succThreadNum ++;
	//任务均完成
	if (succThreadNum == DCM_NUM)
	{
		//允许按钮点击
		pThreadDlg->GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
		pThreadDlg->GetDlgItem(IDC_BTN_CONFIGURE)->EnableWindow(TRUE);
		//设置显示
		pThreadDlg->SetDlgItemInt(IDC_EDIT_SUCCESS, succImgNum);
		pThreadDlg->SetDlgItemInt(IDC_EDIT_FAIL, totalImgNum - succImgNum);
		CString strRatio;
		strRatio.Format(_T("%.2f %%"), (double)succImgNum / totalImgNum * 100);
		pThreadDlg->SetDlgItemText(IDC_EDIT_RATIO, strRatio);
	}
	//释放数据互斥锁
	dataMutex.Unlock();
	return 0;
}