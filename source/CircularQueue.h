#ifndef QUEUE_H_
#define QUEUE_H_

#include "BiCircularIterator.h"

#include <vector>



template <class T>
class CircularQueue {
public:
	typedef BiCircularIterator<typename std::vector<T>::iterator> iterator;
	typedef BiCircularIterator<typename std::vector<T>::const_iterator> const_iterator;


public:
	CircularQueue() = delete;
	explicit CircularQueue(size_t size)
	{
		data.resize(size);
		first = iterator(data.begin(), data.begin(), data.end());
		last = iterator(data.begin(), data.begin(), data.end());
	}

	void clear()
	{
		first = iterator(data.begin(), data.begin(), data.end());
		last = iterator(data.begin(), data.begin(), data.end());
	}

	iterator push_back(const T &item)
	{
		++last;
		*last = item;
		if(last == first)
			++first;
		return last;
	}
	iterator push_front(const T &item)
	{
		--first;
		*first = item;
		if(first == last)
			--last;
		return first;
	}

	template <class... Args>
	iterator emplace_back(const Args ...args) { return push_back(T(args...)); }
	template <class... Args>
	iterator emplace_front(const Args ...args) { return push_front(T(args...)); }

	T pop_front()
	{
		if(first == last)
			return T();
		return *(first++);
	}
	T pop_back()
	{
		if(first == last)
			return T();
		return *(last--);
	}

	iterator begin() { return first; }
	const iterator begin() const { return first; }
	iterator end() { return last; }
	const iterator end() const { return last; }


private:
	std::vector<T> data;
	iterator first = iterator(data.begin(), data.begin(), data.end());
	iterator last = iterator(data.begin(), data.begin(), data.end());
};

#endif
