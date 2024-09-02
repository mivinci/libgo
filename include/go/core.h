#ifndef LIBGO_INCLUDE_GO_CORE_H_
#define LIBGO_INCLUDE_GO_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ARM64_ARCH_8__) || defined(__aarch64__)
#define GOARCH_ARM64 1
#elif defined(__x86_64__) || defined(__amd64__)
#define GOARCH_X86_64 1
#define GOARCH_AMD64 1
#else
#error "Unsupported machine architecture"
#endif /* GOARCH */

#if defined(__linux__) || defined(__linux)
#define GOOS_LINUX 1
#elif defined(__APPLE__)
#define GOOS_APPLE 1
#if defined(__ENVIRONMENT_IOS__) || defined(TARGET_OS_IOS)
#define GOOS_IOS 1
#elif defined(__ENVIRONMENT_MACOS__) || defined(TARGET_OS_MAC)
#define GOOS_MACOS 1
#endif
#elif defined(__WIN32__)
#define GOOS_WINDOWS 1
#else
#error "Unsupported operating system"
#endif /* GOOS */

#ifdef GOOS_WINDOWS
#define GOEXPORT __declspec(dllexport)
#else
#define GOEXPORT
#endif

#define GOSTKMIN 2048
#define GOPTRSIZ sizeof(void *)

#define GOASSERT(e, ...) do {                                  \
  if (!(e)) Go_throw(__FILE__, __LINE__, "assert: "__VA_ARGS__); \
} while (0)

typedef void(*Go_Func)();
GOEXPORT int Go_boot(Go_Func, int, char **) asm("goboot");
GOEXPORT void Go_throw(const char *, int, const char *, ...);
/**
 * Like Go's `go` statement, spawn a goroutine using a function
 * with any number of arguments. The total size of arguments 
 * MUST be equal or less than 256 bytes. 
 * @example
 * ```c
 * void hello(int a, char *p) { ... }
 * Go_spawn(hello, 114514, "man!");
 * ```
 * is identical to
 * ```go
 * func hello(a int, p string) { ... }
 * go hello(114514, "man!")
 * ```
 */
GOEXPORT void Go_spawn(Go_Func, ...) __asm__("gospawn");

#ifdef __cplusplus
};
#endif

#endif // LIBGO_INCLUDE_GO_CORE_H_
