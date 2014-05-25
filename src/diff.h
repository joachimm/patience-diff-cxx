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
			bool operator== (string_id_t const& rhs) const { return _id == rhs._id; }
			string_id_t operator++ (int) { return string_id_t(_id++); }
			std::string to_s () { return std::to_string(_id); }
		private:
			size_t _id;
		};
		struct string_node_t;
		typedef std::unique_ptr<string_node_t> string_node_ptr;
		typedef oak::basic_tree_t<size_t, string_node_ptr> tree_t;
		struct string_node_t
		{
			string_node_t(tree_t::iterator previous, string_id_t identity) : _previous(previous), _id(identity) {}
			tree_t::iterator _previous;
			string_id_t _id;
		};
		tree_t line_position_to_id_A;
		tree_t line_position_to_id_B;
		
		string_id_t identity;
		std::map<std::string, string_id_t> stringToId;
		typedef std::map<string_id_t, tree_t::iterator> top_link_t;
		top_link_t previousA;
		top_link_t previousB;
	};
	void insert (cache_t& cache, size_t offset, std::vector<std::string> const& lines);
	void remove (cache_t& cache, size_t offset, std::vector<std::string> const& lines);
	cache_t updateable_diff (std::vector<std::string> const& linesA, std::vector<std::string> const& linesB);
	std::vector<position_t> update (cache_t& cache);
	std::vector<position_t> diff (std::vector<std::string> const& linesA, std::vector<std::string> const& linesB);
} /* diff */

#endif /* end of include guard: DIFF_H_L7035AOI */