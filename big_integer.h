//file:big_integer.h
//big_integer class implementation file
#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <string>
using namespace std;

class big_integer {
public:
    //constructor
    big_integer(string);
    //creates a big_integer by filling the 'digits' vector and assiging the 'negative'
    //value. Starts with the number represented as a string.

    big_integer();
    //default constructor
    //creates empty big_integer

    big_integer add (big_integer other);
    //performs an addition between the current big_integer and the other one.
    //returns a big_integer.

	big_integer subtract (big_integer other);
	//performs a subtraction between the current big_integer and the other one.
	//returns a big_integer.

	big_integer multiply (big_integer other);
	//performs a multiplication between the current big_integer and the other one.
	//returns a big_integer.

 	big_integer div(big_integer other);
	//performs integer division between the current big_integer and the other one.
	//returns a big_integer.

    big_integer mod(big_integer other);
	//performs integer modulo between the current big_integer and the other one.
	//returns a big_integer.

	bool equals(big_integer other);
	//returns True iff the current big_integer equals the other big_integer.

	big_integer absolute();
	//returns the absolute version of the current big_integer

	bool bigger_than(big_integer other);
	//returns True if the current big_integer bigger the other big_integer

	big_integer sub(int pos = 0, int size = 0);
	//returns a big integer from the current integer starting from the pos digit and size
	//count of digits

	string output();
    //produces a string output from the 'digits' and 'negative' features.

private:
    bool negative; //True if the integer is negative.
    vector <int> digits; //A vector that stores the big integer.

    void normalize();
    //deletes leading zoros from the current big_integer

    void discard(int pos = 0, int size = 0);
    //discards size count of digits from the current big_integer
};//end big_integer class
#endif
