#pragma once

#include <pthread.h>
#include <cerrno>// needed for EBUSY (for some reason its not in pthread)
#include <ctime>
#include <cstdio>
#include <cassert>

#define PRINT_LOCK_ACQUISITIONS

const long quarter_sec = 2000000;

class mutex {
	public:
		pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
#ifndef OPTIMIZE_LOCK
		const char *what, *why_locked{""};
		mutex(const char *name) : what(name) {};
#else
		mutex(const char *name) {};
#endif


		inline void lock(const char *why) {
#ifndef OPTIMIZE_LOCK
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 1;
			ts.tv_nsec += quarter_sec;

			int reason = pthread_mutex_timedlock(&mux, &ts);
			if(reason == ETIMEDOUT) {
				printf("Lock '%s' for '%s' failed (already locked by: '%s'), waiting\n",
						what, why, why_locked);
#endif
				assert(!pthread_mutex_lock(&mux));
#ifndef OPTIMIZE_LOCK
#ifdef PRINT_LOCK_ACQUISITIONS
				printf("lock '%s' acquired for '%s'\n", what, why);
#endif
			} else if(reason) {
				printf("lock '%s' can't lock for another reason\n", what);
				perror("");
				assert(!pthread_mutex_lock(&mux));
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
#ifdef PRINT_LOCK_ACQUISITIONS
				printf("lock '%s' acquired for '%s'\n", what, why);
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
