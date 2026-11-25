#ifndef JOY_CS_ENTRY_H
#define JOY_CS_ENTRY_H

/* native 宿主与 JoyGame.dll 之间的入口名契约。
 * C# 侧对应 JoyGame.JoyEntry；改这里即可改名，不需要翻 jcsx.c / main.cpp 的字面量。 */
#define JOY_CS_NAMESPACE         "JoyGame"
#define JOY_CS_ASSEMBLY          "JoyGame"
#define JOY_CS_ASSEMBLY_FILE     "JoyGame.dll"
#define JOY_CS_ENTRY_CLASS       "JoyEntry"
#define JOY_CS_ENTRY_QUALIFIED   JOY_CS_NAMESPACE "." JOY_CS_ENTRY_CLASS ", " JOY_CS_ASSEMBLY

#define JOY_CS_BOOTSTRAP         "Bootstrap"
#define JOY_CS_BOOTSTRAP_MONO    "BootstrapMono"
#define JOY_CS_ON_INIT           "OnInit"
#define JOY_CS_ON_FRAME          "OnFrame"
#define JOY_CS_ON_SHUTDOWN       "OnShutdown"

#endif /* JOY_CS_ENTRY_H */
