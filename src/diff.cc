#include "diff.h"
#include <algorithm>
#include <iostream>

namespace diff
{

	typedef std::vector<size_t> line_numbers_t;
	struct doc_lines_t
	{
		line_numbers_t a_positions;
		line_numbers_t b_positions;
	};
	typedef std::map<diff::cache_t::string_id_t, doc_lines_t> string_id_positions_t;

	static int comp_abs (size_t key, size_t const& offset, size_t const& node)
	{
		 return key < offset ? -1 : (key == offset ? 0 : +1);
	}

	static void insert (cache_t& cache, string_id_positions_t& positions, cache_t::tree_t& tree, line_numbers_t doc_lines_t::*field, size_t offset, std::vector<std::string> const& lines)
	{
		auto insertPosition = tree.upper_bound(offset, comp_abs);
		for(auto& line : lines)
		{
			auto alreadyThere = cache.stringToId.find(line);
			if(alreadyThere == cache.stringToId.end())
				alreadyThere = cache.stringToId.emplace(line, cache.identity++).first;
			insertPosition = ++tree.insert(insertPosition, 1, alreadyThere->second);
		}
		for(auto& lineId : tree)
			(positions[lineId.value].*field).push_back(lineId.offset);
	}

	static string_id_positions_t setup (cache_t& cache, std::vector<std::string> const& linesA, std::vector<std::string> const& linesB)
	{
		string_id_positions_t lines;
		insert(cache, lines, cache.line_position_to_id_A, &doc_lines_t::a_positions, 0, linesA);
		insert(cache, lines, cache.line_position_to_id_B, &doc_lines_t::b_positions, 0, linesB);
		return lines;
	}

	static auto lone_element_in_range (line_numbers_t const& vector, size_t low, size_t high)
	{
		auto lowerBound = std::lower_bound(vector.begin(), vector.end(), low);
		if(lowerBound == vector.end() || *lowerBound >= high)
			return vector.end();
		++lowerBound;
		if(lowerBound != vector.end() && *lowerBound < high)
			return vector.end();
		return --lowerBound;
	}

	static std::vector<diff::position_t> unique (cache_t& cache, string_id_positions_t& positions, size_t lowA, size_t lowB, size_t highA, size_t highB)
	{
		size_t range = highA - lowA;
		auto lineId = cache.line_position_to_id_A.find(lowA, comp_abs);

		auto lines = positions.find(lineId->value);

		std::map<size_t, size_t> b_to_a;
		while(range-- > 0)
		{
			if(lines != positions.end() && lines->first != lineId->value) {
				lines = positions.find(lineId->value);
			}
			auto tempLines = lines++;
			++lineId;
			auto iterB = lone_element_in_range(tempLines->second.b_positions, lowB, highB);
			if(iterB == tempLines->second.b_positions.end())
				continue;
			auto iterA = lone_element_in_range(tempLines->second.a_positions, lowA, highA);
			if(iterA == tempLines->second.a_positions.end())
				continue;
			b_to_a[*iterB] = *iterA;
		}

		// patience sort
		std::map<size_t, diff::position_t> backpointer;
		std::vector<diff::position_t> topOfPiles;
		// use SIZE_MAX as backpointer to leftmost values
		// values increase with index increase except for first
		topOfPiles.emplace_back(SIZE_MAX, SIZE_MAX);
		for(auto it : b_to_a)
		{
			auto insert_at = std::upper_bound(topOfPiles.begin()+1, topOfPiles.end(), it.second, [](size_t const key, diff::position_t const& position){ return key < position.a_pos;});

			if(insert_at == topOfPiles.end())
				insert_at = topOfPiles.insert(insert_at, diff::position_t{it.second, it.first});
			else
				*insert_at = diff::position_t{it.second, it.first};
			backpointer[it.first]=*(--insert_at);
		}

		// using topOfPiles as result vector
		auto& item = topOfPiles.back();
		size_t index = topOfPiles.size() - 1;
		while(item.b_pos != SIZE_MAX)
			item = backpointer[(topOfPiles[--index] = item).b_pos];
		topOfPiles.resize(topOfPiles.size() - 1);
		return topOfPiles;
	}

	static void recurse (cache_t& cache, string_id_positions_t& positions, std::vector<std::string> const& linesA, std::vector<std::string> const& linesB, size_t lowA, size_t lowB, size_t highA, size_t highB, std::vector<diff::position_t >& matches)
	{
		if(lowA == highA || lowB == highB)
			return;
		auto uniques = unique(cache, positions, lowA, lowB, highA, highB);
		if(uniques.size() > 0)
		{
			// matches are treated range markers, we need an end marker to close the range the last match opened
			// e.g. [lastMatch, highA)
			uniques.emplace_back(highA, highB);

			diff::position_t previousPosition{lowA, lowB};
			for(auto pos : uniques)
			{
				recurse(cache, positions, linesA, linesB, previousPosition.a_pos, previousPosition.b_pos, pos.a_pos, pos.b_pos, matches);
				previousPosition = pos;
				matches.push_back(previousPosition++);
			}
			// pop off the last match, which corresponded to the end position.
			matches.pop_back();
		}
		else if (linesA[lowA] == linesB[lowB])
		{
			// find match from beginning
			while(lowA < highA && lowB < highB && linesA[lowA] == linesB[lowB])
				matches.emplace_back(lowA++, lowB++);
			recurse(cache, positions, linesA, linesB, lowA, lowB, highA, highB, matches);
		}
		else if (linesA[--highA] == linesB[--highB])
		{
			size_t oldHighA = highA;
			// find match from end
			while(highA > lowA && highB > lowB && linesA[--highA] == linesB[--highB])
				;// don't add matches yet
			recurse(cache, positions, linesA, linesB, lowA, lowB, highA, highB, matches);
			// add end matches
			while(highA != oldHighA)
				matches.emplace_back(highA++, highB++);
		}
	}

	std::vector<diff::position_t> diff (std::vector<std::string> const& linesA, std::vector<std::string> const& linesB)
	{
		cache_t cache;
		auto lines = setup(cache, linesA, linesB);
		std::vector<diff::position_t> matches;
		recurse(cache, lines, linesA, linesB, 0, 0, linesA.size(), linesB.size(), matches);
		return matches;
	}
}
