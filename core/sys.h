#ifndef LIBGO_CORE_SYS_H_
#define LIBGO_CORE_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

void setup_sys(void);
int sys_ncpu(void);
int sys_spawn(void(*)(), int);
int sys_tid(void);

#ifdef __cplusplus
};
#endif

#endif // LIBGO_CORE_SYS_H_
