// VLSICAD_Ass1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <algorithm>

using namespace std;

struct CubeList {
    int numVars;
    int numCubes;
    vector<vector<int>> F;
};

void readFile(CubeList* cL, string fileName);
int variableSelection(CubeList pcn);
CubeList posCofactor(CubeList pcn, int var);
CubeList negCofactor(CubeList pcn, int var);

//find indices of max elements in a vector
vector<int> max_element(vector<int>* list)
{
    //allocate a vector of size 1 for now
    vector<int> largest(1,0);

    //if list/array/vector is empty just return the last iterator
    if (list->size() == 0) return largest;
    if (list->size() == 1) {
        largest[0] = 0;
        return largest;
    }
    

    //start with the first element being largest
    largest[0] = 0;

    for (int i = 1; i < list->size(); ++i) {
        //if a larger value is found, clear the vector then add element
        if (list->at(largest[0]) < list->at(i)) {
            largest.clear();
            largest.push_back(i);
        }
        //if value is the same, add iterator to vector
        //this is the only case where there should be more than one element in vector
        else if (list->at(largest[0]) == list->at(i)) {
            largest.push_back(i);
        }   
    }     

    return largest;
}

//find indices of max elements in a vector
vector<int> min_element(vector<int>* list)
{
    //allocate a vector of size 1 for now
    vector<int> largest(1, 0);

    //if list/array/vector is empty just return the last iterator
    if (list->size() == 0) return largest;
    if (list->size() == 1) {
        largest[0] = 0;
        return largest;
    }


    //start with the first element being largest
    largest[0] = 0;

    for (int i = 1; i < list->size(); ++i) {
        //if a larger value is found, clear the vector then add element
        if (list->at(largest[0]) > list->at(i)) {
            largest.clear();
            largest.push_back(i);
        }
        //if value is the same, add iterator to vector
        //this is the only case where there should be more than one element in vector
        else if (list->at(largest[0]) == list->at(i)) {
            largest.push_back(i);
        }
    }

    return largest;
}

int main()
{
    CubeList pcn;
    int x;

    string fileName = "part1.pcn";

    readFile(&pcn, fileName);

    //select variable to cofactor on
    x = variableSelection(pcn);

    //get positive cofactor based on x

    //get negative cofactor based on x

    return 0;
}

int variableSelection(CubeList pcn) {
    //Variable Selection
        //Calculate C, T and |T - C| for all variables
    vector<int> varC(pcn.numVars, 0), varT(pcn.numVars, 0), varTminC(pcn.numVars, 0), varTplusC(pcn.numVars, 0);

    for (auto f : pcn.F) {
        for (int i = 0; i < f.size(); i++) {
            if (f[i] > 0)varT[i]++;
            if (f[i] < 0)varC[i]++;
        }
    }

    for (int i = 0; i < varTminC.size(); ++i)
    {
        varTminC[i] = abs(varT[i] - varC[i]);
        varTplusC[i] = varT[i] + varC[i];

    }

    bool areThereBinate = 0;
    int chosenVar = 99;
    for (int i = 0; i < varTminC.size(); ++i)
    {
        if (varC[i] && varT[i]) {
            areThereBinate = 1;
            break;
        }
    }
    if (areThereBinate)
    {
        vector<int> binMax = max_element(&varTplusC);

        //iterateThrough binMax and if any element as not(T[i] && C[i])) remove it from vector
        //most Binate = max(T+C) and (C[binateIndex] && T[binateIndex])

        //There's more than one variable that appears in multiple cubes
        if (binMax.size() > 1) {
            for (int k = binMax.size() - 1; k > 0; --k) {
                //if variable is not binate remove from cube
                int var = binMax[k];
                if (!(varC[var] && varT[var])) binMax.erase(binMax.begin() + k);
            }
        }
        //at this point, all variables that are unate should be removed

        auto binMin = min_element(&varTminC);
        chosenVar = binMin[0] + 1;

    }
    else
    {
        //no binate variables
        //choose the one that appears in the most cubes with lowest index
        auto maxElems = max_element(&varTplusC);
        chosenVar = maxElems[0] + 1;
    }
    cout << "Variable chosen: " << chosenVar << endl;
    return chosenVar;
}

void readFile(CubeList* cL, string fileName) {
    fstream newfile;
    newfile.open(fileName, ios::in);

    if (newfile.is_open())
    {
        std::string tp;
        std::getline(newfile, tp);
        cL->numVars = std::stoi(tp);
        std::getline(newfile, tp);
        cL->numCubes = std::stoi(tp);

        //Test memory on the heap
        /*cL->F.resize(1048576);
        for (int i = 0; i < cL->F.size(); i++) {
            cL->F[i].resize(20);
        }*/

        cL->F.resize(cL->numCubes);

        for (int i = 0; i < cL->F.size(); i++) {
            getline(newfile, tp);
            string temp;
            bool flag = 1;

            cL->F[i].resize(cL->numVars);
             
            //iterate through string;
            for (string::iterator it = tp.begin(); it != tp.end(); ++it) {
                //if not a space of end of line push to temp
                if (*it != 32 && it != (tp.end() - 1))temp.push_back(*it);  
                //if we're at the end of sring
                else if (it == tp.end() - 1) {
                    temp.push_back(*it);

                    int var = stoi(temp);
                    if (var > 0) cL->F[i][(var-1)] = 1;
                    else cL->F[i][abs(var)-1] = -1;
                    temp = "";
                }
                //we hit a space
                else {
                    //if first element, ignore
                    if (flag) {
                        flag = 0;
                        temp = "";
                        continue;
                    }
                    int var = stoi(temp);
                    if (var > 0) cL->F[i][(var - 1)] = 1;
                    else cL->F[i][abs(var)-1] = -1;
                    temp = "";
                }
            }
        }

        cout << "File Read" << endl;

    }
}

CubeList posCofactor(CubeList pcn, int var)
{
    CubeList pos_pcn = pcn;

    return pos_pcn;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
