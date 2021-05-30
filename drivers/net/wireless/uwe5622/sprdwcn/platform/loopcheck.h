#ifndef _LOOPCHECK
#define _LOOPCHECK

void switch_cp2_log(bool flag);
void get_cp2_version(void);
void start_loopcheck(void);
void stop_loopcheck(void);
int loopcheck_init(void);
int loopcheck_deinit(void);
void complete_kernel_loopcheck(void);
void complete_kernel_atcmd(void);

#endif
