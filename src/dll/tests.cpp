
extern "C"
{
	int __declspec(dllexport) test(int a, int b)
	{
		return a + b;
	}
}