#ifndef DIFF_H_L7035AOI
#define DIFF_H_L7035AOI

#include <stddef.h>
#include <map>
#include <vector>
#include <string>

namespace diff
{
	struct position_t
	{
		size_t a_pos;
		size_t b_pos;
		position_t () : position_t(0,0) {}
		position_t (size_t posA, size_t posB) : a_pos(posA),b_pos(posB) {}
		position_t operator++ (int)
		{
			position_t temp(*this);
			a_pos++; b_pos++; return temp;
		}
	};

	struct cache_t
	{
		std::map<std::string, std::pair<std::vector<size_t>, std::vector<size_t> > > line_positions;
	};

	std::vector<position_t> diff(std::vector<std::string> const& linesA, std::vector<std::string> const& linesB);
} /* diff */

#endif /* end of include guard: DIFF_H_L7035AOI */