# gurad_lock and unique_lock

gurad_lock will be lock mutex immediately
unique_lock will provide more flexible, that we can lock and unlock a mutex by your self.
But of course this flexibility is not free, so consider to use guard_lock if you don't acctualy need
the flexibility of unique_lock

Let's look an example to understand why we need these class.
void test() {
mutex.lock();
// do something
// exception raise here
mutex.unlock();
}

You can see in above example, the mutex can be lock forever because this function catch an exception
That's why we need to use RAII for this scenario.
In guard_lock, constructor will be lock the mutex and destructor will be unlock it.
When function break, the destructor will be invoke and our mutex will be unlock too.

# Event Center

Event center is design base on observer pattern

## INetObserver

This is interface class for any listener want to join into event center

## CEvent

CEvent is event center, this maintain all kevent and all handler want to listen these event
