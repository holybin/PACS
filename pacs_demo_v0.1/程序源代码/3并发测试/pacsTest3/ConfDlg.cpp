// ConfDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pacsTest3.h"
#include "ConfDlg.h"
#include "Protocol.h"

// CConfDlg 对话框

IMPLEMENT_DYNAMIC(CConfDlg, CDialog)

CConfDlg::CConfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfDlg::IDD, pParent)
{

}

CConfDlg::~CConfDlg()
{
}

void CConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipAddrCtrl);
}


BEGIN_MESSAGE_MAP(CConfDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CConfDlg::OnIpnFieldchangedIpaddress1)
END_MESSAGE_MAP()


// CConfDlg 消息处理程序

void CConfDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	BYTE b1,b2,b3,b4;
	//num 是几个有效的域，比如211.155.224.*有3个有效的数字，就返回3。
	int num = m_ipAddrCtrl.GetAddress(b1,b2,b3,b4);
	m_strIPConf.Format(_T("%d.%d.%d.%d"), b1, b2, b3, b4);
}

BOOL CConfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_strIPConf = _T("");
	//测试用
	//m_strIPConf = _T("10.13.0.84");
	//m_strIPConf = _T("192.168.56.1");
	//m_strIPConf = _T("127.0.0.1");

	SetDlgItemInt(IDC_EDIT_PORT, PRE_AGREED_PORT);	//设置默认通信端口号

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
