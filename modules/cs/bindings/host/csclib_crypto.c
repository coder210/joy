/* C# -> joy glue: hand-written native binding for MD5 (crypto bridge).
 *
 * joy_core_utils_md5x is level:X in the IDL (char* in/out, size_t) so the
 * generator skips it. This bridge exposes it as a simple string-in/string-out
 * native call so C# can compute the exact same MD5 as the C++ frame-sync
 * checksum (utils_md5x), without reimplementing the algorithm in managed code.
 *
 * CoreCLR 函数指针表绑定：C# 侧传 UTF-8 的 const char*，返回 UTF-8 指针；
 * C# 用 Marshal.PtrToStringUTF8 拷贝。不再用 MonoString/mono_string_new。
 *
 * ASCII-only comments: MSVC reads UTF-8-without-BOM as the local codepage.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "joy_csharp.h"
#include <joy/joy.h>

/* md5x: input bytes -> 32-char lowercase hex (writes into out, returns 32).
 * Caller passes a C# UTF-8 string; checksum strings are ASCII (digits,:,-,letters). */
static const char* w_md5_hex(const char* input)
{
        static char out[64];
        if (!input) return NULL;
        size_t len = strlen(input);

        memset(out, 0, sizeof(out));
        joy_core_utils_md5x(input, len, out);
        return out;  /* C# 侧立即 Marshal.PtrToStringUTF8 拷贝，无需释放 */
}

void csopen_crypto(void)
{
        csx_register_internal_call("Joy.Api.CryptoExt::md5_hex", (const void*)w_md5_hex);
}
