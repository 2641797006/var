struct A {
	void operator= (A && other) {
		new(this) A();
	}
};

int main()
{
	A a;
	a = A();
}

