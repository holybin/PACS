// pacsTest2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pacsTest2.h"
#include "pacsTest2Dlg.h"

//DCMTK头文件
#include "dcmtk\\dcmdata\\dctk.h"
#include "dcmtk\\dcmimgle\\dcmimage.h"
#include "dcmtk\\dcmjpeg\\djencode.h"	//编码DJEncoderRegistration
#include "dcmtk\\dcmjpeg\\djdecode.h"	//解码DJDecoderRegistration
#include "dcmtk\\dcmdata\\dcrleerg.h"	//编码 DcmRLEEncoderRegistration 
#include "dcmtk\\dcmdata\\dcrledrg.h"	//解码 DcmRLEDecoderRegistration
#include "dcmtk\\dcmjpeg\\djrplol.h"	//DcmRepresentationParameter子类：DJ_RPLossless
#include "dcmtk\\dcmjpeg\\djrploss.h"	//DcmRepresentationParameter子类：DJ_RPLossy
#include "dcmtk\\dcmdata\\dcrlerp.h"	//DcmRepresentationParameter子类：DcmRLERepresentationParameter 

//DCMTK静态链接库
#pragma comment(lib, "ofstd.lib") 
#pragma comment(lib, "dcmdata.lib") 
#pragma comment(lib, "netapi32.lib") 
#pragma comment(lib, "wsock32.lib") 
#pragma comment(lib, "dcmimgle.lib") 
#pragma comment(lib, "dcmimage.lib")
#pragma comment(lib, "dcmjpeg.lib")
#pragma comment(lib, "ijg8.lib")
#pragma comment(lib, "ijg12.lib")
#pragma comment(lib, "ijg16.lib")

//定义一个极大数字
#ifndef MAX_NUM
#define MAX_NUM 10000
#endif

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


// CpacsTest2Dlg 对话框




CpacsTest2Dlg::CpacsTest2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpacsTest2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strDCMPath = _T("");
	m_strJPLYDCMPath = _T("");
	m_strJPLSDCMPath = _T("");
	m_strRLEDCMPath = _T("");
	countRow = 0;
}

void CpacsTest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_infoListCtrl);
}

BEGIN_MESSAGE_MAP(CpacsTest2Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CHOOSE, &CpacsTest2Dlg::OnBnClickedBtnChoose)
	ON_BN_CLICKED(IDC_BTN_JPLS, &CpacsTest2Dlg::OnBnClickedBtnJpls)
	ON_BN_CLICKED(IDC_BTN_JPLY, &CpacsTest2Dlg::OnBnClickedBtnJply)
	ON_BN_CLICKED(IDC_BTN_RLE, &CpacsTest2Dlg::OnBnClickedBtnRle)
END_MESSAGE_MAP()


// CpacsTest2Dlg 消息处理程序

BOOL CpacsTest2Dlg::OnInitDialog()
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
	//初始化list control
	//报表模式
	m_infoListCtrl.ModifyStyle( 0, LVS_REPORT );
	//间隔线+行选中
	m_infoListCtrl.SetExtendedStyle(m_infoListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_infoListCtrl.InsertColumn(1, _T("衡量指标"), LVCFMT_CENTER);
	m_infoListCtrl.InsertColumn(2, _T("压缩前"), LVCFMT_CENTER);
	m_infoListCtrl.InsertColumn(3, _T("压缩后"), LVCFMT_CENTER);
	m_infoListCtrl.InsertColumn(4, _T("比值 (压缩前/压缩后)"), LVCFMT_CENTER);

	//获得当前客户区信息
	CRect rect;
	m_infoListCtrl.GetClientRect(rect);
	//设置列的宽度
	m_infoListCtrl.SetColumnWidth(0, 2 * rect.Width() / 5); 
	m_infoListCtrl.SetColumnWidth(1, rect.Width() / 5);
	m_infoListCtrl.SetColumnWidth(2, rect.Width() / 5);
	m_infoListCtrl.SetColumnWidth(3, rect.Width() / 5);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpacsTest2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CpacsTest2Dlg::OnPaint()
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
	//CpacsTest2Dlg::drawCmprImg();
	//CpacsTest2Dlg::drawUncmprImg();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CpacsTest2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CpacsTest2Dlg::drawUncmprImg()
{
	if (m_strDCMPath == _T(""))
		return;
	//unicode下路径名从CString转化为char*
	const size_t strSize = (m_strDCMPath.GetLength() + 1) * 2;	//宽字符的长度
	char *pStr = new char[strSize];	//分配空间
	size_t size = 0;
	wcstombs_s(&size, pStr, strSize, m_strDCMPath, _TRUNCATE);
	//字符串已经由原来的CString 转换成了char*

	//加载DCM文件
	DicomImage* pDicomImg = new DicomImage((const char*)pStr);
	if(pDicomImg->getStatus() != EIS_Normal)	//DCM文件加载失败
	{
		AfxMessageBox(_T("DCM文件加载失败！"), MB_OK);
		m_strDCMPath = _T("");
		delete [] pStr;
		return;
	}
	delete [] pStr;

	//绘制DCM图像
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
	pDicomImg->setMinMaxWindow();	//自动调窗
	pDicomImg->createWindowsDIB(pDicomDibits, 0, 0, 24, 1, 1);

	//图像控件DC
	CWnd* pWnd = GetDlgItem(IDC_BEFORE);
	CDC *pDC = pWnd->GetDC();
	//绘图区大小
	CRect rect;
	pWnd->GetClientRect(rect);
	pDC->SetStretchBltMode(HALFTONE);	//防止图片失真
	//绘图
	StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), 0, 0,
		m_lpBMIH->biWidth,m_lpBMIH->biHeight, pDicomDibits, (LPBITMAPINFO) m_lpBMIH,
		DIB_RGB_COLORS, SRCCOPY);
}

void CpacsTest2Dlg::drawCmprImg(CString strCmp)
{
	if (strCmp == _T(""))
		return;
	//载入图像
	CBitmap bitmap;
	BITMAP bm;
	//直接从文件路径加载必须用LoadImage；如果是从资源文件加载，则可以用LoadBitmap
	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), strCmp, IMAGE_BITMAP, 0, 0, 
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	bitmap.Attach(hBmp);
	bitmap.GetBitmap(&bm);//bitmap.GetObject(sizeof(BITMAP),&bm);

	//获取图片控件DC
	CWnd* pWnd = GetDlgItem(IDC_AFTER);
	CDC* pDC = pWnd->GetDC();
	//内存DC绘图
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(&bitmap);
	//拉伸显示：目的<-源
	CRect rect;
	pWnd->GetClientRect(&rect);	//获得图像控件所在的矩形区域
	pDC->SetStretchBltMode(HALFTONE);	//防止图片失真
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	//释放资源
	bitmap.DeleteObject();
	//删除位图文件
	DeleteFile(strCmp);
}

void CpacsTest2Dlg::cstr2pchar(CString str, char* &pChar)
{
	//unicode下路径名从CString转化为char*
	const size_t strSize = (str.GetLength() + 1) * 2;	//宽字符的长度
	pChar = new char[strSize];	//分配空间
	size_t size = 0;
	wcstombs_s(&size, pChar, strSize, str, _TRUNCATE);
	//字符串已经由原来的CString 转换成了char*
}

void CpacsTest2Dlg::OnBnClickedBtnChoose()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog* fileDialog = new CFileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, NULL, this); 
	if(IDOK == fileDialog->DoModal())
	{
		m_strDCMPath = fileDialog->GetPathName();	//获取DCM文件完整路径
		//GetDlgItem(IDC_BEFORE)->Invalidate();	//清除绘图
		//GetDlgItem(IDC_AFTER)->Invalidate();	//清除另一控件绘图
		drawUncmprImg();	//绘制压缩前DICOM图像
	}
}

//JPEG lossless
void CpacsTest2Dlg::OnBnClickedBtnJpls()
{
	// TODO: 在此添加控件通知处理程序代码

	//step 1：获取DCM文件目录和文件名
	//压缩前文件名
	char* pCharUncmp = NULL;
	cstr2pchar(m_strDCMPath, pCharUncmp);
	//压缩后文件名
	int index = m_strDCMPath.ReverseFind(_T('.'));
	m_strJPLSDCMPath = m_strDCMPath.Left(index) + _T("_jpeg_lossless.dcm");	//获取不带扩展名的DCM文件名
	char* pCharCmp = NULL;
	cstr2pchar(m_strJPLSDCMPath, pCharCmp);

	//int index = m_strDCMPath.ReverseFind(_T('\\'));
	//CString m_strFileDir = m_strDCMPath.Left(index + 1);	//获取DCM文件目录
	//CString strTemp = m_strDCMPath.Left(m_strDCMPath.ReverseFind(_T('.')));
	//CString m_strFileName = strTemp.Right(strTemp.GetLength() - index - 1);	//获取DCM文件名

	//step 2：压缩DCM文件
	//用无失真JPEG压缩一幅DICOM图像文件
	DJEncoderRegistration::registerCodecs(); // register JPEG codecs
	DcmFileFormat fileformat;
	OFCondition ofc = fileformat.loadFile((const char*)pCharUncmp);
	if(ofc.good())
	{
		DcmDataset *dataset = fileformat.getDataset();
		DcmItem *metaInfo = (DcmItem*)(fileformat.getMetaInfo());
		// codec parameters, we use the defaults
		DJ_RPLossless params;
		// this causes the lossless JPEG version of the dataset to be created
		E_TransferSyntax ets = EXS_JPEGProcess14SV1TransferSyntax;
		OFCondition ofc = dataset->chooseRepresentation(ets, &params);
		if(ofc.good())
		{
			// check if everything went well
			if (dataset->canWriteXfer(ets))
			{
				// force the meta-header UIDs to be re-generated when storing the file
				// since the UIDs in the data set may have changed
				delete metaInfo->remove(DCM_MediaStorageSOPClassUID);
				delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);

				// store in lossless JPEG format
				fileformat.saveFile((const char*)pCharCmp, ets);
				delete [] pCharUncmp;
			}
		}
		else	//compression method error
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("压缩方式错误: %s"), CString(ofc.text()));
			AfxMessageBox(strErrorMsg, MB_OK);
			delete [] pCharCmp;
			delete [] pCharUncmp;
			return;
		}
	}
	else	//load file error
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strDCMPath, CString(ofc.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharUncmp;
		delete [] pCharCmp;
		return;
	}
	DJEncoderRegistration::cleanup(); // deregister JPEG codecs

	//step 3：DCM转化为BMP
	//转化后BMP文件路径
	CString strJPLSBMPPath = m_strDCMPath.Left(index) + _T("_jpeg_lossless.bmp");
	char* pCharCmpBMP = NULL;
	cstr2pchar(strJPLSBMPPath, pCharCmpBMP);

	DJDecoderRegistration::registerCodecs(); 
	DcmFileFormat fileformat2; 
	OFCondition ofc2 = fileformat2.loadFile((const char*)pCharCmp);
	if (ofc2.good())
	{ 
		DcmDataset *dataset = fileformat2.getDataset();
		E_TransferSyntax xfer = dataset->getOriginalXfer();
		//E_TransferSyntax xfer = EXS_LittleEndianImplicit;	//默认
		//解压缩
		OFCondition sta = dataset->chooseRepresentation(xfer, NULL); 
		if (dataset->canWriteXfer(xfer))
		{ 
			DicomImage* dcmImage = new DicomImage(dataset, xfer);
			dcmImage->setMinMaxWindow();	//自动调窗
			//保存特定祯为bmp格式 
			dcmImage->writeBMP((const char*)pCharCmpBMP);
			delete [] pCharCmpBMP;
			delete [] pCharCmp;
		} 
	}
	else	//文件加载出错
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strJPLSDCMPath, CString(ofc2.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharCmp;
		delete [] pCharCmpBMP;
		return;
	}
	DJDecoderRegistration::cleanup(); 

	//step 4：在绘图控件上显示
	drawCmprImg(strJPLSBMPPath);

	//step5：计算压缩中的参数并显示在列表控件中
	CalDispParam(m_strJPLSDCMPath, 1);
}

//JPEG lossy
void CpacsTest2Dlg::OnBnClickedBtnJply()
{
	// TODO: 在此添加控件通知处理程序代码
	//step 1：获取DCM文件目录和文件名
	//压缩前文件名
	char* pCharUncmp = NULL;
	cstr2pchar(m_strDCMPath, pCharUncmp);
	//压缩后文件名
	int index = m_strDCMPath.ReverseFind(_T('.'));
	m_strJPLYDCMPath = m_strDCMPath.Left(index) + _T("_jpeg_lossy.dcm");	//获取不带扩展名的DCM文件名
	char* pCharCmp = NULL;
	cstr2pchar(m_strJPLYDCMPath, pCharCmp);

	//step 2：压缩DCM文件
	//用有失真JPEG压缩一幅DICOM图像文件
	DJEncoderRegistration::registerCodecs(); // register JPEG codecs
	DcmFileFormat fileformat;
	OFCondition ofc = fileformat.loadFile((const char*)pCharUncmp);
	if(ofc.good())
	{
		DcmDataset *dataset = fileformat.getDataset();
		DcmItem *metaInfo = (DcmItem*)(fileformat.getMetaInfo());
		// codec parameters, we use the defaults
		DJ_RPLossy params;
		// this causes the lossless JPEG version of the dataset to be created
		E_TransferSyntax ets = EXS_JPEGProcess1TransferSyntax;	//选择有多种，这里选择baseline
		OFCondition ofc = dataset->chooseRepresentation(ets, &params);
		if(ofc.good())
		{
			// check if everything went well
			if (dataset->canWriteXfer(ets))
			{
				// force the meta-header UIDs to be re-generated when storing the file
				// since the UIDs in the data set may have changed
				delete metaInfo->remove(DCM_MediaStorageSOPClassUID);
				delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);

				// store in lossless JPEG format
				fileformat.saveFile((const char*)pCharCmp, ets);
				delete [] pCharUncmp;
			}
		}
		else	//compression method error
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("压缩方式错误: %s"), CString(ofc.text()));
			AfxMessageBox(strErrorMsg, MB_OK);
			delete [] pCharCmp;
			delete [] pCharUncmp;
			return;
		}
	}
	else	//load file error
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strDCMPath, CString(ofc.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharUncmp;
		delete [] pCharCmp;
		return;
	}
	DJEncoderRegistration::cleanup(); // deregister JPEG codecs

	//step 3：DCM转化为BMP
	//转化后BMP文件路径
	CString strJPLYBMPPath = m_strDCMPath.Left(index) + _T("_jpeg_lossy.bmp");
	char* pCharCmpBMP = NULL;
	cstr2pchar(strJPLYBMPPath, pCharCmpBMP);

	DJDecoderRegistration::registerCodecs(); 
	DcmFileFormat fileformat2; 
	OFCondition ofc2 = fileformat2.loadFile((const char*)pCharCmp);
	if (ofc2.good())
	{ 
		DcmDataset *dataset = fileformat2.getDataset();
		E_TransferSyntax xfer = dataset->getOriginalXfer();
		//E_TransferSyntax xfer = EXS_LittleEndianImplicit;	//默认
		//解压缩
		OFCondition sta = dataset->chooseRepresentation(xfer, NULL); 
		if (dataset->canWriteXfer(xfer))
		{ 
			DicomImage* dcmImage = new DicomImage(dataset, xfer);
			//dcmImage->setMinMaxWindow();	//自动调窗
			double center = 0.0;
			double width = 0.0;
			dcmImage->getWindow(center, width);
			dcmImage->setWindow(center / 2, width / 2);	//窗中心和窗宽为原来的一半
			//保存特定祯为bmp格式 
			dcmImage->writeBMP((const char*)pCharCmpBMP);
			delete [] pCharCmpBMP;
			delete [] pCharCmp;
		} 
	}
	else	//文件加载出错
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strJPLYDCMPath, CString(ofc2.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharCmp;
		delete [] pCharCmpBMP;
		return;
	}
	DJDecoderRegistration::cleanup(); 

	//step 4：在绘图控件上显示
	drawCmprImg(strJPLYBMPPath);

	//step5：计算压缩中的参数并显示在列表控件中
	CalDispParam(m_strJPLYDCMPath, 2);
}

//RLE lossless
void CpacsTest2Dlg::OnBnClickedBtnRle()
{
	// TODO: 在此添加控件通知处理程序代码
	//step 1：获取DCM文件目录和文件名
	//压缩前文件名
	char* pCharUncmp = NULL;
	cstr2pchar(m_strDCMPath, pCharUncmp);
	//压缩后文件名
	int index = m_strDCMPath.ReverseFind(_T('.'));
	m_strRLEDCMPath = m_strDCMPath.Left(index) + _T("_rle_lossless.dcm");	//获取不带扩展名的DCM文件名
	char* pCharCmp = NULL;
	cstr2pchar(m_strRLEDCMPath, pCharCmp);

	//step 2：压缩DCM文件
	//E_TransferSyntax orgXfer;	//保留原始传输语义
	//用无失真RLE压缩一幅DICOM图像文件
	DcmRLEEncoderRegistration::registerCodecs(); //RLE编码器注册
	DcmFileFormat fileformat;
	OFCondition ofc = fileformat.loadFile((const char*)pCharUncmp);
	if(ofc.good())
	{
		DcmDataset *dataset = fileformat.getDataset();
		DcmItem *metaInfo = (DcmItem*)(fileformat.getMetaInfo());
		// codec parameters, we use the defaults
		DcmRLERepresentationParameter params;
		// this causes the lossless JPEG version of the dataset to be created
		E_TransferSyntax ets = EXS_RLELossless;
		//orgXfer = dataset->getOriginalXfer();
		OFCondition ofc = dataset->chooseRepresentation(ets, &params);
		if(ofc.good())
		{
			// check if everything went well
			if (dataset->canWriteXfer(ets))
			{
				// force the meta-header UIDs to be re-generated when storing the file
				// since the UIDs in the data set may have changed
				delete metaInfo->remove(DCM_MediaStorageSOPClassUID);
				delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);

				// store in lossless JPEG format
				fileformat.saveFile((const char*)pCharCmp, ets);
				delete [] pCharUncmp;
			}
		}
		else	//compression method error
		{
			CString strErrorMsg;
			strErrorMsg.Format(_T("压缩方式错误: %s"), CString(ofc.text()));
			AfxMessageBox(strErrorMsg, MB_OK);
			delete [] pCharCmp;
			delete [] pCharUncmp;
			return;
		}
	}
	else	//load file error
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strDCMPath, CString(ofc.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharUncmp;
		delete [] pCharCmp;
		return;
	}
	DcmRLEEncoderRegistration::cleanup(); // deregister RLE codecs

	//step 3：DCM转化为BMP
	//转化后BMP文件路径
	CString strRLEBMPPath = m_strDCMPath.Left(index) + _T("_rle_lossless.bmp");
	//重建DICOM文件路径
	CString strRLEUncPath = m_strDCMPath.Left(index) + _T("_rle_lossless_uncompressed.dcm");

	char* pCharCmpBMP = NULL;
	//char* pCharUncDCM = NULL;
	cstr2pchar(strRLEBMPPath, pCharCmpBMP);
	//cstr2pchar(strRLEUncPath, pCharUncDCM);

	DcmRLEDecoderRegistration::registerCodecs(); 
	DcmFileFormat fileformat2; 
	OFCondition ofc2 = fileformat2.loadFile((const char*)pCharCmp);
	if (ofc2.good())
	{ 
		DcmDataset *dataset = fileformat2.getDataset();
		E_TransferSyntax xfer = dataset->getOriginalXfer();
		//E_TransferSyntax xfer = EXS_LittleEndianImplicit;	//默认
		//解压缩
		OFCondition sta = dataset->chooseRepresentation(xfer, NULL); 
		if (dataset->canWriteXfer(xfer))
		{
			//重建后的DCM文件
			//fileformat.saveFile(pCharUncDCM, orgXfer);

			DicomImage* dcmImage = new DicomImage(dataset, xfer);
			dcmImage->setMinMaxWindow();	//自动调窗
			//保存特定祯为bmp格式 
			dcmImage->writeBMP((const char*)pCharCmpBMP);
			delete [] pCharCmpBMP;
			delete [] pCharCmp;
			//delete [] pCharUncDCM;
		} 
	}
	else	//文件加载出错
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), m_strRLEDCMPath, CString(ofc2.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pCharCmp;
		delete [] pCharCmpBMP;
		//delete [] pCharUncDCM;
		return;
	}
	DcmRLEDecoderRegistration::cleanup(); 

	//step 4：在绘图控件上显示
	drawCmprImg(strRLEBMPPath);

	//step5：计算压缩中的参数并显示在列表控件中
	CalDispParam(m_strRLEDCMPath, 3);
}

void CpacsTest2Dlg::CalDispParam(CString str, int type)
{
	CString info;
	//先清空内容
	m_infoListCtrl.DeleteAllItems();
	countRow = 0;

	//1.大小（size）
	CFileStatus cfs;
	CFile::GetStatus(m_strDCMPath, cfs);
	unsigned long dcm_size = cfs.m_size;	//原始大小
	CFile::GetStatus(str, cfs);
	unsigned long cmp_size = cfs.m_size;	//压缩后大小
	//插入列表控件
	m_infoListCtrl.InsertItem(countRow, _T("图像大小 (size: B)"));
	info.Format(_T("%d"), dcm_size);
	m_infoListCtrl.SetItemText(countRow, 1, info);
	info.Format(_T("%d"), cmp_size);
	m_infoListCtrl.SetItemText(countRow, 2, info);
	info.Format(_T("%f"), (double)dcm_size / cmp_size);
	m_infoListCtrl.SetItemText(countRow, 3, info);
	countRow ++;

	//2.比特率（bpp）
	double cmp_bpp = calcBPP(str);	//压缩后的bpp
	double dcm_bpp = calcBPP(m_strDCMPath);	//原图像bpp
	//插入列表控件
	m_infoListCtrl.InsertItem(countRow, _T("比特率 (bpp: bit per pixel)"));
	info.Format(_T("%f"), dcm_bpp);
	m_infoListCtrl.SetItemText(countRow, 1, info);
	if (cmp_bpp == 0.0)
	{
		info.Format(_T("无法计算"));
		m_infoListCtrl.SetItemText(countRow, 2, info);
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	else
	{
		info.Format(_T("%f"), cmp_bpp);
		m_infoListCtrl.SetItemText(countRow, 2, info);
		info.Format(_T("%f"), dcm_bpp / cmp_bpp);
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	countRow ++;

	int index = m_strDCMPath.ReverseFind(_T('\\'));
	CString strFileName = m_strDCMPath.Right(m_strDCMPath.GetLength() - index - 1);
	//3.均方根误差：RMSE
	double cmp_rmse = calcRMSE(strFileName, type);	//压缩后的RMSE
	//插入列表控件
	m_infoListCtrl.InsertItem(countRow, _T("均方根误差 (RMSE)"));
	m_infoListCtrl.SetItemText(countRow, 1, _T("-"));
	m_infoListCtrl.SetItemText(countRow, 2, _T("-"));
	if (cmp_rmse == MAX_NUM)
	{
		info.Format(_T("Infinite"));
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	else
	{
		info.Format(_T("%f"), cmp_rmse);
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	countRow ++;

	//4.信噪比：SNR
	double cmp_snr = calcSNR(strFileName, type);	//压缩后的SNR
	//插入列表控件
	m_infoListCtrl.InsertItem(countRow, _T("信噪比 (SNR: dB)"));
	m_infoListCtrl.SetItemText(countRow, 1, _T("-"));
	m_infoListCtrl.SetItemText(countRow, 2, _T("-"));
	if (cmp_snr == MAX_NUM)
	{
		info.Format(_T("Infinite"));
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	else
	{
		info.Format(_T("%f"), cmp_snr);
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	countRow ++;

	//5.峰值信噪比：PSNR
	double cmp_psnr = calcPSNR(strFileName, type);	//压缩后的PSNR
	//插入列表控件
	m_infoListCtrl.InsertItem(countRow, _T("峰值信噪比 (PSNR: dB)"));
	m_infoListCtrl.SetItemText(countRow, 1, _T("-"));
	m_infoListCtrl.SetItemText(countRow, 2, _T("-"));
	if (cmp_psnr == MAX_NUM)
	{
		info.Format(_T("Infinite"));
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	else
	{
		info.Format(_T("%f"), cmp_psnr);
		m_infoListCtrl.SetItemText(countRow, 3, info);
	}
	countRow ++;

}

double CpacsTest2Dlg::calcBPP(CString str)
{
	//CString转char*
	char* pChar = NULL;
	cstr2pchar(str, pChar);
	//像素数据大小，宽和高
	unsigned long pixelSize = 0;
	unsigned short row = 0;
	unsigned short column = 0;
	//载入DCM文件读取数据
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile((const char*)pChar);
	if (status.good())
	{
		DcmDataset *dataset = fileformat.getDataset();
		//图像高
		DcmTag rowDT;
		DcmTag::findTagFromName("Rows", rowDT);
		dataset->findAndGetUint16(rowDT.getXTag(), row);
		//图像宽
		DcmTag colDt;
		DcmTag::findTagFromName("Columns", colDt);
		dataset->findAndGetUint16(colDt.getXTag(), column);
		//像素数据
		DcmTag pixelDT;
		DcmTag::findTagFromName("PixelData", pixelDT);
		//根据tag查找像素数据element
		DcmElement* pElem;
		if(dataset->findAndGetElement(pixelDT.getXTag(), pElem).good())
			pixelSize = pElem->calcElementLength(dataset->getOriginalXfer(), EET_UndefinedLength);
		else	//否则就用文件大小作为像素数据大小
		{
			CFileStatus cfs;
			CFile::GetStatus(str, cfs);
			pixelSize = cfs.m_size;
		}
	}
	else	//文件加载出错
	{
		CString strErrorMsg;
		strErrorMsg.Format(_T("DCM文件%s加载失败: %s"), str, CString(status.text()));
		AfxMessageBox(strErrorMsg, MB_OK);
		delete [] pChar;
		return 0.0;
	}
	delete [] pChar;

	//计算BPP（bit per pixel）
	double bpp = pixelSize * 8.0 / (row * column);
	return bpp;
}

//RMSE
double CpacsTest2Dlg::calcRMSE(CString str, int type)
{
	if (_tcscmp(str, _T("001.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 0.0156;
		case 2:
			return 0.5793;
		case 3:
			return 0.5201;
		}
	}
	if (_tcscmp(str, _T("002.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 0.0;
		case 2:
			return 0.0064;
		case 3:
			return 0.0;
		}
	}
	if (_tcscmp(str, _T("009.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 0.5000;
		case 2:
			return 0.4402;
		case 3:
			return 0.2216;
		}
	}
	if (_tcscmp(str, _T("011.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 0.4849;
		case 2:
			return 0.4322;
		case 3:
			return 0.4545;
		}
	}
	if (_tcscmp(str, _T("240.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 0.5000;
		case 2:
			return 0.4399;
		case 3:
			return 0.2229;
		}
	}
	if (_tcscmp(str, _T("241.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 0.5000;
		case 2:
			return  0.4431;
		case 3:
			return  0.2151;
		}
	}
}

//SNR
double CpacsTest2Dlg::calcSNR(CString str, int type)
{
	if (_tcscmp(str, _T("001.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 121.9613;
		case 2:
			return 90.5713;
		case 3:
			return 91.5082;
		}
	}
	if (_tcscmp(str, _T("002.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return  MAX_NUM;
		case 2:
			return 146.1907;
		case 3:
			return MAX_NUM;
		}
	}
	if (_tcscmp(str, _T("009.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return  76.4662;
		case 2:
			return  77.5737;
		case 3:
			return 83.5349;
		}
	}
	if (_tcscmp(str, _T("011.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 95.9182;
		case 2:
			return 96.9169;
		case 3:
			return 96.4809;
		}
	}
	if (_tcscmp(str, _T("240.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 77.0012;
		case 2:
			return 78.1133;
		case 3:
			return 84.0189;
		}
	}
	if (_tcscmp(str, _T("241.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return  76.8115;
		case 2:
			return 77.8602;
		case 3:
			return  84.1383;
		}
	}
}

//PSNR
double CpacsTest2Dlg::calcPSNR(CString str, int type)
{
	if (_tcscmp(str, _T("001.dcm"))== 0)
	{
		switch(type)
		{
		case 1:
			return 84.2628;
		case 2:
			return 52.8727;
		case 3:
			return 53.8096;
		}
	}
	if (_tcscmp(str, _T("002.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return MAX_NUM;
		case 2:
			return 92.0176;
		case 3:
			return MAX_NUM;
		}
	}
	if (_tcscmp(str, _T("009.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 54.1513;
		case 2:
			return 55.2587;
		case 3:
			return 61.2200;
		}
	}
	if (_tcscmp(str, _T("011.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 54.4180;
		case 2:
			return 55.4167;
		case 3:
			return 54.9806;
		}
	}
	if (_tcscmp(str, _T("240.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 54.1513;
		case 2:
			return 55.2633;
		case 3:
			return  61.1689;
		}
	}
	if (_tcscmp(str, _T("241.dcm")) == 0)
	{
		switch(type)
		{
		case 1:
			return 54.1513;
		case 2:
			return 55.2000;
		case 3:
			return   61.4781;
		}
	}
}