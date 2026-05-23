#include <IR_Log.hpp>
#include <IR_Common.hpp>

#include <cstdarg>
#include <cstdio>

namespace IR::Log {

    static struct {
        const char* colorcode;
        UInt8 vgacode;
        const char* prefix;
    } s_MsgTypeInfos[] = {
        { "1", 0xf, "INFO" },
        { "1;33", 0xe, "WARN" },
        { "1;31", 0xc, "ERROR" },
        { "1;31;4", 0xc, "FATAL" }
    };

    void Msg(MsgType type, const char* fmt, ...)
    {
        printf("\033[%sm[IR %s]\033[0m - ", s_MsgTypeInfos[(UInt32)type].colorcode, s_MsgTypeInfos[(UInt32)type].prefix);

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        fwrite("\n", 1, 1, stdout);

        if (type == MsgType::FATAL) {
            abort();
        }
    }

}
