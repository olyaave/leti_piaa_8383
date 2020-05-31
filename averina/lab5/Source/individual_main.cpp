#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;
void inputConsole(string *word, int *n ,vector<string> *templates);

class TNode{
private:
    char symbol;
    unordered_map<char, TNode*> sons;
    TNode* parent = nullptr;
    TNode* suffLink = nullptr;
    string str = "";
    int terminated;

public:
    explicit TNode(char c): symbol(c), terminated(0){}

    void insert(string temp, int numPattern){
        TNode* curr = this;

        for (char symbol: temp) {
            if (curr->sons[symbol] == nullptr) {
                curr->sons[symbol] = new TNode(symbol);  // создаем нового ребенка
                curr->sons[symbol]->parent = curr;
                curr->sons[symbol]->str = curr->str + symbol;
            }
            curr = curr->sons[symbol];
        }
        cout << "Insert pattern: " << temp << endl;
        curr->terminated = numPattern + 1;
    }


    //  поиск символа префикса в боре и всех вхождений шаблонов в его путь
    vector<int> getChain(char c, int *maxSuffLen, int *maxEndLen){
        vector<int> templatesInside;
        int currSuffLen = 0;
        int currEndLen = 0;
        static const TNode* curr = this;

        for (; curr != nullptr ; curr = curr->suffLink) {
            for (auto son: curr->sons) {
                cout << "Child: " << son.first << endl;
                if(son.first == c) {
                    cout << "This is the child they were looking for! \n";
                    curr = son.second;
                    for (auto node = curr; node->suffLink != nullptr; node = node->suffLink, currSuffLen++)
                        if(node->terminated > 0) {
                            currEndLen++;
                            cout << "A terminal vertex '"<<node->symbol <<"' was found for the template " << node->terminated << endl;
                            templatesInside.push_back(node->terminated);
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
        cout << "The end of the template for this position was not found.\n";
        return templatesInside;
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
            for(pair<const char, TNode *> son: curr->sons) {
                q.push(son.second);
            }
            q.pop();

            TNode* par = curr->parent;
            if(par != nullptr)   // переходим по суфф. ссылке предыдущей вершины
                par = par->suffLink;


            while(par && par->sons.find(curr->symbol) == par->sons.end()) //проверка, есть ли нужный символ
            {

                par = par->suffLink;                             // в потомках рассматриваемой вершины,
            }                                                        // если нет, то переходим по суфф ссылке

            if(par)  {curr->suffLink = par->sons[curr->symbol]; cout << " Suffix link: " << curr->suffLink->symbol << " Substring: " << curr->suffLink->str << endl;}
                //  присваиваем суффиксную ссылку, если она найдена
            else curr->suffLink = this;      //  иначе присваиваем ссылку в себя
            cout << endl;
        }

    }


    void printTrie(TNode* root){
        TNode* curr = root;

        cout <<"\nString: " << curr->str << endl;
        if(curr->terminated > 0)
            cout <<"--->Terminated!" << "\n";

        if(curr->parent) {
            if (curr->parent->symbol != '\0') {
                cout << "    Symbol: " << curr->symbol << "    ";
                cout << "    Parent: " << curr->parent->symbol << endl;
            }
            if (curr->parent->symbol == '\0') {
                cout << "    Symbol: " << "'\\0' ";
                cout << "    Parent: root" << endl;
            }
        }
        else
            cout << "    Root" << endl;
        if(curr->suffLink)
            cout << "    Suffix link: " << curr->suffLink->str << endl;

        cout << "    Children: ";
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
    Trie(): node('\0'), maxSuffLen(0), maxEndLen(0) {}

    void printMaxLenghts(){
        cout << "\nMax suffix link chain lenght: " << maxSuffLen << endl;
        cout << "Max end link chain lenght: " << maxEndLen << endl;
    }
    TNode* getRoot(){
        return &node;
    }
    vector<int> getChain(char c){
        return node.getChain(c, &maxSuffLen, &maxEndLen);
    }
    void makeSuffixLinks(){
        node.makeSuffixLinks();
    }
    void insert(string temp, int numPattern){
        node.insert(temp, numPattern);
    }
};


int main() {
    string str;
    int n;
    map<int, vector<int>> res;
    vector<string> templates(10);
//    inputConsole(&str, &n, &templates);
//------------------------------------------
    cin >> str >> n;
    templates.resize(n);
    for (int i = 0; i < n; ++i) { cin >> templates[i]; }
    //-----------------------------------------

    Trie root;
    // построение бора
    cout << "\nStarted the construction of the Trie... \n";
    for (int j = 0; j < n; ++j) {
        root.insert(templates[j], j);
    }
    cout << "-------------------------- Suffix links -----------------------------\n";
    cout << "\nThe process of creating suffix links...\n";
    root.makeSuffixLinks();
    cout << "-------------------------Built the trie -----------------------------\n";
    root.getRoot()->printTrie(root.getRoot());

    cout << "------------------------ Substring search -----------------------------\n";
    for (int i = 0; i < str.length(); ++i) {
        cout << "\nSymbol: " << str[i] << "  Index: " << i << endl;
        vector<int> tmp = root.getChain(str[i]);
        for (auto index: tmp) {
            res[i - templates[index - 1].size() + 2].push_back(index);
            sort(res[i - templates[index - 1].size() + 2].begin(),
                 res[i - templates[index - 1].size() + 2].end());
        }
    }
    root.printMaxLenghts();

    cout << "Index Pattern\n";
    for (auto it: res) {
        for (auto k: it.second) {
            cout << "" << it.first << "     " <<  k << endl;

        }
    }
    return 0;
}

void inputConsole(string *word,int *n ,vector<string> *templates){
    ifstream file;
    file.open("input.txt");
    if (file.is_open()) {
        file >> *word >> *n;
        templates->resize(*n);
        for (int i = 0; i < *n; ++i) {
            file >> (*templates)[i];
        }
        file.close();
    } else {
        cout << "File isn't open!";
    }
}