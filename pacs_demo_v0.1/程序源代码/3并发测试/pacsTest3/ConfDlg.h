#pragma once
#include "afxcmn.h"


// CConfDlg 对话框

class CConfDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfDlg)

public:
	CConfDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfDlg();

// 对话框数据
	enum { IDD = IDD_CONFDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	CIPAddressCtrl m_ipAddrCtrl;
	CString m_strIPConf;
	virtual BOOL OnInitDialog();
};
