#include "StdAfx.h"
#include "DrawGraph.h"
#include "pacsTest3.h"			//应用程序头文件
#include "pacsTest3Dlg.h"	//主对话框头文件

DrawGraph::DrawGraph(void)
{
}

DrawGraph::~DrawGraph(void)
{
}

//绘制初始框架
void DrawGraph::drawFrame()
{
	//获取绘图区窗口指针
	CpacsTest3Dlg* pMainWnd = (CpacsTest3Dlg*)(AfxGetApp()->GetMainWnd());
	CWnd* pWnd = pMainWnd->GetDlgItem(IDC_GRAPH);
	CRect rect;
	pWnd->GetClientRect(&rect);
	//pWnd->ClientToScreen(&rect);
	// 绘图区起始坐标
	x = rect.left;
	y = rect.top;
	// 绘图起始点
	xpoint1 = x;
	ypoint1 = y + height;
	xpoint2 = x;
	ypoint2 = y + height;
	// 绘图区大小
	width = rect.right - x;
	height = rect.bottom - y;
	// 绘制刻度用数据
	int nx = 20;	//均分为20等分
	int ny = 20;	//均分为20等分
	xinterval = width / nx; 
	yinterval = height / ny;
	//刷新绘图区
	//pWnd->Invalidate();
	//pWnd->UpdateWindow();

	//获取设备上下文
	CDC* pDC = pWnd->GetDC();	
	// 创建画笔对象
	CPen* pPenBlack = new CPen;
	// 创建黑色画笔（用于绘制坐标轴）
	pPenBlack->CreatePen(PS_SOLID, 1, RGB(0,0,0));
	// 选入黑色画笔
	pDC->SelectObject(pPenBlack);

	//设置字体
	CFont newFont;
	VERIFY(newFont.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));                 // lpszFacename 

	// 选入自定义字体，并保存以前的字体
	CFont* oldFont = pDC->SelectObject(&newFont);
	pDC->SetTextColor(RGB(0, 0, 0));	//字体颜色
	pDC->SetTextAlign(TA_CENTER);	//字体排列

	// 绘制坐标轴
	// 绘制Y轴
	pDC->MoveTo(x, y + height);
	pDC->LineTo(x, y );
	// 绘制X轴
	pDC->MoveTo(x, y + height);
	pDC->LineTo(x + width, y + height);
	// 绘制坐标轴刻度
	// 绘制X轴刻度值
	for (int i = 0; i <= 20; i++)
	{
		CString strTemp;
		strTemp.Format(_T("%d"), i * 5);
		pDC->TextOut(i * xinterval, height + y + 2, strTemp);

		pDC->MoveTo(i * xinterval, height + y);
		pDC->LineTo(i * xinterval, height + y - 5);
	}
	// 绘制Y轴刻度
	for (int i = 1; i <= 100; i++)
	{
		if(i % 5 == 0)
		{
			// 左边Y轴刻度
			pDC->MoveTo(x, height - i * yinterval / 5);
			pDC->LineTo(x + 8, height - i * yinterval / 5);
			// 右边Y轴刻度
			pDC->MoveTo(x + width - 8, height - i * yinterval / 5);
			pDC->LineTo(x + width, height - i * yinterval / 5);
		}
		else
		{
			// 左边Y轴刻度
			pDC->MoveTo(x, height - i * yinterval / 5);
			pDC->LineTo(x + 5, height - i * yinterval / 5);
			// 右边Y轴刻度
			pDC->MoveTo(x - 5 + width, height - i * yinterval / 5);
			pDC->LineTo(x + width, height - i * yinterval / 5);
		}
	}
	// 绘制Y轴刻度值
	for (int i = 0; i <= 20; i++)
	{
		//左边刻度
		CString strTemp;
		strTemp.Format(_T("%d%%"), i * 5);	//输出百分号数字
		pDC->TextOut(x - 12, y + height - (i * yinterval + 4), strTemp);
		//右边刻度
		strTemp.Format(_T("%d ms"), i * 500);	//输出时间：ms
		pDC->TextOut(x + 20 + width , y + height - (i * yinterval + 4), strTemp);
	}

	//绘制提示
	//创建画笔对象
	CPen* pPenBlue = new CPen;
	// 创建蓝色画笔（用于绘制曲线）
	pPenBlue->CreatePen(PS_SOLID, 1, RGB(0,0,255));
	// 选入蓝色画笔
	pDC->SelectObject(pPenBlue);
	pDC->MoveTo(x + width - 100, y + 20);
	pDC->LineTo(x + width - 80, y + 20);
	pDC->TextOut(x + width - 50, y + 15, _T("传输时间"));
	//刷新绘图区
	//pWnd->Invalidate();
	//pWnd->UpdateWindow();
	//创建画笔对象
	CPen* pPenRed = new CPen;
	// 创建红色画笔（用于绘制曲线）
	pPenRed->CreatePen(PS_SOLID, 1, RGB(255,0,0));
	// 选入红色画笔
	pDC->SelectObject(pPenRed);
	pDC->MoveTo(x + width - 100, y + 40);
	pDC->LineTo(x + width - 80, y + 40);
	pDC->TextOut(x + width - 50, y + 35, _T("传输速率"));

	// 释放图像控件的DC
	pMainWnd->ReleaseDC(pDC);
	// 删除新的画笔
	delete pPenBlack;
	delete pPenBlue;
	delete pPenRed;
}

//绘图
void DrawGraph::drawGraph(GraphData graphData)
//void CGraphDialog::drawGraph()
{
	//获取绘图区窗口指针
	CpacsTest3Dlg* pMainWnd = (CpacsTest3Dlg*)(AfxGetApp()->GetMainWnd());
	CWnd* pWnd = pMainWnd->GetDlgItem(IDC_GRAPH);
	//获取设备上下文
	CDC* pDC = pWnd->GetDC();	
	//刷新绘图区
	//pWnd->Invalidate();
	//pWnd->UpdateWindow();

	//绘制传输时间变化曲线
	//创建画笔对象
	CPen* pPenBlue = new CPen;
	// 创建蓝色画笔（用于绘制曲线）
	pPenBlue->CreatePen(PS_SOLID, 1, RGB(0,0,255));
	// 选入蓝色画笔
	pDC->SelectObject(pPenBlue);
	double spentTime = graphData.efficiency;
	//将时间映射到ms级别绘图
	if (spentTime>=10.0)	//响应时间超过10S则按照10S绘图
		spentTime = 10.0;
	spentTime *=10;
	//绘图
	pDC->MoveTo(xpoint1, ypoint1);
	xpoint1 = x + graphData.clientID * xinterval / 5;
	ypoint1 = y + (100 - spentTime) * yinterval / 5;
	pDC->LineTo(xpoint1, ypoint1);

	//刷新绘图区
	//pWnd->Invalidate();
	//pWnd->UpdateWindow();
	//绘制传输率（负载率：以1MB/s为基准）变化曲线
	//创建画笔对象
	CPen* pPenRed = new CPen;
	// 创建红色画笔（用于绘制曲线）
	pPenRed->CreatePen(PS_SOLID, 1, RGB(255,0,0));
	// 选入红色画笔
	pDC->SelectObject(pPenRed);
	double transRate = graphData.transRate / 1024;	//将B/s换算成kB/s
	//将传输率映射到1MB/s级别绘图
	if (transRate >= 1024.0)	//传输速度超过1MB/s则安1MB/s绘图
		transRate = 1024.0;
	transRate = transRate / 10.24;
	//绘图
	pDC->MoveTo(xpoint2, ypoint2);
	xpoint2 = x + graphData.clientID * xinterval / 5;
	ypoint2 = y + (100 - transRate) * yinterval / 5;
	pDC->LineTo(xpoint2, ypoint2);

	// 释放图像控件的DC
	pMainWnd->ReleaseDC(pDC);
	// 删除新的画笔
	delete pPenBlue;
	delete pPenRed;
}
