#include <renderer/IR_DXRenderer.hpp>

#include <IR_Common.hpp>

namespace IR::Renderer {
    const char* DX::GetName() IR_RETURN("DirectX")
    bool DX::Init() IR_UNIMPLEMENTED
    void DX::Shutdown() IR_UNIMPLEMENTED
    void DX::Present() IR_UNIMPLEMENTED
    Model* DX::MakeModel() IR_UNIMPLEMENTED
    Material* DX::MakeMaterial() IR_UNIMPLEMENTED
}