#include <Renderer/IR_GLLayout.hpp>

namespace IR::Renderer {

    bool GLLayout::InitStandard()
    {
        enum {
            POSITION,
            NORMAL,
            UV
        };

        type = TYPE_STANDARD;

        glCreateVertexArrays(1, &id);

        glEnableVertexArrayAttrib(id, POSITION);
        glVertexArrayAttribFormat(id, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, POSITION, 0);

        glEnableVertexArrayAttrib(id, NORMAL);
        glVertexArrayAttribFormat(id, NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, normal));
        glVertexArrayAttribBinding(id, NORMAL, 0);

        glEnableVertexArrayAttrib(id, UV);
        glVertexArrayAttribFormat(id, UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, texcoord));
        glVertexArrayAttribBinding(id, UV, 0);

        meshpool.Init(id, sizeof(VertexStandard));

        return true;
    }

    bool GLLayout::InitAnimated()
    {
        enum {
            POSITION,
            NORMAL,
            UV,
            BONES,
            WEIGHTS
        };

        type = TYPE_ANIMATED;

        glCreateVertexArrays(1, &id);

        glEnableVertexArrayAttrib(id, POSITION);
        glVertexArrayAttribFormat(id, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, POSITION, 0);

        glEnableVertexArrayAttrib(id, NORMAL);
        glVertexArrayAttribFormat(id, NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, normal));
        glVertexArrayAttribBinding(id, NORMAL, 0);

        glEnableVertexArrayAttrib(id, UV);
        glVertexArrayAttribFormat(id, UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, texcoord));
        glVertexArrayAttribBinding(id, UV, 0);

        glEnableVertexArrayAttrib(id, BONES);
        glVertexArrayAttribIFormat(id, BONES, 1, GL_UNSIGNED_INT, offsetof(VertexAnimated, bones));
        glVertexArrayAttribBinding(id, BONES, 0);

        glEnableVertexArrayAttrib(id, WEIGHTS);
        glVertexArrayAttribFormat(id, WEIGHTS, 4, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, weights));
        glVertexArrayAttribBinding(id, WEIGHTS, 0);

        meshpool.Init(id, sizeof(VertexAnimated));

        return true;
    }

    void GLLayout::Destroy()
    {
        glDeleteVertexArrays(1, &id);

        meshpool.Destroy();

        id = 0;
    }

}