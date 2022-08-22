/* WeightedList.h
Copyright (c) 2021 by Amazinite

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef WEIGHTED_LIST_H_
#define WEIGHTED_LIST_H_

#include "Random.h"

#include <cstddef>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>



template <typename T>
struct WeightedItem {
	T item;
	std::size_t weight;

	bool operator==(const WeightedItem &rhs) const noexcept
	{
		return weight == rhs.weight && item == rhs.item;
	}
	bool operator!=(const WeightedItem &rhs) const noexcept
	{
		return !(*this == rhs);
	}
};
// Template representing a list of objects of a given type where each item in the
// list is weighted with an integer. This list can be queried to randomly return
// one object from the list where the probability of an object being returned is
// the weight of the object over the sum of the weights of all objects in the list.
template <class Type>
class WeightedList {
public:
	using Item = WeightedItem<Type>;
	using iterator = typename std::vector<Item>::iterator;
	using const_iterator = typename std::vector<Item>::const_iterator;


public:
	template <class T>
	friend std::size_t erase(WeightedList<T> &list, const T &item);
	template <class T, class UnaryPredicate>
	friend std::size_t erase_if(WeightedList<T> &list, UnaryPredicate pred);
	template <typename T, typename U>
	friend bool operator==(const WeightedList<T> &lhs, const WeightedList<U> &rhs);
	template <typename T, typename U>
	friend bool operator!=(const WeightedList<T> &lhs, const WeightedList<U> &rhs);

	const Type &Get() const;
	std::size_t TotalWeight() const noexcept { return total; }

	// Average the result of the given function by the choices' weights.
	template <class Callable>
	auto Average(Callable c) const;
	// Supplying a callable that does not return an arithmetic value will fail to compile.

	iterator begin() noexcept { return items.begin(); }
	const_iterator begin() const noexcept { return items.begin(); }
	iterator end() noexcept { return items.end(); }
	const_iterator end() const noexcept { return items.end(); }

	void clear() noexcept { items.clear(); total = 0; }
	void reserve(std::size_t n) { items.reserve(n); }
	std::size_t size() const noexcept { return items.size(); }
	bool empty() const noexcept { return items.empty(); }
	Type &back() noexcept { return items.back().item; }
	const Type &back() const noexcept { return items.back().item; }

	template <class ...Args>
	Type &emplace_back(int weight, Args&&... args);

	iterator eraseAt(iterator position) noexcept;
	iterator erase(iterator first, iterator last) noexcept;


private:
	void RecalculateWeight();


private:
	std::vector<Item> items;
	std::size_t total = 0;

	friend class FleetEditor;
	friend class SystemEditor;
};



template <class T>
std::size_t erase(WeightedList<T> &list, const T &item)
{
	return erase_if(list, [&item](const T &t) noexcept -> bool { return item == t; });
}



template <class T, class UnaryPredicate>
std::size_t erase_if(WeightedList<T> &list, UnaryPredicate pred)
{
	std::size_t erased = 0;
	unsigned available = list.items.size() - 1;
	for(unsigned index = 0; index < list.items.size() - erased; ++index)
		if(pred(list.items[index].item))
		{
			while(index != available && pred(list.items[available].item))
			{
				--available;
				++erased;
			}
			if(index != available)
			{
				list.items[index] = std::move(list.items[available]);
				--available;
			}
			++erased;
		}

	list.items.erase(std::prev(list.items.end(), erased), list.items.end());
	list.RecalculateWeight();

	return erased;
}



template <typename T, typename U>
bool operator==(const WeightedList<T> &lhs, const WeightedList<U> &rhs)
{
	return lhs.items == rhs.items;
}



template <typename T, typename U>
bool operator!=(const WeightedList<T> &lhs, const WeightedList<U> &rhs)
{
	return !(lhs == rhs);
}



template <class Type>
const Type &WeightedList<Type>::Get() const
{
	if(empty())
		throw std::runtime_error("Attempted to call Get on an empty weighted list.");

	unsigned index = 0;
	for(unsigned choice = Random::Int(total); choice >= items[index].weight; ++index)
		choice -= items[index].weight;

	return items[index].item;
}



template <class Type>
template <class Callable>
auto WeightedList<Type>::Average(Callable fn) const
{
	auto sum = std::invoke_result_t<Callable, const Type &>{};

	std::size_t tw = TotalWeight();
	if (tw == 0) return sum;

	for(unsigned index = 0; index < items.size(); ++index)
		sum += std::invoke(fn, items[index].item) * items[index].weight;
	return static_cast<decltype(sum)>(sum / tw);
}



template <class Type>
template <class ...Args>
Type &WeightedList<Type>::emplace_back(int weight, Args&&... args)
{
	// All weights must be >= 1.
	if(weight < 1)
		throw std::invalid_argument("Invalid weight inserted into weighted list. Weights must be >= 1.");

	items.emplace_back(Item{Type(std::forward<Args>(args)...), static_cast<std::size_t>(weight)});
	total += items.back().weight;
	return items.back().item;
}



template <class Type>
auto WeightedList<Type>::eraseAt(iterator position) noexcept -> iterator
{
	total -= position->weight;
	return items.erase(position);
}



template <class Type>
auto WeightedList<Type>::erase(iterator first, iterator last) noexcept -> iterator
{
	auto it = items.erase(first, last);
	RecalculateWeight();
	return it;
}



template <class Type>
void WeightedList<Type>::RecalculateWeight()
{
	total = std::reduce(items.begin(), items.end(), 0, [](int lhs, const auto &rhs) { return lhs + rhs.weight; });
}



#endif
