#include <IR_Log.hpp>
#include <IR_Common.hpp>

#include <bgfx/bgfx.h>

#include <cstdarg>
#include <cstdio>

#include <cstdlib>
#include <string>
#include <vector>

namespace IR::Log {
    
    constexpr UInt32 MAX_SCRMSG_COUNT = 20;

    static struct {
        const char* colorcode;
        UInt8 vgacode;
        const char* prefix;
        Float64 stayTime;
    } s_MsgTypeInfos[] = {
        { "1", 0xf, "INFO", 5.0f },
        { "1;33", 0xe, "WARN", 7.5f },
        { "1;31", 0xc, "ERROR", 10.0f },
        { "1;31;4", 0xc, "FATAL", 0.0f }
    };

    struct BGFXTextData  { 
        std::string str; 
        UInt8 attr;
        Float64 removeTime;
        UInt32 count;
    };

    static std::vector<BGFXTextData> s_BGFXTexts;

    void Msg(MsgType type, const char* fmt, ...)
    {
        printf("\033[%sm[IRREVERENCE %s]\033[0m - ", s_MsgTypeInfos[(int)type].colorcode, s_MsgTypeInfos[(int)type].prefix);

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        
        fwrite("\n", 1, 1, stdout);

        if (type == MsgType::FATAL) {
            abort();
        }
    }

    void ScrMsg(MsgType type, const char* fmt, ...)
    {
        if (type == MsgType::FATAL) {
            IR_MSG(WARN, "ScrMsg can't be used with FATAL.. please fix");
            return;
        }

        char buffer[10000];

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, 10000, fmt, args);
        va_end(args);

        Float64 startTime = Globals.curtime;
        Float64 stayTime = s_MsgTypeInfos[(int)type].stayTime;
        std::string str = buffer;
        if (s_BGFXTexts.size() > 0) {
            BGFXTextData* lastText = &s_BGFXTexts[s_BGFXTexts.size() - 1];
            startTime = lastText->removeTime;
            if (lastText->str == str) {
                lastText->count++;
                lastText->removeTime = Globals.curtime + stayTime;
                return;
            }
        } 

        if (s_BGFXTexts.size() >= MAX_SCRMSG_COUNT) {
            s_BGFXTexts.erase(s_BGFXTexts.begin());
        }

        s_BGFXTexts.push_back({ str, s_MsgTypeInfos[(int)type].vgacode, startTime + stayTime, 1 });
    }

    void DrawScrMsgs()
    {        
        int y = 0;
        for (UInt32 i = 0; i < s_BGFXTexts.size(); i++) {
            const BGFXTextData& textData = s_BGFXTexts[i];
            if (Globals.curtime > textData.removeTime) {
                s_BGFXTexts.erase(s_BGFXTexts.begin() + i);
                continue;
            }

            std::string tmpStr = textData.str;
            if (textData.count > 1) {
                tmpStr += " (" + std::to_string(textData.count) + "x)";
            }

            bgfx::dbgTextPrintf(0, y, textData.attr, tmpStr.c_str());
            y++;
        }
    }
}