#include <algorithm>
#include <iostream>
#include <string>


const int BIT_DEPTH = 32;


void print(std::string number)
{
	for (size_t i = 4; i < number.length(); i += 5)
		number.insert(i, " ");

	std::cout << number << std::endl;
}


std::string add(std::string firstNumber, std::string secondNumber)
{
	while (firstNumber.length() < BIT_DEPTH)
		firstNumber.insert(0, "0");

	while (secondNumber.length() < BIT_DEPTH)
		secondNumber.insert(0, "0");

	std::string result = {};
	bool carry = 0;

	for (int i = firstNumber.size() - 1; i >= 0; --i)
	{
		bool bit1 = firstNumber[i] - '0';
		bool bit2 = secondNumber[i] - '0';
		char sum = (bit1 ^ bit2 ^ carry) + '0';

		result = sum + result;
		carry = (bit1 & carry) | (bit2 & carry) | (bit1 & bit2);
	}

	if (carry && result.length() < BIT_DEPTH)
		result = '1' + result;

	return result;
}


std::string toTwosComplement(std::string number)
{
	while (number.length() < BIT_DEPTH)
		number.insert(0, "0");

	for (auto& currentChar : number)
	{
		if (currentChar == '0')
			currentChar = '1';
		else
			currentChar = '0';
	}

	return add(number, "1");
}


std::string rightShift(std::string& bitArray, size_t bitsNo)
{
	std::string result(BIT_DEPTH * 2 + 1, '0');
	result[0] = bitArray[0];

	for (size_t i = 1; i < bitsNo; i++) {
		result[i] = bitArray[i - 1];
	}

	return result;
}


std::string mul(std::string firstNumber, std::string secondNumber)
{
	std::string result(BIT_DEPTH, '0');
	std::string A(BIT_DEPTH * 2 + 1, '0');
	std::string S(BIT_DEPTH * 2 + 1, '0');
	std::string P(BIT_DEPTH * 2 + 1, '0');
	std::string invertedFirstNumber = toTwosComplement(firstNumber);

	for (size_t i = 0; i < BIT_DEPTH; i++)
	{
		A[i] = firstNumber[i];
		S[i] = invertedFirstNumber[i];
	}

	for (size_t i = BIT_DEPTH; i < BIT_DEPTH * 2; i++)
		P[i] = secondNumber[i - BIT_DEPTH];

	for (size_t i = 0; i < BIT_DEPTH; i++)
	{
	    if (P[2 * BIT_DEPTH] == '1' && P[2 * BIT_DEPTH - 1] == '0')
			P = add(A, P);

		if (P[2 * BIT_DEPTH] == '0' && P[2 * BIT_DEPTH - 1] == '1')
			P = add(S, P);

		P = rightShift(P, BIT_DEPTH * 2 + 1);
	}

	for (size_t i = BIT_DEPTH * 2; i >= BIT_DEPTH; i--)
		result[i - BIT_DEPTH] = P[i];

	return result;
}


void fromHexToBin(std::string &number)
{
	std::string temp;

	for (char currentChar : number)
	{
		switch (currentChar)
		{
		case '0': temp += "0000"; break;
		case '1': temp += "0001"; break;
		case '2': temp += "0010"; break;
		case '3': temp += "0011"; break;
		case '4': temp += "0100"; break;
		case '5': temp += "0101"; break;
		case '6': temp += "0110"; break;
		case '7': temp += "0111"; break;
		case '8': temp += "1000"; break;
		case '9': temp += "1001"; break;
		case 'A': temp += "1010"; break;
		case 'B': temp += "1011"; break;
		case 'C': temp += "1100"; break;
		case 'D': temp += "1101"; break;
		case 'E': temp += "1110"; break;
		case 'F': temp += "1111"; break;
		case '-': temp += "-"; break;
		}
	}
	number = temp;
}


int main()
{
	std::string firstNumber;
	std::string secondNumber;

	std::getline(std::cin, firstNumber);
	std::getline(std::cin, secondNumber);

	firstNumber.erase(std::remove(firstNumber.begin(), firstNumber.end(), ' '), firstNumber.end());
	secondNumber.erase(std::remove(secondNumber.begin(), secondNumber.end(), ' '), secondNumber.end());

	fromHexToBin(firstNumber);
	fromHexToBin(secondNumber);

	if (firstNumber[0] == '-')
	{
		firstNumber.erase(0, 1);
		firstNumber = toTwosComplement(firstNumber);
	}

	if (secondNumber[0] == '-')
	{
		secondNumber.erase(0, 1);
		secondNumber = toTwosComplement(secondNumber);
	}

	while (firstNumber.length() < BIT_DEPTH)
		firstNumber.insert(0, "0");

	while (secondNumber.length() < BIT_DEPTH)
		secondNumber.insert(0, "0");

	std::cout << "First number in binary form:  ";
	print(firstNumber);
	std::cout << "Second number in binary form: ";
	print(secondNumber);

	std::cout << "Sum: ";
	print(add(firstNumber, secondNumber));
	print(mul(firstNumber, secondNumber));

	return 0;
}