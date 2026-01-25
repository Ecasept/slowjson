#ifdef _WIN32
    #include <windows.h>
    #define map_memory(size) (VirtualAlloc(NULL, (size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))
    #define unmap_memory(ptr, size) (VirtualFree((ptr), 0, MEM_RELEASE))
    #define check_mmap_error(ptr) ((ptr) == NULL)
    #define check_unmap_error(ret) ((ret) == 0)
    
    static char* get_mem_err(void) {
        static char error_buf[256];
        DWORD error = GetLastError();
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            error_buf,
            sizeof(error_buf),
            NULL
        );
        return error_buf;
    }
#else
    #include <sys/mman.h>
	#include <errno.h>
	#include <string.h>
    #define map_memory(size) (mmap(NULL, (size), PROT_READ | PROT_WRITE, \
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0))
    #define unmap_memory(ptr, size) (munmap((ptr), (size)))
	#define check_mmap_error(ptr) ((ptr) == MAP_FAILED)
	#define check_unmap_error(ret) ((ret) == -1)
	#define get_mem_err() (strerror(errno))
#endif
