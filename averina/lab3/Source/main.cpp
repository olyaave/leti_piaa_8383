#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <fstream>
#include <algorithm>

// Вар. 5. Поиск не в глубину и не в ширину, а по правилу: каждый раз выполняется переход по дуге,
// имеющей максимальную остаточную пропускную способность. Если таких дуг несколько, то выбрать ту,
// которая была обнаружена раньше в текущем поиске пути.

using namespace std;

class flowSearch {

    struct point {
        char vertex;
        int way;
    };
    struct edge {
        char from;
        char to;
        int flow;
    };

    unordered_map<char, vector<point>>  network;  // остаточная сеть
    unordered_map<char, vector<point>> invertNetwork;

    char start, finish;

    int searchPath(vector<pair<char, bool>> *way){

        char currentPoint = start, newPoint = ' ';
        int maxFlow, minResFlow = 10000;
        bool isInvertEdge = false;

        unordered_map<char, bool> checked;
        checked[start] = true;

        while(currentPoint != finish){

            std::cout <<"Vertex: " <<currentPoint << "\n";
            maxFlow = -1;
            for (auto it : network[currentPoint]) {
                if (!checked[it.vertex] && it.way > 0 && maxFlow < it.way) {
//                if (!checked[it.vertex] && it.way > 0) {
                    newPoint = it.vertex;
                    maxFlow = it.way;
                    isInvertEdge = false;
                }
            }
            for (auto it : invertNetwork[currentPoint]) {
                if (!checked[it.vertex] && it.way > 0 && maxFlow < it.way) {
//                if (!checked[it.vertex] && it.way > 0) {
                    newPoint = it.vertex;
                    maxFlow = it.way;
                    isInvertEdge = true;
                }
            }
            currentPoint = newPoint;
            checked[currentPoint] = true;

            if (maxFlow == -1) {
                way->pop_back();
                if(way->empty())  return 0;

                currentPoint = way->back().first;
                continue;
            }

            way->push_back({currentPoint, isInvertEdge});

            if (maxFlow < minResFlow)
                minResFlow = maxFlow;
        }
        return minResFlow;
    }

    static int comp(edge x, edge y) {
        if(x.from <= y.from)
            return x.to < y.to;
        else
            return x.from < y.from;
    }

    void sortOutput(int flowValue) {

        cout << flowValue << endl;

        vector<edge> answer;
        for (auto u: invertNetwork)
        {
            for (auto it: invertNetwork[u.first]) {
                answer.push_back({it.vertex, u.first, it.way});
            }
        }

        sort(answer.begin(), answer.end(), comp);
        for (auto it: answer) {
            cout << it.from << " " << it.to << " " << it.flow << endl;
        }
    }


public:

    explicit flowSearch(): start(' '), finish(' ') {}

    void inputGraphConsole() {
        char from, to;
        int way_, n;
        cin >> n >> start >> finish;

        for (int i = 0; i < n; ++i) {
            cin >> from >> to >> way_; // from - из, to - в

            network[from].push_back({to, way_});
            invertNetwork[to].push_back({from, 0});
        }
    }

    void networks() {

        int minResFlow;
        int flowValue = 0;
        vector<pair<char, bool>> way;   // найденный временный путь(его вершины)

        way.push_back({start, false});

        while (!way.empty()) {

            minResFlow = searchPath(&way);  // поиск потока
            if(way.size() > 1) {

                cout << "\nFlow is finding: \n";
                for (auto it : way) {
                    cout << it.first << " ";
                }

                vector<point> *list;
                vector<point> *invertList;
                for (auto u = way.begin(); u != (way.end() - 1); ++u)//  вершина u - "из"
                {
                    cout << "\n\nFrom " << u->first << " to " << (u + 1)->first << endl;
                    if (!(u + 1)->second) {
                        list = &network[u->first];
                        invertList = &invertNetwork[(u + 1)->first];
                    } else {
                        list = &invertNetwork[u->first];
                        invertList = &network[(u + 1)->first];
                    }
                    for (auto &it: *list) {
                        if (it.vertex == (u + 1)->first) {
                            cout <<u->first<< " - "<<(u + 1)->first<<": "<<it.way<<" - "<<minResFlow<<" = ";
                            it.way -= minResFlow;
                            cout << it.way << endl;
                        }
                    }
                    for (auto &it: *invertList) {
                        if (it.vertex == u->first) {
                            cout <<(u + 1)->first<<" - "<<u->first<<": "<<it.way<<" + "<<minResFlow<<" = ";
                            it.way += minResFlow;
                            cout << it.way << endl;
                        }
                    }
                }
                way.clear();
                way.push_back({start, 0});
                cout << "\n";
                flowValue += minResFlow;
            } else
                way.clear();
        }
        sortOutput(flowValue);
    }

};

int main() {
    cout << "Program start:\n\n";
    flowSearch *obj = new flowSearch();
    obj->inputGraphConsole();
    obj->networks();
    return 0;
}
