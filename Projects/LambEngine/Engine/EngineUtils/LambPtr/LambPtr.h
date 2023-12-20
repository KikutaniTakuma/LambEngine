#pragma once
#include <cstdint>
#include <type_traits>

struct IUnknown;

namespace Lamb {
	template<class T>
	concept IsDirectX = std::is_base_of_v<IUnknown, T>&& requires(T a){
		{ a.~T() } -> std::convertible_to<void>;
	};

	template<IsDirectX T>
	class LambPtr {
	public:
		LambPtr() :
			ptr_{ nullptr },
			refCount_{ new uint32_t{} }
		{
			*refCount_ = 0u;
		}
		LambPtr(T* ptr) :
			LambPtr{}
		{
			*this = ptr;
		}
		LambPtr(const LambPtr<T>& right) :
			LambPtr{}
		{
			*this = right;
		}
		LambPtr(LambPtr&& right) noexcept :
			LambPtr{}
		{
			*this = std::move(right);
		}
		~LambPtr() {
			Delete();
		}

	public:
		LambPtr<T>& operator=(const LambPtr<T>& right) {
			this->Delete();

			this->ptr_ = right.ptr_;
			this->refCount_ = right.refCount_;

			this->AddRef();

			return *this;
		}

		LambPtr<T>& operator=(LambPtr<T>&& right) noexcept {
			this->Delete();

			this->ptr_ = right.ptr_;
			this->refCount_ = right.refCount_;

			this->AddRef();

			return *this;
		}

		LambPtr<T>& operator=(T* right) {
			this->Delete();

			this->ptr_ = right;

			if (ptr_ && !refCount_) {
				refCount_ = new uint32_t{};
			}

			return *this;
		}

		T* operator->() {
			return ptr_;
		}

		T& operator*() {
			return *ptr_;
		}

		T* const operator->() const {
			return ptr_;
		}

		const T& operator*() const {
			return *ptr_;
		}

		explicit operator bool() const {
			return ptr_;
		}

		bool operator!() const {
			return !ptr_;
		}

		bool operator==(T* right) const {
			return ptr_ == right;
		}

		bool operator==(const LambPtr<T>& right) const {
			return ptr_ == right.ptr_;
		}

		bool operator!=(T* right) const {
			return ptr_ != right;
		}

		bool operator!=(const LambPtr<T>& right) const {
			return ptr_ != right.ptr_;
		}

	public:
		bool Empty() const {
			return !(*this);
		}

		T* Get() {
			return ptr_;
		}

		T* const Get() const {
			return ptr_;
		}

		T** GetAddressOf() {
			return &ptr_;
		}

		T* const* GetAddressOf() const {
			return &ptr_;
		}

		void Reset() {
			Delete();
		}

		void Reset(T* ptr) {
			Reset();
			*this = ptr;
		}


	private:
		void Delete() {
			if (ptr_ && refCount_) {
				if (*refCount_ == 0u) {
					ptr_->Release();
					ptr_ = nullptr;
					delete refCount_;
					refCount_ = nullptr;
				}
				else {
					ptr_ = nullptr;
					(*refCount_)--;
					refCount_ = nullptr;
				}
			}
			else if (ptr_) {
				ptr_->Release();
				ptr_ = nullptr;
			}
			else {
				delete refCount_;
				refCount_ = nullptr;
			}
		}

		void AddRef() {
			if (ptr_ && refCount_) {
				(*refCount_)++;
			}
		}


	private:
		T* ptr_;

		uint32_t* refCount_;
	};
}