#include <iostream>
#include "SHA256.h"
 
using std::string;
using std::cout;
using std::endl;

#define MAXN 102400000  //1000 KB
int main(int argc, char *argv[])
{
    string A(MAXN,'a');
	string B(MAXN,'a');
	B += "d";
    //string output1 = sha256(A);
	//string output1 = sha256(A);
	if(sha256(A)==sha256(B))
	cout<<"true"<<endl;
	else
	cout<<"False"<<endl;
    
    return 0;
}