#include "rwlock.h"

class rw_lock_nr
{
public:
    rw_lock_nr() {}
    ~rw_lock_nr() {}

    __inline void lock_read() { _lock.lockReader(); }
    __inline void unlock_read() { _lock.unlockReader(); }
    __inline bool try_lock_read() { return _lock.tryLockReader(); }

    __inline void lock_write() { _lock.lockWriter(); }
    __inline void unlock_write() { _lock.unlockWriter(); }
    __inline bool try_lock_write() { return _lock.tryLockWriter(); }

private:
    NonRecursiveRWLock _lock;
};

template <typename T>
class auto_lock
{
public:
    auto_lock(T &lock) : _lock(&lock) { _lock->lock(); }
    ~auto_lock() { _lock->unlock(); }

private:
    T *_lock;

    auto_lock(const auto_lock &);
    auto_lock &operator=(const auto_lock &);
};

class auto_read_lock
{
public:
    auto_read_lock(rw_lock_nr &lock) : _lock(&lock) { _lock->lock_read(); }
    ~auto_read_lock() { _lock->unlock_read(); }

private:
    rw_lock_nr *_lock;
};

class auto_write_lock
{
public:
    auto_write_lock(rw_lock_nr &lock) : _lock(&lock) { _lock->lock_write(); }
    ~auto_write_lock() { _lock->unlock_write(); }

private:
    rw_lock_nr *_lock;
};