#pragma once
#include <memory>
#include <limits>
#include <cassert>


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

template<typename T>
class CAllocator
{
private:
	enum Op {
		PLUS = 1,
		MINUS
	};
public:
	using value_type = T;
	
	using pointer = T*;
	using const_pointer = const T*;
	
	using void_pointer = void*;
	using const_void_pointer = const void*;

	using reference = T&;
	using const_reference = const T&;
	
	using size_type = size_t;
	using difference_ptr = std::ptrdiff_t;

	template<typename U>
	struct rebind {
		using other = CAllocator<U>;
	};
public:

	inline CAllocator() : currentSize(0), peakSize(0) {}
	inline ~CAllocator() = default;

	inline CAllocator(const CAllocator& other) {
		this->totalAllocSize = other.TotalAllocations();
		this->currentSize = other.CurrentAllocations();
		this->peakSize = other.PeakAllocations();
	}


	template<typename U>
	inline CAllocator(const CAllocator<U>& other) {
		this->totalAllocSize = other.TotalAllocations();
		this->currentSize = other.CurrentAllocations();
		this->peakSize = other.PeakAllocations();
	}
	
	inline pointer allocate(size_type numObjects) {
		update(numObjects, Op::PLUS);
		return reinterpret_cast<pointer>(operator new(sizeof(T) * numObjects));
	}

	inline pointer allocate(size_type numObjects, const_void_pointer hint) {
		return allocate(numObjects);
	}

	inline void deallocate(pointer p, size_type numObjects) {
		update(numObjects, Op::MINUS);
		operator delete(p);
	}

	template<typename U, typename... Args>
	void construct(U* p, Args&&...args) {
		new(p) U(std::forward<Args>(args)...);
	}

	template<typename U>
	void destroy(U* p) {
		p->~U();
	}

	inline pointer address(reference value) const {
		return &value;
	}

	inline const_pointer address(const_reference value) const {
		return &value;
	}

	inline size_type mas_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}


	inline size_type TotalAllocations() const { return this->totalAllocSize; }
		
	inline size_type CurrentAllocations() const { return this->currentSize; }
		
	inline size_type PeakAllocations() const { return this->peakSize; }
		

private:
	static size_type totalAllocSize;
	size_type currentSize;
	size_type peakSize;

private:
#pragma warning(push)
#pragma warning(disable : 26812)
	void update(size_type size, Op op) {
		if (op == Op::PLUS) {
			this->totalAllocSize += size;
			this->currentSize += size;
			if (this->currentSize > this->peakSize) {
				this->peakSize = this->currentSize;
			}
		}
		else 
			this->currentSize -= size;	
	}
#pragma warning(pop)

};

template<typename T>
typename CAllocator<T>::size_type CAllocator<T>::totalAllocSize = 0;


template<typename T, typename U>
bool operator==(const CAllocator<T>& lhs, const CAllocator<U>& rhs) {
	return operator==(static_cast<T&>(lhs), static_cast<U&>(rhs));
	
}

template<typename T, typename U>
bool operator!=(const CAllocator<T>& lhs, const CAllocator<U>& rhs) {
	return operator!=(static_cast<T&>(lhs), static_cast<U&>(rhs));
}

template<typename T>
bool operator==(const CAllocator<T>& lhs, const CAllocator<T>& rhs) {
	return operator==((lhs), (rhs));

}

template<typename T>
bool operator!=(const CAllocator<T>& lhs, const CAllocator<T>& rhs) {
	return operator!=((lhs), (rhs));
}


namespace MemoryPool {
	using size_type = size_t;

	template<typename T, size_type Block_Size = 1'048'576u>
	class PoolAllocator
	{
	public:
		using value_type = T;

		using pointer = T*;
		using const_pointer = const T*;

		using void_pointer = void*;
		using const_void_pointer = const void*;

		using reference = T&;
		using const_reference = const T&;

		using size_type = size_t;
		using difference_ptr = std::ptrdiff_t;

		template<typename U>
		struct rebind {
			using other = PoolAllocator<U>;
		};

	private:
		union Bank
		{
			value_type element;
			Bank* next;
		};

		using data_ptr = char*;
		using bank_type = Bank;
		using bank_pointer = Bank*;
		using const_bank_pointer = const Bank*;
		using bank_reference = Bank&;
		using const_bank_reference = const Bank&;
		static_assert(Block_Size >= 2 * sizeof(bank_type), "Error: small block size!");
	public:
		
		PoolAllocator() : currentBlock(nullptr), currentBank(nullptr), lastBank(nullptr), freeBank(nullptr){

		}
		~PoolAllocator() noexcept { clear(); }


		PoolAllocator(const PoolAllocator<T>& other) { PoolAllocator(); }

		template<typename U>
		PoolAllocator(const PoolAllocator<U>& other) { PoolAllocator(); }

		PoolAllocator& operator=(const PoolAllocator& other) = delete;

		PoolAllocator& operator=(const PoolAllocator&& other) noexcept {
			if (this != &other) {
				currentBank = std::move(other.currentBank);
				lastBank = std::move(other.lastBank);
				freeBank = std::move(other.freeBank);
				std::swap(currentBlock, other.currentBlock);
			}
			return *this;
		}
		PoolAllocator(const PoolAllocator&& other) noexcept {
			if (this != &other) {
				currentBlock = std::move(other.currentBlock);
				currentBank = std::move(other.currentBank);
				lastBank = std::move(other.lastBank);
				freeBank = std::move(other.freeBank);
				other.currentBlock = nullptr;
			}
		}

		pointer address(reference ptr) const noexcept { return &ptr; }
		const_pointer address(const_reference ptr) const noexcept { return &ptr; }
		pointer address(void_pointer ptr) const noexcept { return &ptr; }
		const_pointer address(const_void_pointer ptr) const noexcept { return &ptr; }

		pointer allocate(size_type numObjs = 1, const_pointer hint = 0) {
			if (freeBank != nullptr) {
				pointer res = reinterpret_cast<pointer>(freeBank);
				freeBank = freeBank->next;
				return res;
			}
			else {
				if (currentBlock >= lastBank)
					allocateBlock();
				return reinterpret_cast<pointer>(currentBank++);

			}
		}

		void deallocate(pointer ptr, size_type numObjs = 1) {
			if (ptr != nullptr) {
				reinterpret_cast<bank_pointer>(ptr)->next = freeBank;
				freeBank = reinterpret_cast<bank_pointer>(ptr);
			}
		}


		template <typename U, typename... Args>
		void construct(U* ptr, Args&&... args) {
			new (ptr) U(std::forward<Args>(args)...);
		}

		template <typename U>
		void destroy(U* ptr) {
			ptr->~U();
		}

		template <typename... Args>
		pointer allocateElement(Args&&... args) {
			pointer res = allocate();
			construct<value_type>(res, std::forward<Args>(args)...);
			return res;
		}
		
		void deallocateElement(pointer ptr) {
			if (ptr != nullptr) {
				ptr->~value_type();
				deallocate(ptr);
			}
		}

		size_type max_size() const noexcept {
			size_type maxBlocks = -1 / Block_Size;
			return (Block_Size - sizeof(data_ptr)) / sizeof(bank_type) * maxBlocks;
		}

	private:
		
		bank_pointer currentBlock;
		bank_pointer currentBank;
		bank_pointer lastBank;
		bank_pointer freeBank;

		size_type calcPadding(data_ptr ptr, size_type align) noexcept {
			uintptr_t res = reinterpret_cast<uintptr_t>(ptr);
			return ((align - res) % align);
		}
		void allocateBlock();
		void clear() {
			auto current = currentBlock;
			while (current != nullptr) {
				auto previous = current->next;
				delete(reinterpret_cast<const_void_pointer>(current));
				current = previous;
			}
		}

	};


	template<typename T, size_type Block_Size>
	void PoolAllocator<T, Block_Size>::allocateBlock() {
		data_ptr newBlock = reinterpret_cast<data_ptr>(::operator new(Block_Size));
		reinterpret_cast<bank_pointer>(newBlock)->next = currentBlock;
		currentBlock = reinterpret_cast<bank_pointer>(newBlock);
		
		data_ptr block = newBlock + sizeof(bank_pointer);
		size_type blockPadding = calcPadding(block, alignof(bank_type));
		currentBank = reinterpret_cast<bank_pointer>(block + blockPadding);
		lastBank = reinterpret_cast<bank_pointer>(newBlock + Block_Size - sizeof(bank_type) + 1);
	}

}
