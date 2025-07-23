#include <Renderer/IR_GLInstance.hpp>
#include <Renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLInstanceList::Init(UInt64 stride, UInt8 location)
    {
        ibuffer.Init(GL_SHADER_STORAGE_BUFFER, nullptr, stride, location, true);
        istride = stride;
    }

    void GLInstanceList::Destroy()
    {
        ibuffer.Destroy();
    }

    UInt32 GLInstanceList::Add(const void* data)
    {
        const UInt8* cdata = (const UInt8*)data;

        UInt32 iId = ilist.size();
        ilist.reserve(ilist.size() + istride);
        ilist.insert(ilist.end(), cdata, cdata + istride);

        return iId / istride;
    }

    void GLInstanceList::Upload()
    {
        ibuffer.Update(ilist.data(), ilist.size(), 0);
    }

    void GLInstanceList::Flush()
    {
        ilist.clear();
    }

}