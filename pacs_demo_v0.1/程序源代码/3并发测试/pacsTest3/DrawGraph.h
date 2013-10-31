#pragma once

//绘图所用的传输数据
typedef struct GraphData
{
	unsigned int clientID;	//客户端计数
	double efficiency;	//效率：平均传输时间，以second为单位
	double transRate;	//传输率：传输数据量/时间，以Byte/second为单位
}GraphData;

class DrawGraph
{
public:
	DrawGraph(void);
	~DrawGraph(void);

//	绘图函数及数据
public:
	// 绘图函数
	void drawGraph(GraphData graphData);
	//void drawGraph();
	// 绘制初始框架
	void drawFrame();
	// 绘图区起始坐标
	double x;
	double y;
	// 绘图区大小
	double width;
	double height;
	// 刻度等分
	double  xinterval;
	double yinterval;
	// 绘图起始点：时间
	double xpoint1;
	double ypoint1;
	// 绘图起始点：传输率
	double xpoint2;
	double ypoint2;
};
