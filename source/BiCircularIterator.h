#ifndef BI_CIRCULAR_ITERATOR_H_
#define BI_CIRCULAR_ITERATOR_H_

#include <iterator>



template <class Iter>
class BiCircularIterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = typename Iter::difference_type;
	using value_type = typename Iter::value_type;
	using pointer = typename Iter::pointer;
	using reference = typename Iter::reference;


public:
	BiCircularIterator(const Iter &pos, const Iter &begin, const Iter &end)
		: value(pos), begin(begin), end(end)
	{
	}

	value_type &operator*() { return *value; }
	const value_type &operator*() const { return *value; }

	bool operator==(const BiCircularIterator &other) const { return value == other.value; }
	bool operator!=(const BiCircularIterator &other) const { return !operator==(other); }

	// Prefix operators.
	BiCircularIterator &operator++()
	{
		++value;
		if(value == end)
			value = begin;
		return *this;
	}
	BiCircularIterator &operator--()
	{
		if(value == begin)
			value = end;
		--value;
		return *this;
	}
	// Postfix operators.
	BiCircularIterator operator++(int)
	{
		BiCircularIterator temp(*this);
		++*this;
		return temp;
	}
	BiCircularIterator operator--(int)
	{
		BiCircularIterator temp(*this);
		--*this;
		return temp;
	}


private:
	Iter value;
	Iter begin;
	Iter end;
};

#endif
