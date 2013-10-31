// pacsTest2Dlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CpacsTest2Dlg 对话框
class CpacsTest2Dlg : public CDialog
{
// 构造
public:
	CpacsTest2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PACSTEST2_DIALOG };

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
public:
	CString m_strDCMPath;	//压缩前DCM文件完整路径
	CString m_strJPLYDCMPath;	//压缩后DCM文件完整路径：JPEG lossy
	CString m_strJPLSDCMPath;	//压缩后DCM文件完整路径：JPEG lossless
	CString m_strRLEDCMPath;	//压缩后DCM文件完整路径：RLE lossless

	CListCtrl m_infoListCtrl;	//列表控件
	unsigned int countRow;	//列表控件行计数
public:
	void drawUncmprImg();							//绘制原始DICOM图像
	void drawCmprImg(CString strCmp);	//绘制压缩后DICOM图像

	void cstr2pchar(CString str, char* &pChar);	//unicode下CString转为char*

	void CalDispParam(CString str, int type);	//计算并显示各类压缩参数
	double calcBPP(CString str);			//1.计算比特率
	double calcRMSE(CString str, int type);		//2.计算均方根误差
	double calcSNR(CString str, int type);			//3.计算信噪比
	double calcPSNR(CString str, int type);		//4.计算峰值信噪比

	//按钮响应
	afx_msg void OnBnClickedBtnChoose();	//选择文件
	afx_msg void OnBnClickedBtnJpls();		//JPEG无损压缩
	afx_msg void OnBnClickedBtnJply();		//JPEG有损压缩
	afx_msg void OnBnClickedBtnRle();			//RLE无损压缩
};
