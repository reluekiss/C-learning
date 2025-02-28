#ifndef COROUTINE_H_
#define COROUTINE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: consider making coroutine.h an stb-style single header library

// Initialize the coroutine runtime. Must be called before using any other
// functions of this API. After the initialization the currently running code is
// considered the main coroutine with the id = 0. Should not be called twice.
// TODO: Allow calling it twice, 'cause why not?!
void coroutine_init(void);

// Finish the currently running coroutine. When called in the main coroutine
// with id = 0 uninitializes the whole runtime of the library, deallocating all
// the memory, and killing all the currently running coroutines.
void coroutine_finish(void);

// Switch to the next coroutine.
void coroutine_yield(void);

// Create a new coroutine. This function does not automatically switch to the
// new coroutine, but continues executing in the current coroutine. The
// execution of the new coroutine will start as soon as the scheduler gets to it
// handling the chains of coroutine_yield()-s.
void coroutine_go(void (*f)(void*), void *arg);

// The id of the current coroutine.
size_t coroutine_id(void);

// How many coroutines are currently alive. Could be used by the main coroutine
// to wait until all the "child" coroutines have died.
size_t coroutine_alive(void);

// Put the current coroutine to sleep until the non-blocking socket `fd` has
// avaliable data to read. Trying to read from fd after coroutine_sleep_read()
// may still cause EAGAIN, if the coroutine was woken up by coroutine_wake_up
// before the socket became available for reading.
void coroutine_sleep_read(int fd);

// Put the current coroutine to sleep until the non-blocking socket `fd` is
// ready to accept data to write. Trying to write to fd after
// coroutine_sleep_write() may still cause EAGAIN, if the coroutine was woken up
// by coroutine_wake_up before the socket became available for writing.
void coroutine_sleep_write(int fd);

// Wake up coroutine by id if it is currently sleeping due to
// coroutine_sleep_read() or coroutine_sleep_write() calls.
void coroutine_wake_up(size_t id);

// TODO: implement sleeping by timeout
// TODO: add timeouts to coroutine_sleep_read() and coroutine_sleep_write()

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // COROUTINE_H_
