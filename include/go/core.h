#ifndef LIBGO_INCLUDE_GO_CORE_H_
#define LIBGO_INCLUDE_GO_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN32__
#define GO_EXPORT __declspec(dllexport)
#else
#define GO_EXPORT
#endif

typedef void(*Go_Func)();

GO_EXPORT int Go_boot(Go_Func, int, char **);

#ifdef __cplusplus
};
#endif

#endif // LIBGO_INCLUDE_GO_CORE_H_
