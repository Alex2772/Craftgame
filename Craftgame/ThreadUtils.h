#pragma once
#include <mutex>
#include <atomic>
#include <shared_mutex>
#include <map>
#include <unordered_map>

// https://codereview.stackexchange.com/questions/161735/recursive-shared-mutex
class RecursiveSharedMutex {
public:
	/**
	* @brief Constructs the mutex.
	*/
	RecursiveSharedMutex();

	/**
	* @brief Locks the mutex for exclusive write access for this thread.
	*              Blocks execution as long as write access is not available:
	*              * other thread has write access
	*              * other threads try to get write access
	*              * other threads have read access
	*
	*              A thread may call lock repeatedly.
	*              Ownership will only be released after the thread makes a matching number of calls to unlock.
	*/
	void lock();

	/**
	* @brief Locks the mutex for sharable read access.
	*              Blocks execution as long as read access is not available:
	*              * other thread has write access
	*              * other threads try to get write access
	*
	*              A thread may call lock repeatedly.
	*              Ownership will only be released after the thread makes a matching number of calls to unlock_shared.
	*/
	void lock_shared();

	/**
	* @brief Unlocks the mutex for this thread if its level of ownership is 1. Otherwise reduces the level of ownership
	*              by 1.
	*/
	void unlock();

	/**
	* @brief Unlocks the mutex for this thread if its level of ownership is 1. Otherwise reduces the level of ownership
	*              by 1.
	*/
	void unlock_shared();

private:
	/// Protects data access of mutex.
	std::mutex _mtx;
	/// Number of threads waiting for exclusive write access.
	std::atomic<uint32_t> _waitingWriters;
	/// Thread id of writer.
	std::atomic<std::thread::id> _writerThreadId;
	/// Level of ownership of writer thread.
	uint32_t _writersOwnership;
	/// Level of ownership of reader threads.
	std::unordered_map<std::thread::id, uint32_t> _readersOwnership;
};

/**
 * \brief Класс-обёртка для потокобезопасного доступа к объекту P.
 */
template <typename P>
class ts
{
private:
	typedef P* T;
	T object;
	RecursiveSharedMutex mutex;
	std::atomic_bool block_lock = false;
public:

	/**
	 * \brief Предоставляет read-only доступ к объекту ts<P*>.
	 */
	class r : public std::shared_lock<RecursiveSharedMutex>
	{
	private:
		ts<P>& object;
	public:
		r(ts<P>& t) :
			std::shared_lock<RecursiveSharedMutex>(t.mutex),
			object(t)
		{

		}
		const T operator->() const {
			return object.object;
		}

		const T get() const
		{
			return object.object;
		}
	};

	/**
	* \brief Предоставляет эклюзивный read-write доступ к объекту ts<P*>.
	*/
	class rw : public std::unique_lock<RecursiveSharedMutex>
	{
	private:
		ts<P>& object;
	public:
		rw(ts<P>& t) :
			std::unique_lock<RecursiveSharedMutex>(t.mutex),
			object(t)
		{

		}
		T operator->() const {
			return object.object;
		}
		T get() const
		{
			return object.object;
		}
	};

	ts(T o) :
		object(o)
	{

	}
	ts() :
		object(nullptr)
	{

	}
	~ts()
	{
		rw r(*this);
		if (object)
		{
			delete object;
			object = nullptr;
		}
	}
	/**
	 * \brief Замена объекта. Деструктор старого объекта НЕ ВЫЗЫВАЕТСЯ.
	 */
	void setObject(T o)
	{
		rw r(*this);
		object = o;
	}
	/**
	 * \brief Проверка на существование объекта.
	 */
	bool isValid() const
	{
		return object != nullptr;
	}
	/**
	 * \brief Вызов деструктора объекта.
	 */
	void destruct()
	{
		T temp = object;
		object = nullptr;
		delete temp;
	}

	/**
	* \brief Опасная функция
	* \return Объект
	*/
	T raw()
	{
		return object;
	}
	operator bool() const {
		return isValid();
	}
};
