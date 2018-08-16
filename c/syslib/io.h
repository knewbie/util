//
// Created by hujianzhe
//

#ifndef	UTIL_C_SYSLIB_IO_H
#define	UTIL_C_SYSLIB_IO_H

#include "../platform_define.h"

#if defined(_WIN32) || defined(_WIN64)
	struct aiocb {
		union {
		#pragma pack(1)
			struct {
				OVERLAPPED __ol;
			} in;
			struct {
				unsigned char __reserved_align;
				OVERLAPPED __ol;
			} out;
		#pragma pack()
		};
		FD_t aio_fildes;
		void* aio_buf;
		unsigned int aio_nbytes;
		int aio_lio_opcode;
		long long aio_offset;
		struct {
			int sigev_notify;
			int sigev_signo;
			union {
				int sival_int;
				void* sival_ptr;
			} sigev_value;
			void(*sigev_notify_function)(struct aiocb*);
		} aio_sigevent;
	};
	#define	LIO_NOP				0
	#define	LIO_READ			1
	#define	LIO_WRITE			2
	typedef OVERLAPPED_ENTRY	NioEv_t;
	#pragma comment(lib, "wsock32.lib")
	#pragma comment(lib, "ws2_32.lib")
#elif defined(__FreeBSD__) || defined(__APPLE__)
	#include <sys/event.h>
	#include <aio.h>
	#define	LIO_NONE_NOTIFY		SIGEV_NONE
	typedef struct kevent		NioEv_t;
#elif __linux__
	#include <sys/epoll.h>
	#include <aio.h>
	typedef struct epoll_event	NioEv_t;
#endif
struct sockaddr_storage;

#ifdef	__cplusplus
extern "C" {
#endif

/* aiocb */
void aioInit(struct aiocb* cb, size_t udata);
void aioSetOffset(struct aiocb* cb, long long offset);
BOOL aioCommit(struct aiocb* cb);
BOOL aioHasCompleted(const struct aiocb* cb);
BOOL aioSuspend(const struct aiocb* const cb_list[], int nent, int msec);
BOOL aioCancel(FD_t fd, struct aiocb* cb);
int aioResult(struct aiocb* cb, unsigned int* transfer_bytes);
/* reactor */
#define	REACTOR_NOP		0
#define	REACTOR_READ	1
#define	REACTOR_WRITE	2
#define	REACTOR_ACCEPT	3
#define REACTOR_CONNECT	4
typedef struct Reactor_t {
	FD_t __hNio;
#ifdef	__linux__
	FD_t __epfd;
#endif
} Reactor_t;
BOOL reactorCreate(Reactor_t* reactor);
BOOL reactorReg(Reactor_t* reactor, FD_t fd);
BOOL reactorCancel(Reactor_t* reactor, FD_t fd);
BOOL reactorCommit(Reactor_t* reactor, FD_t fd, int opcode, void** p_ol, struct sockaddr_storage* saddr);
int reactorWait(Reactor_t* reactor, NioEv_t* e, unsigned int count, int msec);
void reactorResult(const NioEv_t* e, FD_t* p_fd, int* p_event, void** p_ol);
BOOL reactorConnectCheckSuccess(FD_t sockfd);
BOOL reactorAccept(FD_t listenfd, void* ol, void(*callback)(FD_t, struct sockaddr_storage*, void*), void* arg);
BOOL reactorClose(Reactor_t* reactor);

#ifdef	__cplusplus
}
#endif

#endif
