#ifndef GR_ATOMIC_WRAPPER_H
#define GR_ATOMIC_WRAPPER_H

#include <atomic>

template <typename T>
struct atomicwrapper
{
	std::atomic<T> _a;
	
	atomicwrapper()
			:_a(false)
	{}
	
	atomicwrapper(const std::atomic<T> &a)
			:_a(a.load())
	{}
	
	atomicwrapper(const atomicwrapper &other)
			:_a(other._a.load())
	{}
	
	void operator=(const atomicwrapper &other)
	{
		_a.store(other._a.load());
	}
};

#endif