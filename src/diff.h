#ifndef DIFF_H_L7035AOI
#define DIFF_H_L7035AOI

#include <stddef.h>
#include <map>
#include <vector>
#include <string>
#include "basic_tree.h"

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
		struct string_id_t
		{
			string_id_t ():_id(0) {}
			explicit string_id_t (size_t id):_id(id) {}
			bool operator< (string_id_t const& rhs) const { return _id < rhs._id; }
			bool operator!= (string_id_t const& rhs) const { return _id != rhs._id; }
			string_id_t operator++ (int) { return string_id_t(_id++); }

			size_t _id;
		};
		typedef oak::basic_tree_t<size_t, string_id_t> tree_t;
		tree_t line_position_to_id_A;
		tree_t line_position_to_id_B;
		
		string_id_t identity;
		std::map<std::string, string_id_t> stringToId;
	};

	std::vector<position_t> diff(std::vector<std::string> const& linesA, std::vector<std::string> const& linesB);
} /* diff */

#endif /* end of include guard: DIFF_H_L7035AOI */