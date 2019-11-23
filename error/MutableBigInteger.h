#ifndef _MUTABLEBIGINTEGER_H_
#define _MUTABLEBIGINTEGER_H_

#ifndef _ARRAY_H_
#include "Array.h"
#endif

namespace akm {
using namespace std;

class MutableBigInteger {
  private:
	Array<int> value;
	int intLen;
	int offset = 0;

  public:
	MutableBigInteger();
};

MutableBigInteger::MutableBigInteger() {
        value = Array<int>(1);
        intLen = 0;
    }

} // namespace akm

#endif // _MUTABLEBIGINTEGER_H_
