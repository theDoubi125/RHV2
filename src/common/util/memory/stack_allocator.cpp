#include "stack_allocator.h"
#include <iostream>

namespace memory
{
	StackAllocator::StackAllocator(const size_t& size, int maxStepCount)
	{
		m_data = new char[size];
		m_steps = new size_t[maxStepCount];
		m_steps[0] = 0;
		m_size = size;
	}

	StackAllocator::~StackAllocator()
	{
		delete m_data;
		delete m_steps;
	}

	void StackAllocator::pushStack()
	{
		m_currentStep++;
		m_steps[m_currentStep] = m_dataCursor;
	}

	void StackAllocator::popStack()
	{
		m_dataCursor = m_steps[m_currentStep];
		m_currentStep--;
#ifdef _DEBUG
		if (m_currentStep < 0)
			std::cerr << "Error : pop on empty allocator stack" << std::endl;
#endif
	}

	namespace allocators
	{
		StackAllocator stack(1000000, 1000);
	}
}