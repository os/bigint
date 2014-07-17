//file:big_integer.cpp
//big_integer class implementation file
#include "big_integer.h"
#include <string>
#include <iostream>

// constructor
big_integer::big_integer(string value) {
	negative = (value[0] == '-');

	// init digits
	for (int i = (negative ? 1 : 0); i < value.length(); i++)
		digits.push_back(int(value[i] - '0'));
}

big_integer::big_integer() {
	negative = false;
}

big_integer big_integer::add(big_integer other) {
// precon: other should not be NULL

	big_integer result;

	// no need to calculate if the numbers are empty
	if (digits.empty() && other.digits.empty())
		return result;
	else if (digits.empty())
		return other;
	else if (other.digits.empty()) {
		result = *this;
		return result;
	}

	// if the numbers have different signs, subtract
	if (negative != other.negative) {
		result = this->absolute().subtract(other.absolute());
		result.negative = negative != result.negative;

		return result;
	}

	int digit_size = digits.size();

	if (other.digits.size() > digit_size)
		digit_size = other.digits.size();

	int digit; bool carry = false;

	for (int i = 0; i < digit_size; i++) {
		digit = carry ? 1 : 0; carry = false;

		// current digit exists?
		if (digits.size() > i)
			digit += digits[digits.size() - 1 - i];

		// current digit exists?
		if (other.digits.size() > i)
			digit += other.digits[other.digits.size() - 1 - i];

		// add digit totals
		result.digits.insert(result.digits.begin(), digit % 10);
		carry = (digit >= 10);

		// is this the last digit and carry?
		if (i == (digit_size - 1) && carry)
			result.digits.insert(result.digits.begin(), 1);
	}

	result.negative = negative;
	return result;
}

big_integer big_integer::subtract (big_integer other) {
// precon: other should not be NULL

	big_integer result;

	// no need to calculate if they are empty
	if (digits.empty() && other.digits.empty())
		return result;
	else if (digits.empty())
		return other;
	else if (other.digits.empty()) {
		result = *this;
		return result;
	}

	// if the numbers have different signs, add
	if (negative != other.negative) {
		result = this->absolute().add(other.absolute());
		result.negative = negative != result.negative;

		return result;
	}

	// if the numbers are equal, return 0
	if (this->absolute().equals(other)) {
		result.digits.push_back(0);
		return result;
	}

	big_integer bigger, smaller;
	int digit, borrow_digit, digit_size = digits.size();

	if (other.digits.size() > digit_size)
		digit_size = other.digits.size();

	if (this->absolute().bigger_than(other.absolute())) {
		bigger = *this;
		smaller = other;
	} else {
		bigger = other;
		smaller = *this;
	}

	for (int i = 1; i <= digit_size; i++) {
		digit = bigger.digits[bigger.digits.size() - i];

		// substract
		if (smaller.digits.size() >= i)
			digit -= smaller.digits[smaller.digits.size() - i];

		if (digit < 0) {
			borrow_digit = bigger.digits.size() - (i + 1);

			do {
				if (bigger.digits[borrow_digit]) {
					bigger.digits[borrow_digit]--;
					digit += 10;
				} else bigger.digits[borrow_digit] = 9;

				borrow_digit--;
			} while (digit < 0);
		}

		// add current digit value to result
		result.digits.insert(result.digits.begin(), digit);
	}

	result.normalize();

	// result will have the same sign with the bigger number
	result.negative = negative ? bigger_than(other) : !bigger_than(other);
	return result;
}

big_integer big_integer::multiply (big_integer other) {
// precon: other should not be NULL

	int digit, carry = 0;
	big_integer result, total;
	vector<big_integer> totals;

	// no need to calculate if they are empty
	if (digits.empty() || other.digits.empty())
		return result;

	for (int sdigit = digits.size() - 1; sdigit >= 0; sdigit--) {
		// no need to multiply, when the multiplier is 0
		if (!digits[sdigit]) continue;

		// reset for new multiplication
		carry = 0;
		total.digits.clear();

		// add zeros at the end by the current digit
		while (total.digits.size() < (digits.size() - sdigit - 1))
			total.digits.push_back(0);

		for (int odigit = other.digits.size() - 1; odigit >= 0; odigit--) {
			digit = carry ? carry : 0;
			digit += digits[sdigit] * other.digits[odigit];
			carry = digit / 10;
			total.digits.insert(total.digits.begin(), digit % 10);
		}

		// if carry digits, add
		if (carry) total.digits.insert(total.digits.begin(), carry);
		totals.push_back(total);
	}

	// add first element to result
	result = totals.front();
	totals.erase(totals.begin());

	// iterate through totals to find final total
	while (totals.size()) {
		result = result.add(totals.front());
		totals.erase(totals.begin());
	}

	// set sign
	result.negative = negative != other.negative;
	return result;
}

big_integer big_integer::div(big_integer other) {
// precon: other should not be NULL

	int portion_size;
	big_integer result,
				dividend = this->absolute(),
				divisor = other.absolute(),
				portion;

	// no need to calculate if dividend is less than or equal to divisor
	if (dividend.equals(divisor) || !dividend.bigger_than(divisor)) {
		result.digits.push_back(dividend.equals(divisor));
		return result;
	}

	dividend = this->absolute();
	vector<int>::iterator it;

	while(dividend.bigger_than(divisor) || dividend.equals(divisor)) {
		result.digits.push_back(0);

		// portion size must be at least as big as the divisor
		portion_size = divisor.digits.size();
		portion = dividend.sub(0, portion_size);

		// portion is not big enough, add one more digit
		while (!portion.bigger_than(divisor))
			portion = dividend.sub(0, ++portion_size);

		while (portion.bigger_than(divisor) || portion.equals(divisor)) {
			portion = portion.subtract(divisor);
			result.digits.back()++;
		}

		// remove the portion from the dividend
		dividend.discard(0, portion_size);

		// insert remainder digits back to dividend
		it = dividend.digits.begin();
		while (!portion.digits.empty()) {
			dividend.digits.insert(it++, portion.digits.front());
			portion.digits.erase(portion.digits.begin());
		}
	}

	// set sign
	result.negative = negative != other.negative;
	return result;
}

big_integer big_integer::mod(big_integer other) {
// precon: other should not be NULL
// postcon: returns the standard C++ mod operation output

	big_integer result, self = this->absolute(), mod = other.absolute();

	if (self.equals(mod))
		result.digits.push_back(0);
	else if (self.bigger_than(mod))
		result = self.subtract(mod.multiply(self.div(mod)));
	else result = self;

	result.negative = negative;
	return result;
}

bool big_integer::equals (big_integer other) {
// precon: other should not be NULL

	// not equal if their sizes are different
	if (digits.size() != other.digits.size())
		return false;

	// not equal if they have different signs
	if (negative != other.negative)
		return false;

	// not equal if their digit values are different
	for (int i = 0; i < digits.size(); i++)
		if (digits[i] != other.digits[i])
			return false;

	return true;
}

bool big_integer::bigger_than(big_integer other) {
// precon: other should not be NULL

	if (negative != other.negative)
		return !negative;

	// bigger digit size means bigger number
	if (digits.size() != other.digits.size())
		return digits.size() > other.digits.size();

	// compare digits from left to right
	for (int i = 0; i < digits.size(); i++)
		if (digits[i] > other.digits[i])
			return !negative;
		else if (digits[i] < other.digits[i])
			return negative;

	return false;
}

big_integer big_integer::absolute() {
// postcon: returns the absolute version of the number

	big_integer result = *this;
	if (result.negative) result.negative = false;
	return result;
}

void big_integer::normalize() {
// postcon: discards the leading zeros

	while (digits.front() == 0)
		digits.erase(digits.begin());
}

big_integer big_integer::sub(int pos, int size) {
// precon: pos must be pointing to a valid location
// postcon: returns the sub number starting from pos and
//     following with size of digits

	big_integer result;
	vector<int>::iterator it;

	if (size <= 0) size = digits.size() - pos;

	for (it = digits.begin() + pos; it < digits.begin() + size; it++)
		result.digits.push_back(*it);

	return result;
}

void big_integer::discard(int pos, int size) {
// precon: pos must be pointing to a valid location
// postcon: discards size count of digits from the number

	if (size <= 0) size = digits.size() - pos;

	while (size) {
		digits.erase(digits.begin() + pos);
		size--;
	}
}

string big_integer::output() {
	string result;

	for (int i = 0; i < digits.size(); i++)
		result += '0' + digits[i];

	if (negative) result += '-';

	return result;
 }
