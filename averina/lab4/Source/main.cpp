#include <iostream>
#include <vector>
#include <fstream>


using namespace std;

//Вар. 2. Оптимизация по памяти: программа должна требовать O(m) памяти, где m - длина образца.
// Это возможно, если не учитвать память, в которой хранится строка поиска.

// Для изменения алгоритма нужно раскомментировать функцию KMP или offset в main

// Считывание из файла
void inputFile(string *S, string *T)
{
    ifstream file;
//    file.open("/home/olyaave/CLionProjects/PAA_LAB4/input.txt");
    file.open("input.txt");
    if (file.is_open()) {
        file >> *T  >> *S;
        file.close();
    } else {
        cout << "File isn't open!";
    }
}

void prefix(string *T, vector<int> *max_pref){

    size_t n = T->length();

    int prev_pref = (*max_pref)[0] = 0;

    for (int i = 1; i < n; ++i)
    {
        while(prev_pref > 0 && (*T)[prev_pref] != (*T)[i])  // Поиск максимального префикса-суффикса
            prev_pref = (*max_pref)[prev_pref - 1];

        if((*T)[prev_pref] == (*T)[i])  // Если символы совпадают, расширяем префикс
            prev_pref++;

        (*max_pref)[i] = prev_pref;  // Сохраняем значение префикс-функции для текущего префикса
    }

}

// Функция для поиска подстроки
void KMP(string* S, string *T, vector<int> *answer)  // S - строка, T - шаблон
{
    cout << "\nGetting started with the substring occurrence search algorithm.\n";
    size_t n = T->length();
    vector<int> max_pref(n);

    prefix(T, &max_pref);

    int prev_pref = 0;
    for (int i = 0; i < S->length(); ++i)
    {
        while(prev_pref > 0 && (*T)[prev_pref] != (*S)[i])  // Поиск максимального префикса-суффикса
        {
            prev_pref = max_pref[prev_pref - 1];
        }
        cout << "\nT[" << prev_pref << "] = " << (*T)[prev_pref] << endl;
        cout << "S[" << i << "] = " << (*S)[i] << "\n";

        if((*T)[prev_pref] == (*S)[i]) {    // Если символы совпадают, расширяем префикс
            prev_pref++;
            cout << "The symbols are the same.\n";
        }
        if(prev_pref == n) {    // Если найден шаблон, сохраняем в векторе
            answer->push_back(i - n + 1 );
            cout << "Pattern detected, index " << i - n + 1 <<"\n";
        }
    }
    if(answer->empty()) {
        cout << "\nNo occurrences of pattern.\n";
        cout << "-1";
    }
    else {
        cout << "\nAnswer: ";
        for (int i = 0; i < answer->size() - 1; ++i) {
            cout << (*answer)[i] << ",";
        }
        cout << (*answer)[answer->size() - 1] << endl;
    }
}


int offset(string* B, string *A) // ищем B в А
{
    cout << "\nGetting started with the cyclic string shift search algorithm.\n";

    if((*A).size() != (*B).size())
        return -1;

    int sizeA = A->length();
    int sizeB = B->length();

    vector<int> max_pref(sizeB);

    int prev_pref = 0;
    int start_i = -1;

    for (int i = 0; i < sizeA; ++i)
    {
        while(prev_pref > 0 && (*B)[prev_pref] != (*A)[i])  // Поиск максимального префикса-суффикса
            prev_pref = max_pref[prev_pref - 1];

        cout << "\nB[" << prev_pref << "] = " << (*B)[prev_pref] << endl;
        cout << "A[" << i << "] = " << (*A)[i] << "\n";

        if((*B)[prev_pref] == (*A)[i])  // Если символы совпадают, расширяем префикс
        {
            prev_pref++;
            cout << "The symbols are the same.\n";
        }
        if(i == sizeA- 1 && prev_pref > 0) {  // Если встречен крайний префикс,
                                              // сохраняем индекс, с которого возможно начался циклический сдвиг
            start_i = sizeA - prev_pref;
            cout << "Possible occurrence Index: " << start_i << "\n";
        }
        if(prev_pref == sizeB)   // Если значение префикс-функции равно длине исходной строки
        {                        // до конца проверяемой, значит исходная совпадает с проверяемой
            cout << "\nThe string being checked matches the original one.\n";
            return 0;
        }
    }


    for (int i = 0; i < start_i; ++i)
    {
        while(prev_pref > 0 && (*B)[prev_pref] != (*A)[i])
            prev_pref = max_pref[prev_pref - 1];

        cout << "\nB[" << prev_pref << "] = " << (*B)[prev_pref] << endl;
        cout << "A[" << i << "] = " << (*A)[i] << "\n";

        if((*B)[prev_pref] == (*A)[i]) {
            prev_pref++;
            cout << "The symbols are the same.\n";
        }

        if(prev_pref == B->size()) {
            cout << "\nIt is cycle shift.\n"
                    "Answer: ";
            return start_i;
        }
    }
    cout << "It isn't cycle shift.\n";
    return -1;
}


int main() {
    string S, T;
    vector<int> answer;

//    inputFile(&S, &T);
    int type;
    cin >> type >>T >> S;
    if(type == 1)
        KMP(&S, &T, &answer);
    else
        cout << offset(&S, &T);


    return 0;
}


