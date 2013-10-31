// pacsTestServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

// CpacsTestServerDlg 对话框
class CpacsTestServerDlg : public CDialog
{
// 构造
public:
	CpacsTestServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PACSTESTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

//自定义变量
public:
	CString m_strIP;	//IP地址
	CListCtrl m_msgListCtrl;	//消息列表控件
	CWinThread* m_pThread;	//服务器线程
	CString  strDCMPath;	//DCM文件目录
// 自定义成员函数
protected:
	virtual void OnOK();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//启动服务器
	afx_msg void OnBnClickedBtnStart();
	//服务器线程函数
	friend UINT serverThreadFunc( LPVOID pParam );
	//服务端socket线程函数
	friend UINT socketThreadFunc(LPVOID pParam);

};
