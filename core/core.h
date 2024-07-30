#ifndef GMP_CORE_H
#define GMP_CORE_H

#define align_up(n, a) ((n + a - 1) & (~(a - 1)))
#define align_down(n, a) ((n) & (~(a - 1)))

#define len(p) (sizeof(p) / sizeof(p[0]))

#if defined(__ARM64_ARCH_8__) || defined(__aarch64__)
#define Arch_arm64 1
#elif defined(__x86_64__) || defined(__amd64__)
#define Arch_x86_64 1
#elif defined(__riscv)
#define Arch_riscv 1
#else
#error "Unsupported architecture"
#endif /* ARCH */

#if defined(__linux__) || defined(__linux)
#define OS_Linux 1
#elif defined(__APPLE__)
#define OS_Apple 1
#if defined(__ENVIRONMENT_IOS__) || defined(TARGET_OS_IOS)
#define OS_iOS 1
#elif defined(__ENVIRONMENT_MACOS__) || defined(TARGET_OS_MAC)
#define OS_macOS 1
#endif
#else
#error "Unsupported operating system"
#endif /* OS */

#define PTR_SIZE (sizeof(void *))
#define STK_ALIGN 16
#define STK_MIN 2048

#if defined(Arch_arm64)
#define CTX_MAX 80
#elif defined(Arch_riscv)
#define CTX_MAX 88
#elif defined(Arch_x86_64)
#define CTX_MAX 40
#else
#define CTX_MAX 128
#endif

#endif /* GMP_CORE_H */
