/**************************************************************************************
实验题目1：通讯网络的连通性问题
[实验内容及要求]
构建连通n个城市的通讯网络。
(1) 输入：城市个数n（城市编号为1到n）	//但是我都用的(城市编号为0到n-1)这样简单
，城市间通讯线路条数m，随机生成m个不同的城市对（a，b），表示城市间的通讯线路。
依次对这m条通讯线路判断其对于构建连通的通讯网络是否为冗余的，即加入通讯线路（p，q）时，
如果城市p与城市q已经连通，则这条通讯线路就是冗余的，将其剔除，否则将其加入通讯网。
输出：图形展示连通的通讯网络的构建结果及冗余线路信息，并用文件将这些信息存储起来。
(2) 为提高通讯网络的可靠性，需要保证如果任何一段通讯线路出现故障，不影响整个通讯网络的连通性。
针对（1）中输入，判断该网络是否为可靠网络，并将网络图形展示并文件保存。
(3) 建立城市间的通讯网络，每个城市都有通讯交换机，直接或间接与其它城市连接。交换机有可能会发生故障，需要配备备用交换机。
但备用交换机数量有限，不能全部配备，只能给部分重要城市配置。于是规定：如果某个城市由于交换机损坏，不仅本城市通讯中断，还造成其它城市间通讯中断，
则为其配备备用交换机。分别针对前面(1)和(2)两种情形建立的通讯网络，标记出需配备备用交换机的城市。

(1)     随机生成m个不同的城市对（a，b）,也就是说不允许平行边, 不允许自环
        加入边的同时判断是否冗余
        图形展示 构建结果和冗余路线信息    输出到文件中


(2)     对每一条边, 进行一次删除后判断原本与他连通的城市是否依旧连通
	若有任一次出现不连通, 则不可靠

(3)     叶节点不重要, 环中的任一城市都不重要
	单独一个节点为一个连通分量的城市不重要
	如果删除一个节点后, 图中连通分量数增加, 则此城市为重要城市

	文件构筑信息和冗余路线信息将输出到addfalse.txt
        图形展示信息将输出到1.dot
        图可靠性输出到is_reliable.txt
        第(3)问的重要城市信息以绿色标识在3.dot中

	本程序使用Boost.graph类库, 使用graphviz绘图
****************************************************************************************/

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/properties.hpp>

using namespace std;
using namespace boost;

// 定义图类型，使用set存放边, vector存放顶点，无向图, 使用std::set可以保证没有平行边
// adjacency_list是邻接表, vecS是vector, undirectedS代表无向
typedef adjacency_list<setS, vecS, undirectedS> graph_t;

// 顶点类型, vertex_descriptor
typedef graph_traits<graph_t>::vertex_descriptor vertex_descriptor;

// 顶点迭代器类型, vertex_iter
typedef graph_traits<graph_t>::vertex_iterator vertex_iter;

// 边迭代器类型, edge_iter
typedef graph_traits<graph_t>::edge_iterator edge_iter;

int random(int start, int end)
{
        // 随机生成一个在start和end之间的数
        return start + (end - start) * rand() / (RAND_MAX + 1.0);
}

bool is_important_city(graph_t g, int num, vector<int> component, int i)
{
	// 第(3)问, 测试某个城市是否为重要城市
	// 参数分别为: 图g(这样传进来的是副本, 改他没事), 原图连通分量数num, 连通分量所在位置数组, 要测试的第i个点
	vertex_descriptor u = vertex(i, g);
	// 如果一个点的度是0, 他直接就不是重要城市了
	if (degree(u, g) == 0)
		return false;
	// 删除与点u连着的所有边, 再删除点u, 否则会出错
	clear_vertex(u, g);
	remove_vertex(u, g);
	int new_num = connected_components(g, &component[0]);
	return new_num != num;
}

bool is_reliable(graph_t g, int num, vector<int> component, int i)
{
	// 第(2)问, 测试图删除第i条边是否仍为可靠的
	// 参数分别为: 图g(这样传进来的是副本, 改他没事), 原图连通分量数num, 连通分量所在位置数组, 要测试的第i条边
	std::pair<edge_iter, edge_iter> erange = edges(g);
	edge_iter itr = erange.first;
	for (int j = 0; j < i; j++)
		itr++;
	remove_edge(*itr, g);
	int new_num = connected_components(g, &component[0]);
	return new_num != num;
}

int main(int argc, char* argv[])
{
        cout << "输入城市个数n:  ";
        int n;
        cin >> n;

        // 产生图对象，已有n个顶点, 0到n-1
        graph_t g(n);

        cout << endl << "输入城市间通讯线路条数m:  ";
        int m;
        cin >> m;


        // 加入边
        // 随机生成m个不同的城市对（a，b）,也就是说不允许并行边
        ofstream add_false;
        add_false.open("addfalse.txt", ios::trunc);
        srand(unsigned(time(0)));
        add_false << "冗余路线信息:" << endl << endl;
        for (int i = 0; i < m; i++)
        {
                int rand_a = random(0, n);
                int rand_b = random(0, n);
                // rand_a和rand_b不允许相同, 即不允许自环
                while (rand_b == rand_a)
                {
                	add_false << "冗余边: " << rand_a << "---" << rand_b << endl << endl;
                        rand_b = random(0, n);
                }
                // add_edge：在一个图的两个顶点之间添加一条边
                // 因为使用set存储边, 不允许平行边, 所以加入平行边时会失败
                if( (add_edge(rand_a, rand_b, g)).second == false )
		{
			add_false << "冗余边: " << rand_a << "---" << rand_b << endl << endl;
			i--;
		}
        }

        add_false << "通讯网络的构建结果: " << endl;
	add_false << "边的数量: " << boost::num_edges(g) << endl;// 边的数量
	add_false << "顶点的数量: " << boost::num_vertices(g) << endl;// 顶点的数量

        // 输出所有的顶点到文件
        // 得到所有顶点，vrange中的一对迭代器分别指向第一个顶点和最后的一个顶点之后。
        // std::pair主要的作用是将两个数据组合成一个数据，两个数据可以是同一类型或者不同类型
        std::pair<vertex_iter, vertex_iter> vrange = vertices(g);
	//int jishujun = 0;	//计数君用来记住第几个点的
        for (vertex_iter itr = vrange.first; itr != vrange.second; ++itr)//, jishujun++)
	{
		add_false << *itr << endl;
//		cout << "第" << jishujun << "个点的度:" << degree(*itr, g) << endl;
	}



        // 得到所有边，erange中的一对迭代器分别指向第一条边和最后的一条边之后
        std::pair<edge_iter, edge_iter> erange = edges(g);
        // 输出所有的边到文件
        for (edge_iter itr = erange.first; itr != erange.second; ++itr)
                add_false << source(*itr, g) << "---" << target(*itr, g) << endl;
	cout << "第(1)问的文件构筑信息和冗余路线信息已输出到addfalse.txt" << endl << endl;

	// 将图输出到文件"1.dot"中
	// .dot是graphviz的脚本文件,用gvedit.exe打开它,图形就有了
	fstream graphviz_1;
	graphviz_1.open("1.dot", ios::out);
	write_graphviz(graphviz_1, g);
	cout << "第(1)问的图形展示信息已输出到1.dot" << endl << endl;


	// num是连通分支数量, component[i]的值是第i个点在第几个连通分量中
	vector<int> component(num_vertices(g));
	int num = connected_components(g, &component[0]);
	cout << "连通分支数: " << num << endl << endl;


	// 第(2)问, 判断这个图是否可靠
	ofstream is_reliable_txt;
	is_reliable_txt.open("is_reliable.txt", ios::trunc);
	bool ir = false;
	for (int i = 0; i < m; i++)
	{
		ir = is_reliable(g, num, component, i);
		if (ir)
		{
			ir = true;
			cout << "此图不可靠, " << endl << "删除第" << i << "条边就会影响连通性" << endl;
			is_reliable_txt << "此图不可靠, " << endl << "删除第" << i << "条边就会影响连通性" << endl;
			break;
		}
	}
	if (ir == false)
	{
		cout << "此图可靠." << endl;
		is_reliable_txt << "此图可靠." << endl;
	}


	// 第(3)问, 判断各个城市是否为重要城市
	// 同时将图形展示信息输出到"3.dot"中, 同样用gvedit.exe打开就行了
	ofstream three;
	three.open("3.dot", ios::trunc);
	three << "graph G {" << endl;
	for (int i = 0; i < n; i++)
	{
		bool iic = is_important_city(g, num, component, i);
		if (iic)
			three << i << "[style=\"filled\", color=\"black\", fillcolor=\"chartreuse\"];" << endl;
		else
			three << i << ";" << endl;
	}
	for (edge_iter itr = erange.first; itr != erange.second; ++itr)
		three << source(*itr, g) << "--" << target(*itr, g) << ";" << endl;
	three << "}" << endl;
	cout << "第(3)问图形展示信息已输出到3.dot中" << endl << "绿色标记的就是需要配备交换机的重要城市" << endl;

        return 0;
};

