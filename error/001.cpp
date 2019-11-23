struct A {
	void operator= (A && other) {
		new(this) A();
	}
};

struct B {
	A a;
	B() { a = A(); }
};

int main()
{

}

