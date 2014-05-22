#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include "diff.h"
#include <algorithm>

std::vector<std::string> linesFromFile (char const* fileName)
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

int main (int argc, char const* argv[])
{
	if(argc != 3)
		return -1;
	
  std::vector<std::string> linesA = linesFromFile(argv[1]);
  std::vector<std::string> linesB = linesFromFile(argv[2]);

  std::copy(linesA.begin(), linesA.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
	auto result = diff::diff(linesA, linesB);
	printDiff(linesA, linesB, result);
}