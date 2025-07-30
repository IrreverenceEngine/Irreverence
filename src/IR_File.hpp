#pragma once

#include <IR_Common.hpp>

namespace IR {
    class File {
    public:
        enum SeekMode {
            FILE_SEEK_SET,
            FILE_SEEK_CUR,
            FILE_SEEK_END
        };

        File() = delete;
        File(const char* path, const char* mode);
        ~File();

        void Close();

        UInt64 Tell();
        void Seek(SeekMode seek, UInt64 pos);
        void Read(void* out, UInt64 size);
        char* ReadAll(UInt64* size = nullptr, bool nt = true);
        void Write(const void* data, UInt64 size);

        bool IsOpen() const IR_RETURN(m_File != nullptr);

    private:
        void* m_File = nullptr;
    };
}
