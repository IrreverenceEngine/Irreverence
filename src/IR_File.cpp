#include <IR_File.hpp>

#include <stdio.h>

namespace IR {

    File::File(const char* path, const char* mode)
    {
        m_File = fopen(path, mode);
    }

    File::~File()
    {
        Close();
    }

    void File::Close()
    {
        if (!m_File) {
            return;
        }

        fclose((FILE*)m_File);
        m_File = nullptr;
    }

    UInt64 File::Tell() IR_RETURN(IsOpen() ? ftello64((FILE*)m_File) : 0)

    void File::Seek(SeekMode seek, UInt64 pos)
    {
        if (!IsOpen()) {
            return;
        }
        
        fseeko64((FILE*)m_File, pos, seek);
    }

    void File::Read(void* out, UInt64 size)
    {
        if (!IsOpen()) {
            return;
        }

        fread(out, 1, size, (FILE*)m_File);
    }

    char* File::ReadAll(UInt64* size, bool nt)
    {
        UInt64 fsize = 0;

        fseeko64((FILE*)m_File, 0, SEEK_END);
        fsize = ftello64((FILE*)m_File);
        fseeko64((FILE*)m_File, 0, SEEK_SET);

        UInt64 allocSize = fsize + (nt ? 1 : 0);
        char* data = new char[allocSize];

        fread(data, 1, fsize, (FILE*)m_File);

        if (nt) {
            data[fsize] = '\0';
        }

        if (size) {
            *size = fsize;
        }

        return data;
    }

    void File::Write(const void* data, UInt64 size)
    {
        if (!IsOpen()) {
            return;
        }

        fwrite(data, 1, size, (FILE*)m_File);
    }

}