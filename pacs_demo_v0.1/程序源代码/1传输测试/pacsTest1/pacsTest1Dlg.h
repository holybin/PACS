// pacsTest1Dlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CpacsTest1Dlg 对话框
class CpacsTest1Dlg : public CDialog
{
// 构造
public:
	CpacsTest1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PACSTEST1_DIALOG };

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
// 自定义成员变量
public:
	//图像信息列表控件
	CListCtrl m_infoListCtrl1;
	CListCtrl m_infoListCtrl2;
	CListCtrl m_infoListCtrl3;
	//服务器IP地址
	CString m_strIP;
	//DCM文件目录
	CString  m_strDCMPath;
// 自定义成员函数
public:
	void initListCtrl(CListCtrl* listCtrl);		//初始化图像信息列表控件
	void rstImgAndListCtrl();		//重置图像显示控件和信息列表控件
	afx_msg void OnBnClickedBtnConfigure();	//配置网络参数
	afx_msg void OnBnClickedBtnExit();		//退出按钮
	afx_msg void OnBnClickedBtnTest();	//传输测试
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//客户端socket线程函数
	friend UINT socketThreadFunc1( LPVOID pParam );
	friend UINT socketThreadFunc2( LPVOID pParam );
	friend UINT socketThreadFunc3( LPVOID pParam );

protected:
	virtual void OnOK();
};
