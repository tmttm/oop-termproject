#include <iostream>
#include <fstream>
using namespace std;

ofstream fout("test.txt");

int main(){
    cout << "Welcome to hell" << endl;
    fout << "Welcome to hell" << endl;
    fout.close();
}