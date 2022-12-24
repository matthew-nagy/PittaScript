#pragma once
#include <stdio.h>
#include<memory>

namespace pitta {

	template<class T>
	class shared_data {
	public:

		enum class usage_flag : char{
			Internal_Usage,
			External_Usage
		};

		T* operator->() {
			return ptr;
		}
		T& operator*() {
			return *ptr;
		}
		const T* operator->()const {
			return ptr;
		}
		const T& operator*()const {
			return *ptr;
		}

		bool operator!=(void* possiblePtr) {
			return ptr != possiblePtr;
		}

		T* get() {
			return ptr;
		}
		const T* get()const {
			return ptr;
		}

		shared_data<T>& operator=(const shared_data<T>& other) {
			decrimentIfInUse();
			ptr = other.ptr;
			usageCount = other.usageCount;
			if (usageCount != nullptr)
				(*usageCount) += 1;
			return *this;
		}

		shared_data():
			ptr(nullptr),
			usageCount(nullptr)
		{}
		shared_data(const T& data):
			ptr(new T(data)),
			usageCount(new size_t(1))
		{}
		shared_data(T&& data):
			ptr(new T(std::move(data))),
			usageCount(new size_t(1))
		{}
		shared_data(T* data, usage_flag usage = usage_flag::Internal_Usage) :
			ptr(data),
			usageCount(usage == usage_flag::Internal_Usage ? new size_t(1) : nullptr)
		{}
		shared_data(const shared_data& copy) :
			ptr(copy.ptr),
			usageCount(copy.usageCount)
		{
			if (usageCount != nullptr)
				(*usageCount) += 1;
		}
		shared_data(shared_data&& move):
			ptr(move.ptr),
			usageCount(move.usageCount)
		{
			move.ptr = nullptr;
			move.usageCount = nullptr;
		}

		~shared_data() {
			decrimentIfInUse();
		}

	private:
		T* ptr;
		size_t* usageCount;//If nullptr, then there is no usage, pitta does not own this data

		void decrimentIfInUse() {
			if (usageCount != nullptr) {
				(*usageCount) -= 1;
				if (*usageCount == 0) {
					delete usageCount;
					delete ptr;
				}
			}
		}
	};

	template<class T, class... Args>
	shared_data<T> make_shared_data(Args&&... args) {
		shared_data<T> data(new T(std::move(args)...));
		return data;
	}
}