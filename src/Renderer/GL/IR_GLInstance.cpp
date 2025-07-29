#include <Renderer/GL/IR_GLInstance.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLInstanceList::Init(UInt32 stride, UInt8 location)
    {
        m_Buffer.Init(GL_SHADER_STORAGE_BUFFER, nullptr, stride, location, true);
        m_Stride = stride;
    }

    void GLInstanceList::Destroy()
    {
        m_Buffer.Destroy();
    }

    UInt32 GLInstanceList::Add(const void* data)
    {
        const UInt8* cdata = (const UInt8*)data;

        UInt32 iId = m_List.size();
        m_List.reserve(m_List.size() + m_Stride);
        m_List.insert(m_List.end(), cdata, cdata + m_Stride);

        return iId / m_Stride;
    }

    void GLInstanceList::Upload()
    {
        m_Buffer.Update(m_List.data(), m_List.size(), 0);
    }

    void GLInstanceList::Flush()
    {
        m_List.clear();
    }

}