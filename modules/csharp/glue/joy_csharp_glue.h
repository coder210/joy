#ifndef JOY_CSHARP_GLUE_H
#define JOY_CSHARP_GLUE_H

#ifdef __cplusplus
extern "C" {
#endif

void csopen_sdl(void);
void csopen_kcp(void);
void csopen_math(void);
void csopen_collision(void);
void csopen_sys(void);
void csopen_utils(void);

#ifdef __cplusplus
}
#endif


#endif // !JOY_CSHARP_GLUE_H
