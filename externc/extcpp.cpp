#include<iostream>
#include"sb.h"
using namespace std;
int main()
{
#ifdef __cplusplus
	cout<<"cpp file"<<endl;
#endif
	cout<<"cpp_main now call f!"<<endl;
	f();
	cout<<"cpp_main now call b!"<<endl;
	b();
	return 0;
}
