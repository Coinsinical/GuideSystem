#include <graphics.h>
#include "EasyXPng.h"
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#define WIDTH 800
#define HEIGHT 600

//按钮函数
void button(int x, int y, int w, int h, const TCHAR* text)
{
	setbkmode(TRANSPARENT);
	setfillcolor(GREEN);
	setlinecolor(BLUE);
	fillroundrect(x, y, x + w, y + h, 10, 10);
	settextstyle(30, 0, L"黑体");
	int tx = x + (w - textwidth(text)) / 2;
	int ty = y + (h - textheight(text)) / 2;
	outtextxy(tx, ty, text);
}

//景点类
class Spot
{
public:
	float x, y; // 用来刻画景点的中心坐标
	bool isClicked;//用来判断景点是否被点击
	wstring name; //景点名称
	wstring code; //景点代号
	wstring intro; //景点简介

	void initialize(float x, float y, bool isClicked, wstring name, wstring code, wstring intro) // 初始化
	{
		this->x = x;
		this->y = y;
		this->isClicked = isClicked;
		this->name = name;
		this->code = code;
		this->intro = intro;
	}

	void draw(float x, float y)// 显示相关信息    
	{
		fillcircle(x, y, 5);
	}
};

class Edge
{
public:
	int destination;
	int length;
	Edge(int d, int l) : destination(d), length(l) {}
};
// 地图类
class Map
{
public:
	IMAGE im_map; // 地图图像    
	vector<Spot> spots; //多个景点
	vector<Spot> spots_temp;//在最短路径上的点
	vector<vector<Edge>> adjacencyList;//邻接表

	float data_1[14][2];
	float data_2[15][3];
	string str1[15], str2[15], str3[15];
	wchar_t wstr[3][128];

	int status = 0;//是否需要高亮绘制最短路径
	wstring info = L"最短路径：";
	wstring info_2 = L"最短路径长度：";

	// 初始化
	void initialize()
	{
		loadimage(&im_map, _T("map.jpg")); // 导入地图图片

		readRecordFile();

		// 初始化景点
		Spot spot;

		for (int i = 0; i <= 13; i++) {
			MultiByteToWideChar(CP_UTF8, 0, str1[i].c_str(), -1, wstr[0], 128);
			MultiByteToWideChar(CP_UTF8, 0, str2[i].c_str(), -1, wstr[1], 128);
			MultiByteToWideChar(CP_UTF8, 0, str3[i].c_str(), -1, wstr[2], 128);

			spot.initialize(data_1[i][0], data_1[i][1], false, wstr[0], wstr[1], wstr[2]);
			spots.push_back(spot);
		}
		adjacencyList.resize(spots.size());
	}

	void draw()// 显示相关信息    
	{
		// 显示地图
		putimage(0, 0, &im_map);

		// 绘制所有景点
		for (int i = 0; i < spots.size(); i++)
		{
			spots[i].draw(spots[i].x, spots[i].y); // 在坐标(x,y)处画一个半径为10的圆	
		}

		setlinecolor(RED);
		setlinestyle(PS_SOLID, 1);
		for (int i = 0; i < adjacencyList.size(); i++) {
			for (int j = 0; j < adjacencyList[i].size(); j++) {
				int a = i;//记录第一个顶点的信息
				int b = adjacencyList[i][j].destination;//记录第二个顶点的信息
				line(spots[a].x, spots[a].y, spots[b].x, spots[b].y);
			}
		}

		if (status != 0) {
			setlinecolor(YELLOW);
			setlinestyle(PS_SOLID, 5);
			for (int i = 0; i < spots_temp.size() - 1; i++) {
				line(spots_temp[i].x, spots_temp[i].y, spots_temp[i + 1].x, spots_temp[i + 1].y);
			}
			settextcolor(BLUE);//设置字体颜色为蓝色

			settextstyle(18, 0, _T("宋体"));//设置字体

			LPCTSTR output = (LPCTSTR)info.c_str();//转换数据格式

			outtextxy(0, 0, output);

			output = (LPCTSTR)info_2.c_str();//转换数据格式

			outtextxy(0, 25, output);
		}

		setlinestyle(PS_SOLID, 1);

		//绘制按钮
		button(625, 90, 150, 60, L"1.景点查询");
		button(625, 210, 150, 60, L"2.路径查询");
		button(625, 330, 150, 60, L"3.问路查询");
		button(625, 450, 150, 60, L"4.退出系统");
	}

	void addEdge(int source, int destination, int length) {
		adjacencyList[source].emplace_back(destination, length);
		adjacencyList[destination].emplace_back(source, length);
	}

	void dijkstraShortestPath(int source, int destination) {
		status = 1;//需要高亮绘制路径了
		int numSpots = spots.size();
		vector<int> distance(numSpots, INT_MAX);
		vector<int> previous(numSpots, -1);
		vector<bool> visited(numSpots, false);
		priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

		distance[source] = 0;
		pq.push(make_pair(0, source));

		while (!pq.empty()) {
			int current = pq.top().second;
			pq.pop();

			if (current == destination)
				break;

			if (visited[current])
				continue;

			visited[current] = true;

			for (const Edge& edge : adjacencyList[current]) {
				int neighbor = edge.destination;
				int length = edge.length;

				if (!visited[neighbor] && distance[current] + length < distance[neighbor]) {
					distance[neighbor] = distance[current] + length;
					previous[neighbor] = current;
					pq.push(make_pair(distance[neighbor], neighbor));
				}
			}
		}

		if (distance[destination] == INT_MAX) {
			MessageBox(GetHWnd(), L"无法从起点到达目的地", L"提示", MB_ICONERROR | MB_OK);
		}
		else {
			wostringstream oss;
			oss << distance[destination];
			wstring wstr = oss.str();
			int current = destination;
			vector<int> path;
			while (current != -1) {
				path.push_back(current);
				current = previous[current];
			}

			for (int i = path.size() - 1; i >= 0; --i) {
				info += spots[path[i]].name;
				spots_temp.push_back(spots[path[i]]);

				if (i > 0)
					info += L" -> ";
			}
			info_2 += wstr;
		}
	}

	Spot clickSpot(float x, float y) {
		for (int i = 0; i < spots.size(); i++)
		{
			if (x >= spots[i].x - 5 && x <= spots[i].x + 5 && y >= spots[i].y - 5 && y <= spots[i].y + 5) {
				return spots[i];
			}
		}
		return Spot();
	}

	void readRecordFile()  //读取游戏数据文件存档
	{
		ifstream ifs("data.txt");

		char c[100];
		for (int i = 0; i <= 13; i++) {
			ifs >> data_1[i][0] >> data_1[i][1] >> str1[i] >> str2[i] >> str3[i];
		}
		for (int i = 0; i <= 14; i++) {
			ifs >> data_2[i][0] >> data_2[i][1] >> data_2[i][2];
		}
		// 关闭文件
		ifs.close();
	}

};

// 一些全局变量

Map map; // 定义地图全局对象

void startup() // 初始化
{
	map.initialize(); // 地图初始化
	for (int i = 0; i <= 14; i++) {
		map.addEdge(map.data_2[i][0], map.data_2[i][1], map.data_2[i][2]);
	}
	initgraph(WIDTH, HEIGHT); // 新开一个画面
	setbkcolor(RGB(241, 236, 243)); // 设置背景颜色
	BeginBatchDraw(); // 开始批量绘制
}

void show() // 显示
{
	cleardevice();  // 以背景颜色清空画布
	setlinecolor(RGB(0, 0, 255)); // 设置针颜色为蓝色
	map.draw(); // 显示地图相关信息
	FlushBatchDraw(); // 批量绘制
}

void updateWithInput() // 和输入有关的更新
{
	ExMessage msg;
	if (peekmessage(&msg, EM_MOUSE))
	{
		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			if (msg.x >= 625 && msg.x <= 625 + 150 && msg.y >= 90 && msg.y <= 90 + 60)
			{
				map.status = 0;
				map.spots_temp.clear();
				// 使用MessageBox显示信息框
				MessageBox(GetHWnd(), L"请直接在地图上点击相应的景点", L"景点查询", MB_ICONINFORMATION | MB_OK);
			}
			if (msg.x >= 625 && msg.x <= 625 + 150 && msg.y >= 210 && msg.y <= 210 + 60)
			{
				map.status = 0;
				map.spots_temp.clear();
				// 使用MessageBox显示信息框
				MessageBox(GetHWnd(), L"请直接在地图上点击相应的路径", L"路径查询", MB_ICONINFORMATION | MB_OK);
			}
			if (msg.x >= 625 && msg.x <= 625 + 150 && msg.y >= 330 && msg.y <= 330 + 60)
			{
				map.status = 0;
				map.spots_temp.clear();
				// 使用MessageBox显示信息框
				/*MessageBox(GetHWnd(), L"请在地图上依次点击两个点，作为起点和终点", L"问路信息", MB_ICONINFORMATION | MB_OK);*/
				wchar_t input1[20];
				wchar_t input2[20];

				InputBox(input1, 60, L"请输入起点", L"起点", L"", 480, 0, false);
				InputBox(input2, 400, L"请输入终点", L"终点", L"", 480, 0, false);
				int destination = -1, source = -1;

				for (int i = 0; i <= 13; i++) {
					MultiByteToWideChar(CP_UTF8, 0, map.str1[i].c_str(), -1, map.wstr[0], 128);
					if (wcscmp(input1, map.wstr[0]) == 0) {
						source = i;
						break;
					}
				}

				for (int i = 0; i <= 13; i++) {
					MultiByteToWideChar(CP_UTF8, 0, map.str1[i].c_str(), -1, map.wstr[0], 128);
					if (wcscmp(input2, map.wstr[0]) == 0) {
						destination = i;
						break;
					}
				}

				if (source == -1 || destination == -1) {
					MessageBox(GetHWnd(), L"你输的是啥呀", L"出错了！", MB_ICONINFORMATION | MB_OK);
				}
				else {
					map.dijkstraShortestPath(source, destination);
				}
			}
			if (msg.x >= 625 && msg.x <= 625 + 150 && msg.y >= 450 && msg.y <= 450 + 60)
			{
				exit(0);
			}

			else
			{
				Spot spot = map.clickSpot(msg.x, msg.y);
				if (spot.name != L"")
				{
					map.status = 0;
					map.spots_temp.clear();
					wstring info = L"景点名称：" + spot.name + L"\n\n";
					info += L"景点代号：" + spot.code + L"\n\n";
					info += L"景点简介：" + spot.intro;
					// 使用MessageBox显示信息框
					MessageBoxW(GetHWnd(), info.c_str(), L"景点信息", MB_OK);
				}
			}
		}
	}
}

int main() // 主函数
{
	startup(); // 初始化
	while (1) // 一直循环
	{
		show(); // 显示
		updateWithInput(); // 与输入有关的更新
	}
	return 0;
}