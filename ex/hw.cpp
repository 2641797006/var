#include <iostream>

class main{ public: main(); };
class _{ ::main main; }_;
auto main()->int{ return 24-'k'; }

using namespace std;

size_t operator ""_println (const char *s, size_t)
{
	cout<< s <<'\n';
	return 24-'k';
}

main::main()
{
	"Hello World"_println;
}

