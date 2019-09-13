#pragma once

#include <pthread.h>
#include <cerrno>// needed for EBUSY (for some reason its not in pthread)
#include <ctime>
#include <cstdio>
#include <cassert>

#define PRINT_LOCK_AQUISITIONS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
const timespec quater_sec = {0, 200000000};
#pragma GCC diagnostic pop

class mutex {
	public:
		pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
#ifndef OPTIMIZE_LOCK
		const char *what, *why_locked;
		mutex(const char *name) : what(name) {};
#else
		mutex(const char *name) {};
#endif


		inline void lock(const char *why) {
#ifndef OPTIMIZE_LOCK
			if(!pthread_mutex_timedlock(&mux, &quater_sec)) {
				printf("Lock %s for %s failed (already locked by: %s), waiting\n",
						what, why, why_locked);
#endif
				assert(!pthread_mutex_lock(&mux));
#ifndef OPTIMIZE_LOCK
#ifdef PRINT_LOCK_AQUISITIONS
				printf("lock %s acquired for %s\n", what, why);
#endif
			}
			why_locked = why;
#endif
		}

		inline void unlock() {
			assert(!pthread_mutex_unlock(&mux));
		}

		inline bool trylock(const char *why) {
#ifndef OPTIMIZE_LOCK
			switch(pthread_mutex_trylock(&mux)) {
				case 0:// we locked
					why_locked = why;
#ifdef PRINT_LOCK_AQUISITIONS
				printf("lock %s acquired for %s\n", what, why);
#endif
					return true;

				default:
				case EBUSY:// its already busy
					return false;
			}
#else
			return pthread_mutex_trylock(&mux) != EBUSY;
#endif
		}

};
