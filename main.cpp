#include "my_callback.h"
/*

Only unit-test here ^^

*/
// TODO: When testing, put some random code in here, to make sure
//the registers do not get corrupted in between, which could be a problem
void test_func()
{
	printf("\tTEST void func\n");
}
void __cdecl test_func2(uint32_t a)
{
	printf("\tTEST 32b arg: %d\n", a);
}
void __cdecl test_func3(uint8_t a)
{
	printf("\tTEST 8b arg: %d\n", a);
}

class DummyClass
{
public:
	void DummyFunc() {
		printf("\tDummyClass::DummyFunc\n");
	}
};

int main(int argc, char** argv)
{
	CallFunc((uint32_t)test_func, 0);
	CallFunc((uint32_t)test_func2, 1, 12);
	CallFunc((uint32_t)test_func3, 1, 18);
	CallFunc((uint32_t)test_func2, 2, 12, 13); // Pass 2 args, even through function needs one.
	CallFunc((uint32_t)test_func2, 1); // Forgot to pass an arg.
	DummyClass* dc = new DummyClass();
	CallClassFunc(DummyClass, DummyFunc, dc, 0);
	DummyClass d;
	CallClassFunc(DummyClass, DummyFunc, &d, 0);
	system("PAUSE");
	return 0;
}