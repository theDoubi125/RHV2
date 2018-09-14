#pragma once

namespace memory
{
	class StackAllocator
	{
	public:
		StackAllocator(const size_t& size, int maxStepCount);
		~StackAllocator();

		void pushStack();
		void popStack();

		template<typename T>
		T* allocate(size_t size)
		{
			T* result = (T*)(m_data + m_dataCursor);
			m_dataCursor += size * sizeof(T);
			return result;
		}

		char* m_data;
		size_t *m_steps;
		int m_currentStep = 0;
		int m_dataCursor = 0;
		size_t m_size;
	};

	namespace allocators
	{
		extern StackAllocator stack;
	}
}