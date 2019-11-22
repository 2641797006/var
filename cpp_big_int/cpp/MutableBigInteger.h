#ifndef _MUTABLEBIGINTEGER_H_
#define _MUTABLEBIGINTEGER_H_

#define _	0

#include <memory.h>

namespace akm {
using namespace std;

class MutableBigInteger {
  private:
	int *value;
	int value_len;
	int intLen;
	int offset=0;

	static const int BigInteger_BURNIKEL_ZIEGLER_THRESHOLD = 80;
	static const int BigInteger_BURNIKEL_ZIEGLER_OFFSET = 40;
	static const long LONG_MASK = 0xffffffffL;
	static const int KNUTH_POW2_THRESH_LEN = 6;
	static const int KNUTH_POW2_THRESH_ZEROS = 3;
	static const long Long_MIN_VALUE = 0x8000000000000000L;

	static long divWord(long n, int d);
	static int Integer_numberOfLeadingZeros(int i);
	static int Integer_numberOfTrailingZeros(int i);
	static int BigInteger_bitLengthForInt(int n);

  public:
	MutableBigInteger();
	MutableBigInteger(int val);
	MutableBigInteger(int *val, int val_len);
	MutableBigInteger(MutableBigInteger const& val);
	~MutableBigInteger();

	MutableBigInteger* divide(MutableBigInteger* b, MutableBigInteger* quotient, bool needRemainder=true);
	MutableBigInteger* divideKnuth(MutableBigInteger* b, MutableBigInteger* quotient, bool needRemainder=true);
	int compare(MutableBigInteger* b);
	void clear();
	int divideOneWord(int divisor, MutableBigInteger* quotient);
	void normalize();
	int getLowestSetBit();
	void rightShift(int n);
	void leftShift(int n);
	void primitiveLeftShift(int n);
	void primitiveRightShift(int n);
	void setValue(int *val, int val_len, int length);
	MutableBigInteger* divideMagnitude(MutableBigInteger *div, MutableBigInteger *quotient, bool needRemainder);
	bool unsignedLongCompare(long one, long two);
	int mulsub(int* q, int* a, int x, int len, int offset);
	int divadd(int *a, int a_len, int *result, int offset);
	int mulsubBorrow(int *q, int *a, int x, int len, int offset);
	void copyAndShift(int *src, int srcFrom, int srcLen, int *dst, int dstFrom, int shift);

}; // class MutableBigInteger

MutableBigInteger::~MutableBigInteger() {
	delete[] value;
}

MutableBigInteger::MutableBigInteger() {
	value = new int[1];
	value_len = 1;
	intLen = 0;
}

MutableBigInteger::MutableBigInteger(int val) {
	value = new int[1];
	value_len = 1;
	intLen = 1;
	value[0] = val;
}

MutableBigInteger::MutableBigInteger(int *val, int val_len) {
	value = val;
	value_len = val_len;
	intLen = val_len;
}

MutableBigInteger::MutableBigInteger(MutableBigInteger const& val) {
	intLen = val.intLen;
	memcpy(value, val.value+val.offset, intLen*sizeof(int)); 
}

MutableBigInteger*
MutableBigInteger::divide(MutableBigInteger* b, MutableBigInteger* quotient, bool needRemainder) {
	if (b->intLen < BigInteger_BURNIKEL_ZIEGLER_THRESHOLD ||
			intLen - b->intLen < BigInteger_BURNIKEL_ZIEGLER_OFFSET) {
		return divideKnuth(b, quotient, needRemainder);
	} else {
#if _
		return divideAndRemainderBurnikelZiegler(b, quotient);
#else
		return NULL;
#endif
	}
}

MutableBigInteger*
MutableBigInteger::divideKnuth(MutableBigInteger* b, MutableBigInteger* quotient, bool needRemainder) {
	if (b->intLen == 0)
		throw "BigInteger divide by zero";

	// Dividend is zero
	if (intLen == 0) {
		quotient->intLen = quotient->offset = 0;
		return needRemainder ? new MutableBigInteger() : NULL;
	}

	int cmp = compare(b);
	// Dividend less than divisor
	if (cmp < 0) {
		quotient->intLen = quotient->offset = 0;
		return needRemainder ? new MutableBigInteger(*this) : NULL;
	}
	// Dividend equal to divisor
	if (cmp == 0) {
		quotient->value[0] = quotient->intLen = 1;
		quotient->offset = 0;
		return needRemainder ? new MutableBigInteger() : NULL;
	}

	quotient->clear();
	// Special case one word divisor
	if (b->intLen == 1) {
		int r = divideOneWord(b->value[b->offset], quotient);
		if (needRemainder) {
			if (r == 0)
				return new MutableBigInteger();
			return new MutableBigInteger(r);
		} else {
			return NULL;
		}
	}

	// Cancel common powers of two if we're above the KNUTH_POW2_* thresholds
	if (intLen >= KNUTH_POW2_THRESH_LEN) {
		int trailingZeroBits = min(getLowestSetBit(), b->getLowestSetBit());
		if (trailingZeroBits >= KNUTH_POW2_THRESH_ZEROS*32) {
			MutableBigInteger *a = new MutableBigInteger(*this);
			b = new MutableBigInteger(*b);
			a->rightShift(trailingZeroBits);
			b->rightShift(trailingZeroBits);
			MutableBigInteger *r = a->divideKnuth(b, quotient);
			r->leftShift(trailingZeroBits);
			return r;
		}
	}

	return divideMagnitude(b, quotient, needRemainder);
}

int
MutableBigInteger::getLowestSetBit() {
        if (intLen == 0)
            return -1;
        int j, b;
        for (j=intLen-1; (j > 0) && (value[j+offset] == 0); j--)
            ;
        b = value[j+offset];
        if (b == 0)
            return -1;
        return ((intLen-1-j)<<5) + Integer_numberOfTrailingZeros(b);
    }

int
MutableBigInteger::compare(MutableBigInteger* b) {
	int blen = b->intLen;
	if (intLen < blen)
		return -1;
	if (intLen > blen)
		return 1;

	// Add Integer.MIN_VALUE to make the comparison act as unsigned integer
	// comparison.
	int *bval = b->value;
	for (int i = offset, j = b->offset; i < intLen + offset; i++, j++) {
		int b1 = value[i] + 0x80000000;
		int b2 = bval[j]  + 0x80000000;
		if (b1 < b2)
			return -1;
		if (b1 > b2)
			return 1;
	}
	return 0;
}

void
MutableBigInteger::clear() {
	offset = intLen = 0;
	memset(value, 0, value_len*sizeof(int));
}

int
MutableBigInteger::divideOneWord(int divisor, MutableBigInteger* quotient) {
	long divisorLong = divisor & LONG_MASK;

	// Special case of one word dividend
	if (intLen == 1) {
		long dividendValue = value[offset] & LONG_MASK;
		int q = (int) (dividendValue / divisorLong);
		int r = (int) (dividendValue - q * divisorLong);
		quotient->value[0] = q;
		quotient->intLen = (q == 0) ? 0 : 1;
		quotient->offset = 0;
		return r;
	}

	if (quotient->value_len < intLen) {
		delete[] quotient->value;
		quotient->value = new int[intLen];
		quotient->value_len = intLen;
	}
	quotient->offset = 0;
	quotient->intLen = intLen;

	// Normalize the divisor
	int shift = Integer_numberOfLeadingZeros(divisor);

	int rem = value[offset];
	long remLong = rem & LONG_MASK;
	if (remLong < divisorLong) {
		quotient->value[0] = 0;
	} else {
		quotient->value[0] = (int)(remLong / divisorLong);
		rem = (int) (remLong - (quotient->value[0] * divisorLong));
		remLong = rem & LONG_MASK;
	}
	int xlen = intLen;
	while (--xlen > 0) {
		long dividendEstimate = (remLong << 32) |
			(value[offset + intLen - xlen] & LONG_MASK);
		int q;
		if (dividendEstimate >= 0) {
			q = (int) (dividendEstimate / divisorLong);
			rem = (int) (dividendEstimate - q * divisorLong);
		} else {
			long tmp = divWord(dividendEstimate, divisor);
			q = (int) (tmp & LONG_MASK);
			rem = (int) ((unsigned long)tmp >> 32);
		}
		quotient->value[intLen - xlen] = q;
		remLong = rem & LONG_MASK;
	}

	quotient->normalize();
	// Unnormalize
	if (shift > 0)
		return rem % divisor;
	else
		return rem;
}

void
MutableBigInteger::normalize() {
        if (intLen == 0) {
            offset = 0;
            return;
        }

        int index = offset;
        if (value[index] != 0)
            return;

        int indexBound = index+intLen;
        do {
            index++;
        } while(index < indexBound && value[index] == 0);

        int numZeros = index - offset;
        intLen -= numZeros;
        offset = (intLen == 0 ?  0 : offset+numZeros);
    }

long
MutableBigInteger::divWord(long n, int d) {
        long dLong = d & LONG_MASK;
        long r;
        long q;
        if (dLong == 1) {
            q = (int)n;
            r = 0;
            return (r << 32) | (q & LONG_MASK);
        }

        // Approximate the quotient and remainder
        q = ((unsigned long)n >> 1) / ((unsigned long)dLong >> 1);
        r = n - q*dLong;

        // Correct the approximation
        while (r < 0) {
            r += dLong;
            q--;
        }
        while (r >= dLong) {
            r -= dLong;
            q++;
        }
        // n - q*dlong == r && 0 <= r <dLong, hence we're done.
        return (r << 32) | (q & LONG_MASK);
    }

int
MutableBigInteger::Integer_numberOfLeadingZeros(int i) {
        // HD, Figure 5-6
        if (i == 0)
            return 32;
        int n = 1;
        if ((unsigned int)i >> 16 == 0) { n += 16; i <<= 16; }
        if ((unsigned int)i >> 24 == 0) { n +=  8; i <<=  8; }
        if ((unsigned int)i >> 28 == 0) { n +=  4; i <<=  4; }
        if ((unsigned int)i >> 30 == 0) { n +=  2; i <<=  2; }
        n -= (unsigned int)i >> 31;
        return n;
    }

int
MutableBigInteger::Integer_numberOfTrailingZeros(int i) {
        // HD, Figure 5-14
        int y;
        if (i == 0) return 32;
        int n = 31;
        y = i <<16; if (y != 0) { n = n -16; i = y; }
        y = i << 8; if (y != 0) { n = n - 8; i = y; }
        y = i << 4; if (y != 0) { n = n - 4; i = y; }
        y = i << 2; if (y != 0) { n = n - 2; i = y; }
        return n - ((unsigned int)(i << 1) >> 31);
    }

void
MutableBigInteger::rightShift(int n) {
	if (intLen == 0)
            return;
        int nInts = (unsigned int)n >> 5;
        int nBits = n & 0x1F;
        this->intLen -= nInts;
        if (nBits == 0)
            return;
        int bitsInHighWord = BigInteger_bitLengthForInt(value[offset]);
        if (nBits >= bitsInHighWord) {
            this->primitiveLeftShift(32 - nBits);
            this->intLen--;
        } else {
            primitiveRightShift(nBits);
        }
    }

void
MutableBigInteger::leftShift(int n) {
        /*
         * If there is enough storage space in this MutableBigInteger already
         * the available space will be used. Space to the right of the used
         * ints in the value array is faster to utilize, so the extra space
         * will be taken from the right if possible.
         */
        if (intLen == 0)
           return;
        int nInts = (unsigned int)n >> 5;
        int nBits = n&0x1F;
        int bitsInHighWord = BigInteger_bitLengthForInt(value[offset]);

        // If shift can be done without moving words, do so
        if (n <= (32-bitsInHighWord)) {
            primitiveLeftShift(nBits);
            return;
        }

        int newLen = intLen + nInts +1;
        if (nBits <= (32-bitsInHighWord))
            newLen--;
        if (value_len < newLen) {
            // The array must grow
            int* result = new int[newLen];
            for (int i=0; i < intLen; i++)
                result[i] = value[offset+i];
            setValue(result, newLen, newLen);
        } else if (value_len - offset >= newLen) {
            // Use space on right
            for(int i=0; i < newLen - intLen; i++)
                value[offset+intLen+i] = 0;
        } else {
            // Must use space on left
            for (int i=0; i < intLen; i++)
                value[i] = value[offset+i];
            for (int i=intLen; i < newLen; i++)
                value[i] = 0;
            offset = 0;
        }
        intLen = newLen;
        if (nBits == 0)
            return;
        if (nBits <= (32-bitsInHighWord))
            primitiveLeftShift(nBits);
        else
            primitiveRightShift(32 -nBits);
    }

int
MutableBigInteger::BigInteger_bitLengthForInt(int n) {
        return 32 - Integer_numberOfLeadingZeros(n);
    }

void
MutableBigInteger::primitiveLeftShift(int n) {
        int* val = value;
        int n2 = 32 - n;
        for (int i=offset, c=val[i], m=i+intLen-1; i < m; i++) {
            int b = c;
            c = val[i+1];
            val[i] = (b << n) | ((unsigned int)c >> n2);
        }
        val[offset+intLen-1] <<= n;
    }

void
MutableBigInteger::primitiveRightShift(int n) {
        int* val = value;
        int n2 = 32 - n;
        for (int i=offset+intLen-1, c=val[i]; i > offset; i--) {
            int b = c;
            c = val[i-1];
            val[i] = (c << n2) | ((unsigned int)b >> n);
        }
        val[offset] = (unsigned int)val[offset] >> n;
    }

void
MutableBigInteger::setValue(int *val, int val_len, int length) {
        value = val;
	value_len = val_len;
        intLen = length;
        offset = 0;
    }

void
MutableBigInteger::copyAndShift(int *src, int srcFrom, int srcLen, int *dst, int dstFrom, int shift) {
        int n2 = 32 - shift;
        int c=src[srcFrom];
        for (int i=0; i < srcLen-1; i++) {
            int b = c;
            c = src[++srcFrom];
            dst[dstFrom+i] = (b << shift) | ((unsigned int)c >> n2);
        }
        dst[dstFrom+srcLen-1] = c << shift;
    }

bool
MutableBigInteger::unsignedLongCompare(long one, long two) {
        return (one+Long_MIN_VALUE) > (two+Long_MIN_VALUE);
    }

int
MutableBigInteger::mulsub(int *q, int *a, int x, int len, int offset) {
        long xLong = x & LONG_MASK;
        long carry = 0;
        offset += len;

        for (int j=len-1; j >= 0; j--) {
            long product = (a[j] & LONG_MASK) * xLong + carry;
            long difference = q[offset] - product;
            q[offset--] = (int)difference;
            carry = ((unsigned long)product >> 32)
                     + (((difference & LONG_MASK) >
                         (((~(int)product) & LONG_MASK))) ? 1:0);
        }
        return (int)carry;
    }

int
MutableBigInteger::divadd(int *a, int a_len, int *result, int offset) {
        long carry = 0;

        for (int j=a_len-1; j >= 0; j--) {
            long sum = (a[j] & LONG_MASK) +
                       (result[j+offset] & LONG_MASK) + carry;
            result[j+offset] = (int)sum;
            carry = (unsigned long)sum >> 32;
        }
        return (int)carry;
    }

int
MutableBigInteger::mulsubBorrow(int *q, int *a, int x, int len, int offset) {
        long xLong = x & LONG_MASK;
        long carry = 0;
        offset += len;
        for (int j=len-1; j >= 0; j--) {
            long product = (a[j] & LONG_MASK) * xLong + carry;
            long difference = q[offset--] - product;
            carry = ((unsigned long)product >> 32)
                     + (((difference & LONG_MASK) >
                         (((~(int)product) & LONG_MASK))) ? 1:0);
        }
        return (int)carry;
    }


MutableBigInteger*
MutableBigInteger::divideMagnitude(MutableBigInteger *div,
                                              MutableBigInteger *quotient,
                                              bool needRemainder ) {
        // assert div.intLen > 1
        // D1 normalize the divisor
        int shift = Integer_numberOfLeadingZeros(div->value[div->offset]);
        // Copy divisor value to protect divisor
        const int dlen = div->intLen;
        int* divisor;
	int divisor_len;
        MutableBigInteger *rem; // Remainder starts as dividend with space for a leading zero
        if (shift > 0) {
            divisor = new int[dlen];
            copyAndShift(div->value,div->offset,dlen,divisor,0,shift);
            if (Integer_numberOfLeadingZeros(value[offset]) >= shift) {
		int remarr_len = intLen + 1;
                int* remarr = new int[remarr_len];
                rem = new MutableBigInteger(remarr, remarr_len);
                rem->intLen = intLen;
                rem->offset = 1;
                copyAndShift(value,offset,intLen,remarr,1,shift);
            } else {
		int remarr_len = intLen + 2;
                int* remarr = new int[remarr_len];
                rem = new MutableBigInteger(remarr, remarr_len);
                rem->intLen = intLen+1;
                rem->offset = 1;
                int rFrom = offset;
                int c=0;
                int n2 = 32 - shift;
                for (int i=1; i < intLen+1; i++,rFrom++) {
                    int b = c;
                    c = value[rFrom];
                    remarr[i] = (b << shift) | ((unsigned int)c >> n2);
                }
                remarr[intLen+1] = c << shift;
            }
        } else {
            divisor_len = div->intLen;
            divisor = new int[divisor_len];
            memcpy(divisor, value+offset, divisor_len*sizeof(int));
            rem = new MutableBigInteger(new int[intLen + 1], intLen+1);
            memcpy(rem->value+1, value+offset, intLen*sizeof(int));
            rem->intLen = intLen;
            rem->offset = 1;
        }

        int nlen = rem->intLen;

        // Set the quotient size
        const int limit = nlen - dlen + 1;
        if (quotient->value_len < limit) {
            quotient->value = new int[limit];
            quotient->value_len = limit;
            quotient->offset = 0;
        }
        quotient->intLen = limit;
        int* q = quotient->value;


        // Must insert leading 0 in rem if its length did not change
        if (rem->intLen == nlen) {
            rem->offset = 0;
            rem->value[0] = 0;
            rem->intLen++;
        }

        int dh = divisor[0];
        long dhLong = dh & LONG_MASK;
        int dl = divisor[1];

        // D2 Initialize j
        for (int j=0; j < limit-1; j++) {
            // D3 Calculate qhat
            // estimate qhat
            int qhat = 0;
            int qrem = 0;
            bool skipCorrection = false;
            int nh = rem->value[j+rem->offset];
            int nh2 = nh + 0x80000000;
            int nm = rem->value[j+1+rem->offset];

            if (nh == dh) {
                qhat = ~0;
                qrem = nh + nm;
                skipCorrection = qrem + 0x80000000 < nh2;
            } else {
                long nChunk = (((long)nh) << 32) | (nm & LONG_MASK);
                if (nChunk >= 0) {
                    qhat = (int) (nChunk / dhLong);
                    qrem = (int) (nChunk - (qhat * dhLong));
                } else {
                    long tmp = divWord(nChunk, dh);
                    qhat = (int) (tmp & LONG_MASK);
                    qrem = (int) ((unsigned long)tmp >> 32);
                }
            }

            if (qhat == 0)
                continue;

            if (!skipCorrection) { // Correct qhat
                long nl = rem->value[j+2+rem->offset] & LONG_MASK;
                long rs = ((qrem & LONG_MASK) << 32) | nl;
                long estProduct = (dl & LONG_MASK) * (qhat & LONG_MASK);

                if (unsignedLongCompare(estProduct, rs)) {
                    qhat--;
                    qrem = (int)((qrem & LONG_MASK) + dhLong);
                    if ((qrem & LONG_MASK) >=  dhLong) {
                        estProduct -= (dl & LONG_MASK);
                        rs = ((qrem & LONG_MASK) << 32) | nl;
                        if (unsignedLongCompare(estProduct, rs))
                            qhat--;
                    }
                }
            }

            // D4 Multiply and subtract
            rem->value[j+rem->offset] = 0;
            int borrow = mulsub(rem->value, divisor, qhat, dlen, j+rem->offset);

            // D5 Test remainder
            if (borrow + 0x80000000 > nh2) {
                // D6 Add back
                divadd(divisor, divisor_len, rem->value, j+1+rem->offset);
                qhat--;
            }

            // Store the quotient digit
            q[j] = qhat;
        } // D7 loop on j
        // D3 Calculate qhat
        // estimate qhat
        int qhat = 0;
        int qrem = 0;
        bool skipCorrection = false;
        int nh = rem->value[limit - 1 + rem->offset];
        int nh2 = nh + 0x80000000;
        int nm = rem->value[limit + rem->offset];

        if (nh == dh) {
            qhat = ~0;
            qrem = nh + nm;
            skipCorrection = qrem + 0x80000000 < nh2;
        } else {
            long nChunk = (((long) nh) << 32) | (nm & LONG_MASK);
            if (nChunk >= 0) {
                qhat = (int) (nChunk / dhLong);
                qrem = (int) (nChunk - (qhat * dhLong));
            } else {
                long tmp = divWord(nChunk, dh);
                qhat = (int) (tmp & LONG_MASK);
                qrem = (int) ((unsigned long)tmp >> 32);
            }
        }
        if (qhat != 0) {
            if (!skipCorrection) { // Correct qhat
                long nl = rem->value[limit + 1 + rem->offset] & LONG_MASK;
                long rs = ((qrem & LONG_MASK) << 32) | nl;
                long estProduct = (dl & LONG_MASK) * (qhat & LONG_MASK);

                if (unsignedLongCompare(estProduct, rs)) {
                    qhat--;
                    qrem = (int) ((qrem & LONG_MASK) + dhLong);
                    if ((qrem & LONG_MASK) >= dhLong) {
                        estProduct -= (dl & LONG_MASK);
                        rs = ((qrem & LONG_MASK) << 32) | nl;
                        if (unsignedLongCompare(estProduct, rs))
                            qhat--;
                    }
                }
            }


            // D4 Multiply and subtract
            int borrow;
            rem->value[limit - 1 + rem->offset] = 0;
            if(needRemainder)
                borrow = mulsub(rem->value, divisor, qhat, dlen, limit - 1 + rem->offset);
            else
                borrow = mulsubBorrow(rem->value, divisor, qhat, dlen, limit - 1 + rem->offset);

            // D5 Test remainder
            if (borrow + 0x80000000 > nh2) {
                // D6 Add back
                if(needRemainder)
                    divadd(divisor, divisor_len, rem->value, limit - 1 + 1 + rem->offset);
                qhat--;
            }

            // Store the quotient digit
            q[(limit - 1)] = qhat;
        }


        if (needRemainder) {
            // D8 Unnormalize
            if (shift > 0)
                rem->rightShift(shift);
            rem->normalize();
        }
        quotient->normalize();
        return needRemainder ? rem : NULL;
    }

} // namespace akm

#endif // _MUTABLEBIGINTEGER_H_

