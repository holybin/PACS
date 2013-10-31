// pacsTest3Dlg.h : 头文件
//
#pragma once

#include "DrawGraph.h"			//绘图类头文件
#include <list>
using namespace std;

// CpacsTest3Dlg 对话框
class CpacsTest3Dlg : public CDialog
{
// 构造
public:
	CpacsTest3Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PACSTEST3_DIALOG };

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
protected:
	virtual void OnOK();
// 自定义成员变量
public:
	CString m_strIP;	//服务器IP地址
	CWinThread* m_graphThread;	//绘图线程
	CString  m_strDCMPath;	//DCM文件目录
	list<GraphData> m_transDataSet;	//存储所有传输数据，用于计算绘图数据
	list<GraphData> m_graphDataSet;	//存储绘图数据，用于绘图类进行绘图

// 自定义成员函数
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnConfigure();	//网络配置
	afx_msg void OnBnClickedBtnExit();				//退出
	afx_msg void OnBnClickedBtnAddclient();	//增加客户端
	//客户端socket线程
	friend	UINT socketThreadFunc(LPVOID pParam);
	//客户端绘图线程
	bool isSuspent;	//是否挂起
	friend UINT graphThreadFunc( LPVOID pParam );

};
