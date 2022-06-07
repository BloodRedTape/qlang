#ifndef CORTEX_UTILS_HPP
#define CORTEX_UTILS_HPP

#include <cstdint>
#include <assert.h>
#include <vector>
#include <iterator>

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

constexpr size_t InvalidIndex = -1;

template<typename T>
class Stack{
private:
	std::vector<T> m_Data;
public:
	void Push(const T &element) {
		m_Data.push_back(element);
	}
	void Push(T &&element) {
		m_Data.push_back(std::move(element));
	}

	void Pop(){
		m_Data.pop_back();
	}
	
	size_t Size()const {
		return m_Data.size();
	}

	T &operator[](size_t index) {
		return m_Data[Size() - 1 - index];
	}

	const T &operator[](size_t index) const{
		return m_Data[Size() - 1 - index];
	}

	auto begin() { return std::make_reverse_iterator(m_Data.end()); }

	auto end() { return std::make_reverse_iterator(m_Data.begin()); }

	auto begin()const{ return std::make_reverse_iterator(m_Data.end()); }

	auto end()const{ return std::make_reverse_iterator(m_Data.begin()); }

};

#endif//CORTEX_UTILS_HPP