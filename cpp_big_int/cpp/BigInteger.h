#ifndef _BIGINTEGER_H_
#define _BIGINTEGER_H_

#ifndef NDEBUG
#define TEST	printf("__LINE__=%d\n", __LINE__);
#else
#define TEST
#endif

#ifndef _GLIBCXX_STRING
#include <string>
#endif

#include <memory.h>

namespace akm {
using namespace std;

class BigInteger{
  private:
	int signum;

	int *mag;
	int mag_len;

	static const int MIN_RADIX = 2, MAX_RADIX = 36;
	static const long LONG_MASK = 0xffffffffL;
	static const int MAX_MAG_LENGTH = (1 << 26);
	static const long bitsPerDigit[37];
	static const int digitsPerInt[37];
	static const int intRadix[37];
	static const BigInteger ZERO;

	static void destructiveMulAdd(int* x, int x_length, int y, int z);
	static int* trustedStripLeadingZeroInts(int *val, int val_length, int *_arr_len);
	static void reportOverflow();

	void checkRange();

  public:
	BigInteger(int* magnitude, int magnitude_len, int signum);
	BigInteger(string val, int radix=10);
	~BigInteger();

	string toString(int radix=10) const;
};

BigInteger::~BigInteger() { delete[] mag; }

BigInteger::BigInteger(int* magnitude, int magnitude_len, int signum) {
	this->signum = (magnitude_len == 0 ? 0 : signum);
	this->mag = magnitude;
	this->mag_len = magnitude_len;
	if (mag_len >= MAX_MAG_LENGTH) {
		checkRange();
	}
}

BigInteger::BigInteger(string val, int radix) {
	int cursor = 0, numDigits;
	const int len = val.length();
	if (radix < MIN_RADIX || radix > MAX_RADIX)
		throw "Radix out of range";
	if (len == 0)
		throw "Zero length BigInteger";

	// Check for at most one leading sign
	int sign = 1;
	int index1 = val.rfind('-');
	int index2 = val.rfind('+');
	if (index1 >= 0) {
		if (index1 != 0 || index2 >= 0) {
			throw "Illegal embedded sign character";
		}
		sign = -1;
		cursor = 1;
	} else if (index2 >= 0) {
		if (index2 != 0) {
			throw "Illegal embedded sign character";
		}
		cursor = 1;
	}
	if (cursor == len)
		throw "Zero length BigInteger";

	// Skip leading zeros and compute number of digits in magnitude
	while (cursor < len && val[cursor] == '0')
		++cursor;

	if (cursor == len) {
		signum = 0;
		mag_len = ZERO.mag_len;
		mag = new int[mag_len];
		memcpy(mag, ZERO.mag, mag_len*sizeof(int));
		return;
	}

	numDigits = len - cursor;
	signum = sign;

	// Pre-allocate array of expected size. May be too large but can
	// never be too small. Typically exact.
	long numBits = ((unsigned long)(numDigits * bitsPerDigit[radix]) >> 10) + 1;
	if (numBits + 31 >= (1L << 32))
		reportOverflow();

	const int numWords = (unsigned int) (numBits + 31) >> 5;
	int* magnitude = new int[numWords];

	// Process first (potentially short) digit group
	int firstGroupLen = numDigits % digitsPerInt[radix];
	if (firstGroupLen == 0)
		firstGroupLen = digitsPerInt[radix];
	string group = val.substr(cursor, firstGroupLen);
	//magnitude[numWords - 1] = Integer.parseInt(group, radix);
	magnitude[numWords - 1] = stoi(group, 0, radix);
	if (magnitude[numWords - 1] < 0)
		throw "Illegal digit";

	// Process remaining digit groups
	int superRadix = intRadix[radix];
	int groupVal = 0;
	while (cursor < len) {
		group = val.substr(cursor, digitsPerInt[radix]);
		cursor += digitsPerInt[radix];
		groupVal = stoi(group, 0, radix);
		if (groupVal < 0)
			throw "Illegal digit";
		destructiveMulAdd(magnitude, numWords, superRadix, groupVal);
	}
	// Required for cases where the array was overallocated.
	mag_len = -1;
	mag = trustedStripLeadingZeroInts(magnitude, numWords, &mag_len);
	if (mag_len < 0)
		mag_len = numWords;
	else
		delete[] magnitude;
	if (mag_len >= MAX_MAG_LENGTH) {
		checkRange();
	}
}

string
BigInteger::toString(int radix) const{
	
}

void
BigInteger::destructiveMulAdd(int* x, int x_length, int y, int z) {
        // Perform the multiplication word by word
        long ylong = y & LONG_MASK;
        long zlong = z & LONG_MASK;
        int len = x_length;

        long product = 0;
        long carry = 0;
        for (int i = len-1; i >= 0; i--) {
            product = ylong * (x[i] & LONG_MASK) + carry;
            x[i] = (int)product;
            carry = (unsigned long)product >> 32;
        }

        // Perform the addition
        long sum = (x[len-1] & LONG_MASK) + zlong;
        x[len-1] = (int)sum;
        carry = (unsigned long)sum >> 32;
        for (int i = len-2; i >= 0; i--) {
            sum = (x[i] & LONG_MASK) + carry;
            x[i] = (int)sum;
            carry = (unsigned long)sum >> 32;
        }
    }

int*
BigInteger::trustedStripLeadingZeroInts(int *val, int val_length, int *_arr_len) {
        int vlen = val_length;
        int keep;

        // Find first nonzero byte
        for (keep = 0; keep < vlen && val[keep] == 0; keep++)
            ;
	if (keep != 0) {
		int len = vlen - keep;
		int *arr = new int[len];
		memcpy(arr, val+keep, len*sizeof(int));
		*_arr_len = len;
		val = arr;
	}
	return val;
    }

void
BigInteger::checkRange() {
        if (mag_len > MAX_MAG_LENGTH || (mag_len == MAX_MAG_LENGTH && mag[0] < 0)) {
            reportOverflow();
        }
    }

void
BigInteger::reportOverflow() {
        throw "BigInteger would overflow supported range";
    }

const long BigInteger::bitsPerDigit[37] = { 0, 0,
        1024, 1624, 2048, 2378, 2648, 2875, 3072, 3247, 3402, 3543, 3672,
        3790, 3899, 4001, 4096, 4186, 4271, 4350, 4426, 4498, 4567, 4633,
        4696, 4756, 4814, 4870, 4923, 4975, 5025, 5074, 5120, 5166, 5210,
                                           5253, 5295};

const int BigInteger::digitsPerInt[37] = {0, 0, 30, 19, 15, 13, 11,
        11, 10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5};

const int BigInteger::intRadix[37] = {0, 0,
        0x40000000, 0x4546b3db, 0x40000000, 0x48c27395, 0x159fd800,
        0x75db9c97, 0x40000000, 0x17179149, 0x3b9aca00, 0xcc6db61,
        0x19a10000, 0x309f1021, 0x57f6c100, 0xa2f1b6f,  0x10000000,
        0x18754571, 0x247dbc80, 0x3547667b, 0x4c4b4000, 0x6b5a6e1d,
        0x6c20a40,  0x8d2d931,  0xb640000,  0xe8d4a51,  0x1269ae40,
        0x17179149, 0x1cb91000, 0x23744899, 0x2b73a840, 0x34e63b41,
        0x40000000, 0x4cfa3cc1, 0x5c13d840, 0x6d91b519, 0x39aa400
	};

const BigInteger BigInteger::ZERO(new int[0], 0, 0);


} // namespace akm

#endif // _BIGINT_H_

