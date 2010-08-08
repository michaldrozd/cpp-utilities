// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// C++ code to search for given string (plain, UTF-8 or UNICODE) in given process in LINUX.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstring>

using namespace std;


// Function to convert string to hexadecimal format.
string string_to_hex(const string& input) {

	static const char* const lut = "0123456789ABCDEF";

	size_t len = input.length();

	string output;
	output.reserve(2 * len);

	for (size_t i = 0; i < len; ++i) {

		const unsigned char c = input[i];

		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);

	}

	return output;
}


// Function to convert hexadecimal format to string.
string hex_to_string(const string& input) {

	static const char* const lut = "0123456789ABCDEF";

	size_t len = input.length();

	if (len & 1) throw invalid_argument("odd length");

	string output;
	output.reserve(len / 2);

	for (size_t i = 0; i < len; i += 2) {

		char a = input[i];
		const char* p = lower_bound(lut, lut + 16, a);

		if (*p != a) throw invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = lower_bound(lut, lut + 16, b);

		if (*q != b) throw invalid_argument("not a hex digit");

		output.push_back(((p - lut) << 4) | (q - lut));

	}

	return output;
}


// Function to convert string to UNICODE format.
string string_to_unicode(const string& input) {

	size_t len = input.length();

	string output;
	output.reserve(2 * len);

	for (size_t i = 0; i < len; ++i) {

		const unsigned char c = input[i];

		output.push_back(c);
		output.push_back('\0');

	}

	return output;
}

// Function to convert UNICODE format to string.
string unicode_to_string(const string& input) {

	size_t len = input.length();

	if (len & 1) throw invalid_argument("odd length");

	string output;
	output.reserve(len / 2);

	for (size_t i = 0; i < len; i += 2) {

		char a = input[i];
		char b = input[i + 1];

		if (b != '\0') throw invalid_argument("not a unicode digit");

		output.push_back(a);

	}

	return output;
}

// Function to search for given string in given process in Linux.
void search_in_memory(string pid, string str, bool isHex, bool isUnicode) {

	// Open the memory file of the given process in read mode using ifstream class object "memFile".
	ifstream memFile("/proc/" + pid + "/mem", std::ios::binary | std::ios::in);  // std::ios::binary is used to open the file in binary mode and std::ios::in is used to open the file in read mode. 												   // If you want to open the file in write mode, use std::ios::out instead of std::ios::in and if you want to open the file in append mode, use std::ios::app instead of std::ios::in.

	// Check whether the memory file of the given process is opened successfully or not using ifstream class object "memFile".
	if (!memFile) {
		cout << "Error: Unable to open memory file." << endl;
		exit(EXIT_FAILURE);
	}

	// Convert given string into hexadecimal format if it is not already in hexadecimal format and user has specified that it should be converted into hexadecimal format by passing true value for "isHex" argument while calling this function from main() function or any other function from where this function is called by passing arguments as required by this function's prototype declaration defined above main() function or any other function from where this function is called.
	if (!isHex) {
		str = string_to_hex(str);
	}

	// Convert given string into UNICODE format if it is not already in UNICODE format and user has specified that it should be converted into UNICODE format by passing true value for "isUnicode" argument while calling this function from main() function or any other function from where this function is called by passing arguments as required by this function's prototype declaration defined above main() function or any other function from where this function is called.
	if (!isUnicode) {  // If given string is not in UNICODE format, then convert it into UNICODE format using string_to_unicode() function defined above main() function or any other function from where this function is called.
		str = string_to_unicode(str);
	}

	// Get the length of the converted hexadecimal/UNICODE formatted version of the given plain text/UTF-8 formatted version of the given text using size() member method of std::string class on "str" variable which stores the converted hexadecimal/UNICODE formatted version of the given plain text/UTF-8 formatted version of the given text.
	int strLen = str.size();

	// Create a character array with size equal to length of converted hexadecimal/UNICODE formatted version of the given plain text/UTF-8 formatted version of the given text stored in "strLen" variable to store each character read from memory file one by one as we read each character one by one from memory file using getline() member method on ifstream class object "memFile".
	char *chars = new char[strLen];

	// Read each character one by one from memory file using getline() member method on ifstream class object "memFile" and store each character in character array "chars" created above.
	while (memFile.getline(chars, strLen)) {  // Read each character one by one from memory file using getline() member method on ifstream class object "memFile" and store each character in character array "chars" created above.

		// Convert the read characters from memory file into string using std::string class constructor which takes two arguments,
		// first argument is the address of the first element of the character array which stores the read characters from memory file and second argument is the length of the string to be created using std::string class constructor which takes two arguments.
		string str1(chars, strLen);

		if (isHex) {
			str = hex_to_string(str);
		}

		if (isUnicode) {
			str = unicode_to_string(str);
		}

		// Check whether the converted plain text/UTF-8 formatted version of the given string stored in "str" variable matches with the converted plain text/UTF-8 formatted version of the read characters from memory file stored in "str1" variable or not using find() member method on std::string class on "str1" variable which stores the converted plain text/UTF-8 formatted version of the read characters from memory file.
		if (str1.find(str) != std::string::npos) {
			cout << "String found." << endl;
			break;
		}
	}

	// Close memory file opened above using close() member method on ifstream class object "memFile".
	memFile.close();

	// Delete dynamically allocated character array created above to store each character read from memory file one by one as we read each character one by one from memory file using getline() member method on ifstream class object "memFile".
	delete[] chars;
}

int main(int argc, char *argv[]) {

	if (argc != 5) {
		cout << "Error: Invalid number of arguments." << endl;
		exit(EXIT_FAILURE);
	}

	// Convert the third command line argument passed by user into boolean type variable using strcmp() function defined in cstring header file which is included above main() function definition using #include preprocessor directive.
	bool isHex = !strcmp(argv[3], "true");

	// Convert the fourth command line argument passed by user into boolean type variable using strcmp() function defined in cstring header file which is included above main() function definition using #include preprocessor directive.
	bool isUnicode = !strcmp(argv[4], "true");

	// Call search() function defined above main() function definition by passing three arguments, first argument is PID (Process ID) number stored inside argv[1] array element at index 1,
	// second argument is string to search for stored inside argv[2] array element at index 2 and third argument is true or false depending upon whether user wants
	//  to convert given string into hexadecimal format or not before searching for it stored inside argv[3] array element at index 3.
	search_in_memory(argv[1], argv[2], isHex, isUnicode);

	return 0;
}