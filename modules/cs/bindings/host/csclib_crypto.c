/* C# -> joy glue: hand-written InternalCall for MD5 (crypto bridge).
 *
 * joy_core_utils_md5x is level:X in the IDL (char* in/out, size_t) so the
 * generator skips it. This bridge exposes it as a simple string-in/string-out
 * InternalCall so C# can compute the exact same MD5 as the C++ frame-sync
 * checksum (utils_md5x), without reimplementing the algorithm in managed code.
 *
 * ASCII-only comments: MSVC reads UTF-8-without-BOM as the local codepage.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>   /* mono_get_root_domain */
#include <mono/utils/mono-publib.h>
#include "joy_csharp.h"
#include <joy/joy.h>

/* md5x: input bytes -> 32-char lowercase hex (writes into out, returns 32).
 * Caller passes a C# string; checksum strings are ASCII (digits,:,-,letters). */
static MonoString* w_md5_hex(MonoString* input)
{
        if (!input) return NULL;
        char* utf8 = mono_string_to_utf8(input);
        if (!utf8) return NULL;
        size_t len = strlen(utf8);

        char out[64];
        memset(out, 0, sizeof(out));
        joy_core_utils_md5x(utf8, len, out);
        mono_free(utf8);

        return mono_string_new(mono_get_root_domain(), out);
}

void csopen_crypto(void)
{
        csx_register_internal_call("Joy.Api.CryptoExt::md5_hex", (const void*)w_md5_hex);
}
