#include <IR_Common.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_Physics.hpp>
#include <IR_Random.hpp>
#include <IR_BinaryMap.hpp>
#include <IR_Navmesh.hpp>
#include <IR_Gradient.hpp>
#include <IR_Assets.hpp>
#include <IR_CVar.hpp>
#include <IR_Audio.hpp>

#include <glm/gtc/quaternion.hpp>

#include <IR_Tracy.hpp>

using namespace IR;

static void loadConvars()
{
    KeyValue* kv = KeyValue::Load("convars.ir");

    if (!kv) {
        IRX_MSG(WARN, "Failed to load convars, using defaults");
        return;
    }

    UInt64 children = kv->ChildCount();

    for (UInt64 i = 0; i < children; i++) {
        KeyValue* child = kv->GetChild(i);

        CVar* cvar = CVar::Get(child->GetKey().c_str());

        if (!cvar) {
            continue;
        }

        switch (child->GetType()) {
            case KeyValue::Type::STRING: {
                cvar->SetString(child->GetString().c_str());
                break;
            }

            case KeyValue::Type::NUMBER: {
                cvar->SetFloat64(child->GetNumber());
                break;
            }

            // Works for up to 8 components, but not more
            case KeyValue::Type::ARRAY: {
                Int64 value = 0;
                for (UInt64 j = 0; j < 8; j++) {
                    KeyValue* subChild = child->GetChild(j);

                    if (!subChild) {
                        continue;
                    }

                    if (subChild->GetType() != KeyValue::Type::NUMBER) {
                        IRX_MSG(WARN, "CVar '%s' has non-number child, skipping", child->GetKey().c_str());
                        continue;
                    }

                    Int64 val = (Int64)subChild->GetNumber();

                    value += val << (j * 8);
                }

                cvar->SetInt64(value);
                break;
            }

            case KeyValue::Type::OBJECT: {
                std::string key = child->GetKey();
                
                std::string type = child->FindChildString("type");
                
                // TODO: Support flags.
                
                if (type == "Int64") {
                    Int64 defaultValue = child->FindChildNumber("default", 0);
                    new CVar(key.c_str(), 0, defaultValue);
                } else if (type == "Float64") {
                    Float64 defaultValue = child->FindChildNumber("default", 0.0);
                    new CVar(key.c_str(), 0, defaultValue);
                } else if (type == "String") {
                    std::string defaultValue = child->FindChildString("default", "");
                    new CVar(key.c_str(), 0, defaultValue.c_str());
                } else {
                    IRX_MSG(WARN, "CVar '%s' has unknown type '%s', skipping", key.c_str(), type.c_str());
                }

                break;
            }

            case KeyValue::Type::NIL: {
                break;
            }
        }
    }
}

CVAR(tickrate, 0, (Int64)33);

int main(int argc, char** argv)
{
    Random::SeedRandom();

    loadConvars();

    CVar::Iterate([](CVar* cvar) {
        switch (cvar->GetType()) {
            case CVar::Type::INT64:
                IRX_MSG(INFO, "CVar '%s' = %lld", cvar->GetName().c_str(), cvar->GetInt64());
                break;
            case CVar::Type::FLOAT64:
                IRX_MSG(INFO, "CVar '%s' = %f", cvar->GetName().c_str(), cvar->GetFloat64());
                break;
            case CVar::Type::STRING:
                IRX_MSG(INFO, "CVar '%s' = '%s'", cvar->GetName().c_str(), cvar->GetString().c_str());
                break;
            case CVar::Type::BOOL: {
                IRX_MSG(INFO, "CVar '%s' = %s", cvar->GetName().c_str(), cvar->GetBool() ? "true" : "false");
                break;
            }
        }
    });

    if (!Window::Init(Renderer::API::OPENGL)) {
        IRX_MSG(FATAL, "Failed to init Window, shutting down!");
    }

    if (!Renderer::Init()) {
        IRX_MSG(FATAL, "Failed to init Renderer, shutting down!");
    }

    if (!Physics::Init()) {
        IRX_MSG(FATAL, "Failed to init Physics, shutting down!");
    }

    if (!Audio::Init()) {
        IRX_MSG(FATAL, "Failed to init Audio, shutting down!");
    }

    IRX_MSG(INFO, "Successfully initialized Irreverence");

    std::vector<BinaryMap::EntityData> entDatas;
    Navmesh navmesh;
    if (!BinaryMap::Load("dev_generic.irbm", entDatas, navmesh)) {
        return 1;
    }

    std::vector<glm::vec3> testNavPath;
    if (!navmesh.FindPath(QUtil::QVec3ToVec3({ 56, 824, 8 }), QUtil::QVec3ToVec3({ 1636, 2912, 136 }), testNavPath)) {
        IRX_MSG(WARN, "Could find nav path :(");
    }

    Physics::BeginCompoundObject();

    struct MeshMaterial { Renderer::Mesh* mesh; Renderer::Material* mat; };
    std::vector<MeshMaterial> meshes;
    for (BinaryMap::EntityData& ent : entDatas) {
        std::string className = ent.keyvalues["classname"];
        if (className == "worldspawn") {
            for (const auto& brush : ent.brushes) {
                for (const auto& face : brush.faces) {
                    if (face.flags & BinaryMap::FaceData::FLAGS_NOMESH || face.flags & BinaryMap::FaceData::FLAGS_NORENDER) {
                        continue;
                    }

                    Renderer::Mesh* mesh = Renderer::MakeMesh();
                    mesh->InitPool(face.vertices.data(), face.vertices.size(), face.indices.data(), face.indices.size());
                    meshes.push_back({ mesh, Assets::Material(face.materialName.c_str()) });
                }

                if (brush.flags & BinaryMap::BrushData::FLAGS_NOCONVEX) {
                    continue;
                }

                Physics::AddCompoundConvexHull(brush.convexPoints.data(), brush.convexPoints.size(), brush.origin);
            }
        } else if (className == "light_point_dyn") {
            UInt32 light = Renderer::MakePLight();
            glm::vec3 pos;
            glm::vec3 color;

            pos = QUtil::QVec3ToVec3(QUtil::StrToVec3(ent.keyvalues["origin"].c_str()));
            color = QUtil::StrToVec3(ent.keyvalues["color"].c_str());

            Renderer::SetPLightInnerRadius(light, atof(ent.keyvalues["innerRadius"].c_str()));
            Renderer::SetPLightOuterRadius(light, atof(ent.keyvalues["outerRadius"].c_str()));
            Renderer::SetPLightPosition(light, pos);
            Renderer::SetPLightColor(light, { color.r, color.g, color.b, atoi(ent.keyvalues["intensity"].c_str())});
        }
    }

    Physics::EndCompoundObject(Physics::Type::STATIC, Physics::Layer::NON_MOVING);

    for (UInt32 i = 0; i < meshes.size(); i++) {
        Renderer::SubmitMapMesh(meshes[i].mesh, meshes[i].mat);
    }

    Physics::Object* obj = Physics::MakeCubeObject(glm::vec3(32.0f), Physics::Type::DYNAMIC, Physics::Layer::MOVING, { 180, 250, -476 });

    Float64 nextTick = 0.0f;
    const Float64 tickTime = 1.0 / tickrate.GetInt64();

    IR_THREAD_NAME("Main Thread");

    Gradient<glm::vec4> colorGrad;
    colorGrad.Add(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.0f);
    colorGrad.Add(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.2f);
    colorGrad.Add(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.4f);
    colorGrad.Add(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), 0.6f);
    colorGrad.Add(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.8f);
    colorGrad.Add(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);

    while(!Window::ShouldClose()) {
        IR_ZONE_NAME("Main Thread");

        if (nextTick < Globals.Curtime()) {
            IR_ZONE_NAME("Tick");
            Physics::Update();

            nextTick = Globals.Curtime() + tickTime;
        }

        if (Input::IsKeyPressed(Input::Key::R)) {
            Physics::SetObjectVelocity(obj, { 32, 256, 0 });
        }

        Renderer::SubmitMesh(Renderer::GetMeshCube(), obj->pos, obj->rot, glm::vec3(32.0f), Color(colorGrad.Evaluate(glm::abs(glm::mod(Globals.Curtime() * 0.3, 1.0)) ) * 255.0f), Assets::Material("GRID_BLACK0_ent.shader"));

        for (glm::vec3& pathPoint : testNavPath) {
            Renderer::SubmitMesh(Renderer::GetMeshCube(), pathPoint, glm::quat(), glm::vec3(8.0f), Color(0, 255, 50, 255), Assets::Material("GRID_BLACK0_ent.shader"));
        }

        Renderer::Present();

        if (Input::IsKeyPressed(Input::Key::F1)) {
            Window::ToggleMouseLock();
        }

        if (Input::IsKeyPressed(Input::Key::F2)) {
            Window::ToggleFullscreen();
        }

        if (Input::IsKeyPressed(Input::Key::ESCAPE)) {
            Window::Close();
        }
    }

    Audio::Shutdown();
    Physics::Shutdown();
    Renderer::Shutdown();
    Window::Shutdown();

    return 0;
}
