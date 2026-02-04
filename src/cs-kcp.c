#include "liv-config.h"
#include "liv-mono.h"
#include "external/kcp.h"
#include "csclib.h"


static void
ckcp_setoutput(ikcpcb* kcp, MonoObject* del)
{
        //ikcp_setoutput(kcp, );
}


static int
ckcp_input(ikcpcb* kcpcb, MonoArray* bytes)
{
        char* buf;
        int result;
        buf = livS_mono_array_addr(bytes, sizeof(char), 0);
        result = ikcp_input(kcpcb, buf, livS_mono_array_length(bytes));
        return result;
}


static int
ckcp_recv(ikcpcb* kcpcb, MonoArray** out_bytes)
{
        char cstr_buf[LIVNET_MAX_BUFFER];
        int peeksize;
        peeksize = ikcp_peeksize(kcpcb);
        if (peeksize > 0 && peeksize <= LIVNET_MAX_BUFFER) {
                peeksize = ikcp_recv(kcpcb, cstr_buf, peeksize);
                *out_bytes = livS_mono_array_new(livS_mono_domain_get(), livS_mono_get_byte_class(), peeksize);
                char* data = livS_mono_array_addr(*out_bytes, sizeof(char), 0);
                for (int i = 0; i < peeksize; i++) {
                        data[i] = cstr_buf[i];
                }
        }
        else {
                peeksize = 0;
        }
        return peeksize;
}


static int
ckcp_send(ikcpcb* kcpcb, MonoArray* bytes)
{
        char* buf;
        int result;
        buf = livS_mono_array_addr(bytes, sizeof(char), 0);
        result = ikcp_send(kcpcb, buf, livS_mono_array_length(bytes));
        return result;
}


void csopen_kcp()
{
        livS_mono_add_internal_call("Livlib.Native.Kcp::Create", ikcp_create);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Wndsize", ikcp_wndsize);
        livS_mono_add_internal_call("Livlib.Native.Kcp::SetOutput", ckcp_setoutput);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Release", ikcp_release);
        livS_mono_add_internal_call("Livlib.Native.Kcp::GetConv", ikcp_getconv);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Check", ikcp_check);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Update", ikcp_update);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Input", ckcp_input);
        livS_mono_add_internal_call("Livlib.Native.Kcp::PeekSize", ikcp_peeksize);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Recv", ckcp_recv);
        livS_mono_add_internal_call("Livlib.Native.Kcp::Send", ckcp_send);

}
