template <class T>
struct A {
	void operator= (A && other) {
		new(this) A();
	}
};

class B {
  public:
	A<int> value;

	B() {
		value = A<int>();
	}
};

