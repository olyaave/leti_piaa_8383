#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;
char inputConsole(string *str, string *templates);

class TNode{
private:
    char symbol;
    unordered_map<char, TNode*> sons;
    TNode* parent = nullptr;
    TNode* suffLink = nullptr;
    string str = "";
    int terminated;
    vector<pair<int, int>> arrayPatterns;

public:
    explicit TNode(char c): symbol(c), terminated(0){}

    void insert(const string &temp, int pos, int size) {
        TNode* curr = this;
        for (char symbol: temp) {
            if (curr->sons[symbol] == nullptr) {
                curr->sons[symbol] = new TNode(symbol);  // создаем нового ребенка
                curr->sons[symbol]->parent = curr;
                curr->sons[symbol]->str = curr->str + symbol;
            }
            curr = curr->sons[symbol];
        }
        cout << "Insert substring: " << temp << endl;
        curr->terminated = 1;
        curr->arrayPatterns.emplace_back(pos, size); // сохраняем все вставленные маски в вектор
    }                                                // pos - позиция начала в шаблоне
                                                   // size - длина маски


    //  поиск символа префикса в боре и всех вхождений шаблонов в его путь
    vector<pair<int, int>> getChain(char c, int *maxSuffLen, int *maxEndLen) {
        vector <pair<int, int>> templatesInside;
        int currSuffLen = 0;
        int currEndLen = 0;
        static const TNode* curr = this;

        for (; curr != nullptr ; curr = curr->suffLink) {
            for (auto son: curr->sons) {
                cout << "Child: " << son.first << endl;
                if(son.first == c) {
                    cout << "This is the child they were looking for! \n";
                    curr = son.second;
                    for (auto node = curr; node->suffLink != nullptr; node = node->suffLink, currSuffLen++) {
                        if(node->terminated > 0)
                            currEndLen++;
                        for (auto it: node->arrayPatterns) {
                            templatesInside.push_back(it);
                        }
                    }
                    *maxSuffLen = (*maxSuffLen < currSuffLen) ? currSuffLen : *maxSuffLen;
                    *maxEndLen = (*maxEndLen < currEndLen) ? currEndLen : *maxEndLen;
                    cout << "Current max suffix link chain lenght: " << *maxSuffLen << endl;
                    cout << "Current max end link chain lenght: " << *maxEndLen << endl;
                    return templatesInside;
                }
            }
        }
        curr = this;
        return {};
    }


    // функция для построения суффиксных ссылок
    void makeSuffixLinks(){

        queue<TNode*> q;
        for (auto son: sons) {     // можно внести это в цикл
            q.push(son.second);
        }
        while(!q.empty()) {
            TNode* curr = q.front();  //  берем вершину из очереди для обработки
            cout << "Considered vertex: " << curr->symbol << " Substring: " << curr->str << endl;
            for(pair<const char, TNode *> son: curr->sons)
                q.push(son.second);
            q.pop();

            TNode* par = curr->parent;
            if(par != nullptr)   // переходим по суфф. ссылке предыдущей вершины
                par = par->suffLink;


            while(par && par->sons.find(curr->symbol) == par->sons.end()) //проверка, есть ли нужный символ
                par = par->suffLink;                             // в потомках рассматриваемой вершины,
                                                                  // если нет, то переходим по суфф ссылке
            if(par)  {curr->suffLink = par->sons[curr->symbol]; cout << " Suffix link: " << curr->suffLink->symbol << " Substring: " << curr->suffLink->str << endl;}
                                                            //  присваиваем суффиксную ссылку, если она найдена
            else curr->suffLink = this;      //  иначе присваиваем ссылку в себя
        }

    }

    void printTrie(TNode* root){
        TNode* curr = root;
        cout <<"\nString:" << curr->str << endl;
        if(curr->terminated > 0)
            cout <<"--->Terminated!" << "\n";

        if(curr->parent && curr->parent->symbol != '\0') {
            cout <<"    Symbol:" <<curr->symbol << "\n";
            cout <<"    Parent:" << curr->parent->symbol << endl;
        }
        else if(curr->parent && curr->parent->symbol == '\0')
            cout <<"    Parent: root" << endl;
        else
            cout <<"    Root" << endl;

        if(curr->suffLink)
            cout << "    Suffix link: " << curr->suffLink->str << endl;

        cout << "    Children:";
        if(curr->sons.size() > 0) {
            for (auto c:curr->sons) {
                cout << c.first << " ";
            }
            cout << endl;
        }
        else cout << " none \n";
        for(auto tmp:curr->sons) {
            if (tmp.second) {
                printTrie(tmp.second);
            }
        }
    }
};

class Trie{
private:
    TNode node;
    int maxSuffLen;
    int maxEndLen;
public:
    Trie(): node('\0'), maxSuffLen(0), maxEndLen(0){}

    void printMaxLenghts(){
        cout << "\nMax suffix link chain lenght: " << maxSuffLen << endl;
        cout << "Max end link chain lenght: " << maxEndLen << endl;
    }

    TNode* getRoot(){ return &node; }

    vector<pair<int, int>> getChain(char c){ return node.getChain(c, &maxSuffLen, &maxEndLen); }

    void makeSuffixLinks(){  node.makeSuffixLinks(); }

    void insert(const string &temp, int pos, int size){ node.insert(temp, pos, size); }
};

int main() {
    string str;
    char joker;
    string pattern;
//    joker = inputConsole(&str, &pattern);
//------------------------------------------
    cin >> str >> pattern >> joker;
    //-----------------------------------------
    int freePart = 0;  // счетчик подстрок в шаблоне без джокера
    Trie root;
    string mask;
    vector <size_t> maskEnter(str.size());  // массив флагов попадания подстроки в текст
    char c;
    cout << "\nStarted the construction of the Trie... \n";
    // построение бора
    for (size_t i = 0; i <= pattern.size(); i++)
    {
        if(i == pattern.size())  // если встречен конец строки, присваиваем с значение джокера,
                                    // чтобы вставить оставшуюся маску в бор
            c = joker;
        else c = pattern[i];

        if (c != joker) {       // накапливаем маску без джокеров
            mask += c;
        }
        else if (!mask.empty()) { // если встречен джокер и подстрока непустая, добавляем маску в бор
            freePart++;
            root.insert(mask, i - mask.size(), mask.size());
            mask.clear();
        }
    }
    cout << "-------------------------- Suffix links -----------------------------\n";
    cout << "\nThe process of creating suffix links...\n";
    root.makeSuffixLinks();

    cout << "-------------------------Built the trie -----------------------------\n";
    root.getRoot()->printTrie(root.getRoot());
    cout << "------------------------ Substring search -----------------------------\n";
    for (int j = 0; j < str.size(); j++) {
        cout << "\nSymbol: " << str[j] << "  Index: " << j << endl;
        vector<pair<int, int>> tmp = root.getChain(str[j]);
        for (auto pos: tmp) {                       // добавление всех вхождений масок
            int i = j - pos.first - pos.second + 1;
            if (i >= 0 && i + pattern.size() <= str.size())  // отмечаем вхождение, если границы шаблона
                maskEnter[i]++;                              // входят в границы текста
        }
    }

    root.printMaxLenghts();

    cout << "Index Pattern\n";
    for (int i = 0; i < maskEnter.size(); i++)
        if (maskEnter[i] == freePart){
            cout << i+1 << endl;     // печать индексов вхождения шаблона
        }
    return 0;
}

char inputConsole(string *str, string *templates) {
    ifstream file;
    char joker;
    file.open("input.txt");
    if (file.is_open()) {
        file >> *str >> templates[0] >> joker;
        file.close();
    } else {
        cout << "File isn't open!";
    }
    return joker;
}