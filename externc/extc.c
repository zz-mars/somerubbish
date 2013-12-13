#include"glob.h"
#include"sb.h"
int main()
{
	printf("c_main now call f!\n");
	f();
	printf("c_main now call b!\n");
	b();
	return 0;
}
