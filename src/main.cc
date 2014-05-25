#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include "diff.h"

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::vector<std::string> linesFromFile (std::string const& fileName)
{
	std::vector<std::string> strings;
	std::ifstream file(fileName);

	if(file.is_open() ) {
	std::string temp;
	while(std::getline(file, temp).good() ) {
		strings.push_back(temp);
	}
}
return strings;
}

void printDiff (std::vector<std::string> const& linesA, std::vector<std::string> const& linesB, std::vector<diff::position_t> result)
{
	auto previous = diff::position_t(0,0);

	for(auto equal : result) {
		for_each(linesB.begin() + previous.b_pos, linesB.begin() + equal.b_pos, [](auto string) {
			std::cout << "-" << string << std::endl;
		});
	  for_each(linesA.begin() + previous.a_pos, linesA.begin() + equal.a_pos, [](auto string) {
			std::cout << "+" << string << std::endl;
		});
		std::cout << "=" << linesA[equal.a_pos] << std::endl;
		previous = equal;
		previous++;
	}

	for_each(linesB.begin() + previous.b_pos, linesB.end(), [](auto string) {
		std::cout << "-" << string << std::endl;
	});
	for_each(linesA.begin() + previous.a_pos, linesA.end(), [](auto string) {
		std::cout << "+" << string << std::endl;
	});
}

void debug (std::vector<std::string> const& linesA, std::vector<std::string> const& linesB, std::vector<diff::position_t> result)
{
	std::cout << "-debug-" << std::endl;
	std::cout << "linesA:" << linesA.size() << std::endl;

	for(auto string : linesA)
		std::cout << string << std::endl;

	std::cout << "linesB:" << linesB.size() << std::endl;
	for(auto string : linesB)
		std::cout << string << std::endl;

	std::cout << "result" << std::endl;
	for(auto equal : result)
		std::cout << equal.a_pos << ":" << equal.b_pos << ", ";
	std::cout << std::endl;
	
}
int main (int argc, char const* argv[])
{
	if(argc < 3)
	{
		std::cout << "Supply at least two files" << std::endl;
		return -1;
	}

	std::vector<std::string> linesA = linesFromFile(argv[1]);
	std::vector<std::string> linesB = linesFromFile(argv[2]);
	auto cache = diff::updateable_diff(linesA, linesB);
	auto result = diff::update(cache);
	for(int i = 3 ; i < argc; i++)
	{
		std::vector<std::string> x = split(argv[i], ':');
		if(x.size() != 5 && x.size() != 3)
		{
			std::cout << "expected format operator:point:sourceFile:start:length operator is + or -" << std::endl;
			std::cout << "expected 3 or 5 arguments got:" << x.size() << std::endl;
			std::cout << "skipping:" << argv[i] << std::endl;
			continue;
		}
		std::string type(x[0]), point(x[1]), sourceFile(x[2]);
		std::vector<std::string> lines = linesFromFile(sourceFile);
		size_t offset = std::stoi(point), start = 0, length = lines.size();
		if(x.size() == 5)
		{
			std::string first(x[3]), len(x[4]);
			size_t tstart = std::stoi(first), tlen = std::stoi(len);
			start = std::max(std::min(length, tstart), 0UL);
			length = std::max(std::min(tlen, length - start), 0UL);
		}
		std::vector<std::string> usedLines;
		std::copy (lines.begin() + start, lines.begin() + start + length, back_inserter(usedLines));
		if(type == "+")
		{
			linesA.insert(linesA.begin() + offset, usedLines.begin(), usedLines.end());
			diff::insert(cache, offset, usedLines);
		}
		else if(type == "-")
		{
			linesA.erase(linesA.begin() + offset, linesA.begin()+offset+length);
			diff::remove(cache, offset, usedLines);
		}
		else
			std::cout << "unknown operator:" << type << " use + or -" << std::endl;
		result = diff::update(cache);
	}
	//debug(linesA, linesB, result);
	printDiff(linesA, linesB, result);
}
