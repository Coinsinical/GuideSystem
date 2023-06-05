#include <graphics.h>
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
	setfillcolor(LIGHTGRAY);
	setlinecolor(DARKGRAY);
	fillroundrect(x, y, x + w, y + h, 10, 10);
	settextstyle(25, 0, L"黑体");
	settextcolor(BLUE); // 设置文字颜色为黑色
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

	void initialize(float spot_x, float spot_y, bool spot_isClicked, wstring spot_name, wstring spot_code, wstring spot_intro) // 初始化
	{
		x = spot_x;
		y = spot_y;
		isClicked = spot_isClicked;
		name = spot_name;
		code = spot_code;
		intro = spot_intro;
	}

	void draw()// 显示相关信息    
	{
		fillcircle(x, y, 6);
		settextstyle(25, 0, 0); // 设置字体大小为5号字体
		settextcolor(BLUE); // 设置文字颜色为黑色
		LPCTSTR output = (LPCTSTR)code.c_str(); // 转换数据格式
		outtextxy(x, y + 5, output); // 输出得分文字
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
	vector<Spot> spots; // 定义景点存储向量
	vector<Spot> spots_temp; // 在最短路径上的点
	vector<vector<Edge>> adjacencyList; // 邻接矩阵

	float data_1[14][2]; // 存储景点的坐标数据
	float data_2[15][3]; // 存储路径的长度数据
	string str1[15], str2[15], str3[15]; // 存储景点的名称和介绍
	wchar_t wstr[3][128]; // 存储转换后的字符串

	int isShow = 0;
	int status = 0; // 是否需要高亮绘制最短路径
	wstring info = L"最短路径："; // 最短路径信息字符串
	wstring info_2 = L"最短路径长度："; // 最短路径长度信息字符串

	// 初始化
	void initialize()
	{
		loadimage(&im_map, _T("map.jpg")); // 导入地图图片

		readRecordFile(); // 读取数据文件

		// 初始化景点
		Spot spot;

		for (int i = 0; i <= 11; i++) {
			// 将字符串转换为宽字符格式
			MultiByteToWideChar(CP_UTF8, 0, str1[i].c_str(), -1, wstr[0], 128);
			MultiByteToWideChar(CP_UTF8, 0, str2[i].c_str(), -1, wstr[1], 128);
			MultiByteToWideChar(CP_UTF8, 0, str3[i].c_str(), -1, wstr[2], 128);

			// 初始化景点对象并添加到列表中
			spot.initialize(data_1[i][0], data_1[i][1], false, wstr[0], wstr[1], wstr[2]);
			spots.push_back(spot);
		}

		adjacencyList.resize(spots.size()); // 调整邻接表的大小

		for (int i = 0; i <= 12; i++) {
			adjacencyList[data_2[i][0]].emplace_back(data_2[i][1], data_2[i][2]);
			adjacencyList[data_2[i][1]].emplace_back(data_2[i][0], data_2[i][2]);
		}
	}

	void draw() // 显示相关信息
	{
		// 显示地图
		putimage(0, 0, &im_map);

		// 绘制所有景点
		
		for (int i = 0; i < spots.size(); i++)
		{
			spots[i].draw(); // 在坐标(x,y)处画一个半径为6的圆
		}
		if (isShow == 0) {
			setlinecolor(RED);
			setlinestyle(PS_SOLID, 1);
			for (int i = 0; i < adjacencyList.size(); i++) {
				for (int j = 0; j < adjacencyList[i].size(); j++) {
					int a = i; // 记录第一个顶点的信息
					int b = adjacencyList[i][j].destination; // 记录第二个顶点的信息
					line(spots[a].x, spots[a].y, spots[b].x, spots[b].y);

					settextstyle(18, 0, 0); // 设置字体大小为5号字体
					settextcolor(RED); // 设置文字颜色为黑色
					TCHAR s[20]; // 定义字符串数组
					swprintf_s(s, _T("%d"), adjacencyList[i][j].length); // 将score转换为字符串
					outtextxy((spots[a].x + spots[b].x - 15) / 2, (spots[a].y + spots[b].y - 20) / 2, s); // 输出得分文字
				}
			}
		}
		

		if (status != 0) {
			setlinecolor(YELLOW);
			setlinestyle(PS_SOLID, 5);
			for (int i = 0; i < spots_temp.size() - 1; i++) {
				line(spots_temp[i].x, spots_temp[i].y, spots_temp[i + 1].x, spots_temp[i + 1].y);
			}
			settextcolor(BLUE); // 设置字体颜色为蓝色

			settextstyle(12, 0, _T("宋体")); // 设置字体

			LPCTSTR output = (LPCTSTR)info.c_str(); // 转换数据格式

			outtextxy(0, 0, output);

			output = (LPCTSTR)info_2.c_str(); // 转换数据格式

			outtextxy(0, 25, output);
		}

		setlinestyle(PS_SOLID, 1);

		// 绘制按钮
		button(615, 90, 170, 60, L"1.景点查询");

		settextcolor(BLACK); // 设置字体颜色为蓝色
		settextstyle(25, 0, _T("宋体")); // 设置字体
		wchar_t s[] = L"2.线路查询";
		outtextxy(635, 210, s);

		button(615, 260, 170, 50, L"Dijkstra算法");
		button(615, 340, 170, 50, L"Floyd算法");
		button(615, 450, 170, 60, L"3.退出系统");
		button(615, 450, 170, 60, L"3.退出系统");
		button(615, 520, 80, 60, L"显示");
		button(705, 520, 80, 60, L"隐藏");
	}

	void dijkstraShortestPath(int source, int destination)
	{
		status = 1; // 需要高亮绘制路径了
		int numSpots = spots.size(); // 获取景点数量
		vector<int> distance(numSpots, INT_MAX); // 初始化距离存储向量默认权值为无穷
		vector<int> previous(numSpots, -1); //
		vector<bool> visited(numSpots, false); //初始化访问辅助数组

		// 优先队列是一种基于堆的数据结构，可以支持插入、删除和查找最值的操作。
		// 优先队列的元素类型为pair<int, int>，其中第一个int表示元素的权重值，第二个int表示元素的编号。
		// 堆按照权重值从小到大的顺序进行排序，因此称为小根堆。在插入新元素时，堆会自动进行排序和调整，以保证堆的性质。
		priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

		distance[source] = 0; 
		pq.push(make_pair(0, source)); // 插入元素

		while (!pq.empty()) {
			int current = pq.top().second; // 输出当前景点
			pq.pop(); //弹出堆顶元素

			if (current == destination)
				break;

			// 判断当前节点是否访问过
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

			info = L"";
			info = L"最短路径：";
			for (int i = path.size() - 1; i >= 0; --i) {
				info += spots[path[i]].name;
				spots_temp.push_back(spots[path[i]]);

				if (i > 0)
					info += L" -> ";
			}
			info_2 = L"";
			info_2 = L"最短路径长度：";
			info_2 += wstr;
		}
	}

	void floydShortestPath(int source, int destination) {
		status = 1;
		int n = spots.size();
		vector<vector<int>> dist(n, vector<int>(n, INT_MAX)); // 初始化距离矩阵
		vector<vector<vector<int>>> path(n, vector<vector<int>>(n)); // 初始化路径矩阵
		for (int i = 0; i < n; ++i) {
			for (const Edge& edge : adjacencyList[i]) {
				int j = edge.destination;
				int w = edge.length;
				dist[i][j] = w; // 更新距离矩阵
				path[i][j].push_back(i);
				path[i][j].push_back(j); // 初始化路径矩阵
			}
		}
		for (int k = 0; k < n; ++k) {
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < n; ++j) {
					if (dist[i][k] < INT_MAX && dist[k][j] < INT_MAX && dist[i][j] > dist[i][k] + dist[k][j]) {
						dist[i][j] = dist[i][k] + dist[k][j]; // 更新距离矩阵
						path[i][j].clear(); // 清空路径矩阵
						path[i][j].insert(path[i][j].end(), path[i][k].begin(), path[i][k].end());
						path[i][j].insert(path[i][j].end(), path[k][j].begin() + 1, path[k][j].end()); // 更新路径矩阵
					}
				}
			}
		}
		// 输出最短路径矩阵和路径
		if (dist[source][destination] == INT_MAX) {
			MessageBox(GetHWnd(), L"无法从起点到达目的地", L"提示", MB_ICONERROR | MB_OK);
		}
		else if (path[source][destination].size() > 0) {
			info = L"";
			info = L"最短路径：";
			wostringstream oss;
			oss << dist[source][destination];
			wstring wstr = oss.str();
			info += spots[path[source][destination][0]].name;
			spots_temp.push_back(spots[path[source][destination][0]]);
			for (int k = 1; k < path[source][destination].size(); ++k) {
				info += L" -> ";
				info += spots[path[source][destination][k]].name;
				spots_temp.push_back(spots[path[source][destination][k]]);
			}
			info_2 = L"";
			info_2 = L"最短路径长度：";
			info_2 += wstr;
			// spots_temp.resize(path[source][destination].size());
		}
	}

	Spot clickSpot(float x, float y)
	{
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
		for (int i = 0; i <= 11; i++) {
			ifs >> data_1[i][0] >> data_1[i][1] >> str1[i] >> str2[i] >> str3[i];
		}
		for (int i = 0; i <= 12; i++) {
			ifs >> data_2[i][0] >> data_2[i][1] >> data_2[i][2];
		}
		// 关闭文件
		ifs.close();
	}
};


// 一些全局变量

Map map; // 定义地图全局对象
// int isShow = 0;

void startup() // 初始化
{
	map.initialize(); // 地图初始化
	initgraph(WIDTH, HEIGHT); // 新开一个画面
	setbkcolor(RGB(241, 236, 243)); // 设置背景颜色
	BeginBatchDraw(); // 开始批量绘制
}

void show() // 显示
{
	cleardevice();  // 以背景颜色清空画布
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
			if (msg.x >= 615 && msg.x <= 615 + 170 && msg.y >= 90 && msg.y <= 90 + 60)
			{
				map.status = 0;
				map.spots_temp.clear();
				// 使用MessageBox显示信息框
				MessageBox(GetHWnd(), L"请直接在地图上点击相应的景点", L"景点查询", MB_ICONINFORMATION | MB_OK);
			}
			if (msg.x >= 615 && msg.x <= 615 + 170 && msg.y >= 260 && msg.y <= 260 + 50)
			{
				map.status = 0;
				map.spots_temp.clear();
				wchar_t input1[20];
				wchar_t input2[20];

				InputBox(input1, 60, L"请输入起点", L"起点", L"", 480, 0, false);
				InputBox(input2, 400, L"请输入终点", L"终点", L"", 480, 0, false);
				int destination = -1, source = -1;

				for (int i = 0; i <= 11; i++) {
					MultiByteToWideChar(CP_UTF8, 0, map.str1[i].c_str(), -1, map.wstr[0], 128);
					if (wcscmp(input1, map.wstr[0]) == 0) {
						source = i;
						break;
					}
				}

				for (int i = 0; i <= 11; i++) {
					MultiByteToWideChar(CP_UTF8, 0, map.str1[i].c_str(), -1, map.wstr[0], 128);
					if (wcscmp(input2, map.wstr[0]) == 0) {
						destination = i;
						break;
					}
				}

				if (source == -1 || destination == -1) {
					MessageBox(GetHWnd(), L"请输入正确的景点名称", L"出错了！", MB_ICONINFORMATION | MB_OK);
				}
				else {
					map.dijkstraShortestPath(source, destination);
					//map.floydShortestPath(source, destination);
				}
			}
			if (msg.x >= 615 && msg.x <= 615 + 170 && msg.y >= 340 && msg.y <= 340 + 50)
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
					MessageBox(GetHWnd(), L"请输入正确的景点名称", L"出错了！", MB_ICONINFORMATION | MB_OK);
				}
				else {
					map.floydShortestPath(source, destination);
				}
			}
			if (msg.x >= 625 && msg.x <= 625 + 150 && msg.y >= 450 && msg.y <= 450 + 60)
			{
				exit(0);
			}
			//button(615, 520, 80, 60, L"显示");
			//button(705, 520, 80, 60, L"隐藏");
			if (msg.x >= 615 && msg.x <= 615 + 80 && msg.y >= 520 && msg.y <= 520 + 60)
			{
				map.isShow = 0;
			}
			if (msg.x >= 705 && msg.x <= 705 + 80 && msg.y >= 520 && msg.y <= 520 + 60)
			{
				map.isShow = 1;
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