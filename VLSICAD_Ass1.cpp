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
void andCube(CubeList* cL, int var, bool comp);
void orCube(CubeList* P, CubeList N);
bool compareVector(vector<int> v1, vector<int> v2);
int isSimple(CubeList F);
CubeList complement(CubeList F);
void printCube(CubeList C);

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
        //most binate var should be left, if there's still more than one, do a tiebreak
        if (binMax.size() > 1) {
            vector<int> varTminC2(binMax.size(), 0);
            int j = 0;
            for (auto i : binMax) {
                varTminC2[j] = varTminC[i];
                j++;
            }
            auto binMin = min_element(&varTminC2);
            chosenVar = binMax[binMin[0]]+1;
        }
        else {
            chosenVar = binMax[0] + 1;
        }
        

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
    CubeList* pos_pcn = &pcn;
    int ind = var - 1;

    for (int i = pos_pcn->numCubes - 1; i >= 0; i--) {
        //var is opposite of Shannon expansion
        //get rid of cube
        if (pos_pcn->F[i].at(ind) == -1) {
            pos_pcn->F.erase(pos_pcn->F.begin() + i);
            pos_pcn->numCubes--;
        }
        //set var in cube to dont care
        else if (pos_pcn->F[i].at(ind) == 1) {
            pos_pcn->F[i][ind] = 0;
        }
    }

    return *pos_pcn;
}

CubeList negCofactor(CubeList pcn, int var)
{
    CubeList* neg_pcn = &pcn;
    int ind = var - 1;

    for (int i = neg_pcn->numCubes - 1; i >= 0; i--) {
        //var is opposite of Shannon expansion
        //get rid of cube
        if (neg_pcn->F[i].at(ind) == 1) {
            neg_pcn->F.erase(neg_pcn->F.begin() + i);
            neg_pcn->numCubes--;
        }
        //set var in cube to dont care
        else if (neg_pcn->F[i].at(ind) == -1) {
            neg_pcn->F[i][ind] = 0;
        }
    }

    return *neg_pcn;
}

void andCube(CubeList* cL, int var, bool comp)
{
    int ind = var-1;
    for (int i = 0; i < cL->numCubes; i++) {
        if (comp)cL->F[i][ind] = 1;
        else cL->F[i][ind] = -1;
    }
}

bool compareVector(vector<int> v1, vector<int> v2)
{
    int n = v1.size();

    for (int i = 0; i < n; i++) {
        if (v1[i] != v2[i])return 0;
    }
    return 1;
}

void orCube(CubeList* P, CubeList N)
{
    int n = P->numCubes;
    int m = P->numVars;
    bool flag = 0;

    for (auto f : N.F) {
        for (int i = 0; i < n; i++) {
            //if theres a duplicate vector, set flag and don't copy
            if (compareVector(P->F[i], f))flag = 1;
        }
        if (flag == 0)P->F.push_back(f);
        flag = 0;
    }

}

int isSimple(CubeList F)
{
    int sum = 0;

    //cubeList is empty 
    if (F.numCubes == 0) return 1;
    //compute the complement directly
    else if (F.numCubes == 1)return 2;
    //check for "All Don't Cares" cube
    else {
        for (auto f : F.F) {
            for (auto i : f) {
                sum += std::abs(i);
            }
            //All Dont Care cubes is present
            if (sum == 0) return 3;
            sum = 0;
        }
        return 0;
    }
}

CubeList complement(CubeList F)
{
    int x;
    int jeez = isSimple(F);
    if (jeez) {
        CubeList* C = new CubeList();

        switch (jeez) {
        case 1:
            //cubeList is empty
            C->numCubes = F.numCubes;
            C->numVars = F.numVars;
            C->F[0] = vector<int>(C->numVars, 0);
            return *C;
            break;
        case 2:
            //compute complement directly
            C->numVars = F.numVars;
            C->F.push_back(vector<int>(C->numVars, 0));
            for (int i=0;i<F.numVars;i++) {
                C->F[0][i] = -1 * F.F[0][i];
            }
            C->numCubes = C->F.size();
            return *C;
            break;
        case 3:
            //All DOn't Cares
            return *C;
            break;
        default:
            break;
        }
    }
    else {
        x = variableSelection(F);
        CubeList pos_pcn = complement(posCofactor(F, x));
        CubeList neg_pcn = complement(negCofactor(F, x));

        andCube(&pos_pcn, x, 1);
        andCube(&neg_pcn, x, 0);

        orCube(&pos_pcn, neg_pcn);

        pos_pcn.numCubes = pos_pcn.F.size();

        return pos_pcn;
    }
}

void printCube(CubeList C)
{
    fstream newfile;
    string fileName = "URPOutput.txt";
    newfile.open(fileName, ios::out);
    
    newfile << C.numVars << "\n";
    newfile << C.numCubes << "\n";

    //lambda helper for evaluating the number of non zero elements
    auto vector_sum = [](vector<int> v)
    {
        int sum = 0;
        for (auto i : v) {
            sum += abs(i);
        }
        return sum;
    };

    if (newfile.is_open()) {
        for (int i = 0; i < C.numCubes; i++) {
            int numVars = vector_sum(C.F[i]);
            newfile << numVars << " ";
            for (int j = 0; j < C.numVars; j++) {
                if (C.F[i][j] != 0) {
                    newfile << C.F[i][j] * (j + 1) << " ";
                }
            }
            newfile << "\n";

        }
        newfile.close();
    }
    else {
        cout << "Could not open file\n";
    }

}

int main()
{
    CubeList pcn;
    int x;

    string fileName = "part1.pcn";

    readFile(&pcn, fileName);

    CubeList* C = new CubeList();
    *C = complement(pcn);

    printCube(*C);

    /*vector<vector<int>> F = { {1,0,0}, {1, -1, 0}, {0, 1,-1}, {0, 0, 0} };
    pcn_test->F = F;
    pcn_test->numCubes = 4;
    pcn_test->numVars = 3;

    int t = isSimple(*pcn_test);*/
    //select variable to cofactor on

    ////********MANUAL TEST*******

    //x = variableSelection(pcn);
    //CubeList pos_pcn = posCofactor(pcn, x);
    //x = variableSelection(pos_pcn);
    //CubeList pos_pcn2 = posCofactor(pos_pcn, x);
    //CubeList* P = new CubeList();
    //*P = complement(pos_pcn2);

    //x = variableSelection(pcn);
    //CubeList neg_pcn = negCofactor(pcn, x);
    //x = variableSelection(neg_pcn);
    //CubeList neg_pcn2 = negCofactor(neg_pcn, x);
    //CubeList* N = new CubeList();
    //*N = complement(neg_pcn2);

    //andCube(P, 1, 1);
    //andCube(N, 1, 0);

    //orCube(P, *N);

    ////********** END TEST ****

    return 0;
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
