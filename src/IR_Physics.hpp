#pragma once

#include <IR_Common.hpp>

#include <glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace IR::Physics {
    constexpr UInt16 MAX_OBJECTS = UINT16_MAX;
    constexpr UInt32 INVALID_BODYID = UINT32_MAX;

    enum class Shape : UInt8 {
        CUBE,
        SPHERE,
        CYLINDER,
        CAPSULE,
        CONVEX,
        COMPOUND,
        _COUNT
    };

    enum class Layer : UInt8 {
        NON_MOVING,
        MOVING,
        _COUNT
    };

    enum class Type : UInt8 {
        STATIC,
        KINEMATIC,
        DYNAMIC,
        _COUNT
    };

    struct ObjectInfo {
        UInt32 bodyId;
        UInt16 index;
        Shape shape;
        Layer layer;
        UInt32 flags;

        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 vel;
        glm::vec3 angularVel;
        Float32 friction;
        Float32 restitution;
    };

    bool Init();
    void Shutdown();

    void Update();

    ObjectInfo* MakeCubeObject(
        const glm::vec3 scale,
        Type type, Layer layer,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);
    ObjectInfo* MakeSphereObject(
        Float32 radius,
        Type type, Layer layer,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);
    ObjectInfo* MakeCylinderObject(
        Float32 radius, Float32 height,
        Type type, Layer layer,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);
    ObjectInfo* MakeCapsuleObject(
        Float32 radius, Float32 height,
        Type type, Layer layer,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);
    ObjectInfo* MakeConvexHullObject(
        const glm::vec3* points, UInt32 count,
        Type type, Layer layer,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);

    void BeginCompoundObject(
        Type type, Layer layer,
        bool bStatic = false,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0);
    void AddCompoundCube(
        const glm::vec3 scale,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0
    );
    void AddCompoundSphere(
        Float32 radius,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0
    );
    void AddCompoundCylinder(
        Float32 radius, Float32 height,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0
    );
    void AddCompoundCapsule(
        Float32 radius, Float32 height,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0
    );
    void AddCompoundConvexHull(
        const glm::vec3* points, UInt32 count,
        const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(),
        UInt32 extra = 0
    );
    ObjectInfo* EndCompoundObject();

    ObjectInfo* GetObjectInfo(UInt32 index);
}