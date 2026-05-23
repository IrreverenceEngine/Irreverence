#include <IR_Physics.hpp>
#include <IR_CVar.hpp>
#include <IR_Tracy.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <cstdarg>

namespace IR::Physics {
    static void jphTraceImpl(const char* inFMT, ...);

    constexpr UInt32 MAX_BODIES = MAX_OBJECTS;
    constexpr UInt32 NUM_BODY_MUTEXES = 0;
    constexpr UInt32 MAX_BODY_PAIRS = MAX_BODIES;
    constexpr UInt32 MAX_CONTACT_CONSTRAINTS = MAX_BODIES;

    constexpr UInt8 s_TouchMask[(UInt8)Layer::_COUNT] = {
        /*                    01*/
        /*NON-MOVING: [0]*/ 0b10,
        /*    MOVING: [1]*/ 0b11,
    };

    class BPLayerInterface : public JPH::BroadPhaseLayerInterface {
    public:
        BPLayerInterface() {}

        virtual UInt32 GetNumBroadPhaseLayers() const override IR_RETURN((UInt32)Layer::_COUNT)

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
            IR_RETURN((JPH::BroadPhaseLayer)inLayer)

        virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch ((UInt8)inLayer)
            {
            case (UInt8)Layer::NON_MOVING: return "NON-MOVING";
            case (UInt8)Layer::MOVING: return "MOVING";
            default: return "INVALID";
            }
        }
    };

    class OvsBPLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
            IR_RETURN((s_TouchMask[(UInt8)inLayer1] >> (UInt8)inLayer2) & 1)
    };

    class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
           IR_RETURN((s_TouchMask[(UInt8)inObject1] >> (UInt8)inObject2) & 1)
    };

    class ContactListener : public JPH::ContactListener {
    public:
        virtual JPH::ValidateResult	OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override
            IR_RETURN(JPH::ValidateResult::AcceptAllContactsForThisBodyPair)

        virtual void OnContactAdded(
            const JPH::Body& inBody1,
            const JPH::Body& inBody2,
            const JPH::ContactManifold& inManifold,
            JPH::ContactSettings& ioSettings) override {}

        virtual void OnContactPersisted(
            const JPH::Body& inBody1,
            const JPH::Body& inBody2,
            const JPH::ContactManifold& inManifold,
            JPH::ContactSettings& ioSettings) override {}

        virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {}
    };

    static BPLayerInterface s_BPLayerInterface;
    static OvsBPLayerFilter s_OvsBPLayerFilter;
    static ObjectLayerPairFilter s_ObjectLayerPairFilter;
    static ContactListener s_ContactListener;

    static JPH::JobSystemThreadPool s_JobSystem;
    static JPH::TempAllocatorImpl* s_TmpAlloc; // NOTE: If we run out of temp memory, we can change this to the fallback version.

    static JPH::PhysicsSystem s_PhysicsSystem;
    static JPH::BodyInterface* s_BodyInterface = nullptr;
    static const JPH::BodyLockInterface* s_BodyNoLockInterface = nullptr;
    static const JPH::BodyLockInterface* s_BodyLockInterface = nullptr;
    static const JPH::BroadPhaseQuery* s_BPQuery = nullptr;

    static Object s_Objects[MAX_OBJECTS];
    static UInt16 s_ObjectCount = 0;
    static UInt16 s_ObjectReach = 0;

    static UInt16 GetFreeObjectId()
    {
        UInt16 index = MAX_OBJECTS;
        for (UInt16 i = 0; i < MAX_OBJECTS; i++) {
            Object* obj = &s_Objects[i];

            if (obj->bodyId != INVALID_BODYID) continue;

            index = i;
            break;
        }

        if (index == MAX_OBJECTS) {
            IR_MSG(FATAL, "We hit max Physics object limit");
        }

        return index;
    }

    static Object* AddObject(const Object& data)
    {
        UInt32 count = data.index + 1;
        if (count > s_ObjectReach) {
            s_ObjectReach = count;
        }

        Object& obj = s_Objects[data.index];

        obj = data;
        s_ObjectCount++;

        return &obj;
    }

    static Object* MakeObject(
        const JPH::Shape* joltShape, Shape shape,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        const UInt32 extra
    )
    {
        UInt16 index = GetFreeObjectId();

        JPH::BodyCreationSettings settings(
            joltShape,
            { pos.x, pos.y, pos.z },
            { rot.x, rot.y, rot.z, rot.w },
            (JPH::EMotionType)type,
            (JPH::ObjectLayer)layer
        );

        settings.mUserData = index;
        JPH::BodyID bodyId = s_BodyInterface->CreateAndAddBody(settings, JPH::EActivation::Activate);

        if (bodyId == (JPH::BodyID)INVALID_BODYID) {
            return nullptr;
        }

        Object obj;
        obj.bodyId = *(UInt32*)&bodyId;
        obj.index = index;
        obj.layer = layer;
        obj.shape = shape;
        obj.flags = 0;

        obj.pos = pos;
        obj.rot = rot;
        obj.vel = glm::vec3(0.0f);
        obj.angularVel = glm::vec3(0.0f);

        return AddObject(obj);
    }

    bool Init()
    {
        JPH::RegisterDefaultAllocator();

        JPH::Trace = jphTraceImpl;
        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        s_JobSystem.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

        static JPH::TempAllocatorImpl tmpAlloc(128 * 1024 * 1024);
        s_TmpAlloc = &tmpAlloc;

        s_PhysicsSystem.Init(
            MAX_BODIES,
            NUM_BODY_MUTEXES,
            MAX_BODY_PAIRS,
            MAX_CONTACT_CONSTRAINTS,
            s_BPLayerInterface,
            s_OvsBPLayerFilter,
            s_ObjectLayerPairFilter
        );

        s_PhysicsSystem.SetGravity({0, -9.81 * 128.0f, 0});

        s_BodyInterface = &s_PhysicsSystem.GetBodyInterface();
        s_BodyNoLockInterface = &s_PhysicsSystem.GetBodyLockInterfaceNoLock();
        s_BodyLockInterface = &s_PhysicsSystem.GetBodyLockInterface();
        s_BPQuery = &s_PhysicsSystem.GetBroadPhaseQuery();

        for (UInt16 i = 0; i < MAX_OBJECTS; i++) {
            Object& obj = s_Objects[i];
            obj.bodyId = INVALID_BODYID;
            obj.index = 0;
            obj.layer = Layer::NON_MOVING;
            obj.shape = Shape::CUBE;
            obj.flags = 0;
        }

        return true;
    }

    void Shutdown()
    {
        for (UInt32 i = 0; i < MAX_OBJECTS; i++) {
            Object* obj = &s_Objects[i];
            if (obj->bodyId == INVALID_BODYID) continue;

            s_BodyInterface->RemoveBody((JPH::BodyID)obj->bodyId);
            s_BodyInterface->DestroyBody((JPH::BodyID)obj->bodyId);
            obj->bodyId = INVALID_BODYID;
	    }

        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void Update()
    {
		IR_ZONE_NAME("Physics Update");

		static CVar* tickrate = CVar::Get("tickrate");

		if (!tickrate) {
			IR_MSG(FATAL, "Failed to get tickrate CVar");
			return;
		}

		Float32 delta = 1.0f / tickrate->GetInt64();

	    constexpr Int32 steps = 2;

        s_PhysicsSystem.Update(delta, steps, s_TmpAlloc, &s_JobSystem);

        // TODO: Check this out later, https://jrouwe.github.io/JoltPhysicsDocs/5.2.0/class_body_lock_multi_read.html

        JPH::RVec3 tempVec3;
        JPH::Quat tempQuat;
        for (UInt16 i = 0; i < s_ObjectReach; i++) {
            Object& obj = s_Objects[i];
            if (obj.bodyId == UINT32_MAX) {
                continue;
            }

            JPH::BodyLockRead lock(*s_BodyLockInterface, (JPH::BodyID)obj.bodyId);
            if (!lock.Succeeded()) {
                continue;
            }

            const JPH::Body& body = lock.GetBody();

            tempVec3 = body.GetCenterOfMassPosition();
            obj.pos = { tempVec3.GetX(), tempVec3.GetY(), tempVec3.GetZ() };

            tempQuat = body.GetRotation();
            obj.rot = { tempQuat.GetW(), tempQuat.GetX(), tempQuat.GetY(), tempQuat.GetZ() };

            tempVec3 = body.GetLinearVelocity();
            obj.vel = { tempVec3.GetX(), tempVec3.GetY(), tempVec3.GetZ() };

            tempVec3 = body.GetAngularVelocity();
            obj.angularVel = { tempVec3.GetX(), tempVec3.GetY(), tempVec3.GetZ() };

            obj.flags = 0;
            obj.flags |= body.IsActive() ? Object::FLAG_ISACTIVE : 0;
        }
    }

    Object* MakeCubeObject(
        const glm::vec3 scale,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::Vec3 jphSize = JPH::Vec3( scale.x, scale.y, scale.z );
        JPH::BoxShape* shape = new JPH::BoxShape(jphSize);

        Object* obj = MakeObject(shape, Shape::CUBE, type, layer, pos, rot, 0);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    Object* MakeSphereObject(
        Float32 radius,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::SphereShape* shape = new JPH::SphereShape(radius);
        Object* obj = MakeObject(shape, Shape::SPHERE, type, layer, pos, rot, 0);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    Object* MakeCylinderObject(
        Float32 radius, Float32 height,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::CylinderShape* shape = new JPH::CylinderShape(height, radius);
        Object* obj = MakeObject(shape, Shape::SPHERE, type, layer, pos, rot, 0);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    Object* MakeCapsuleObject(
        Float32 radius, Float32 height,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::CapsuleShape* shape = new JPH::CapsuleShape(height, radius);
        Object* obj = MakeObject(shape, Shape::CAPSULE, type, layer, pos, rot, 0);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    Object* MakeConvexHullObject(
        const glm::vec3* points, UInt32 count,
        Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::Array<JPH::Vec3> bruh;

        for (UInt32 i = 0; i < count; i++) {
            bruh.emplace_back(JPH::Vec3(points[i].x, points[i].y, points[i].z));
        }

        JPH::ConvexHullShapeSettings settings(bruh);

        JPH::Shape::ShapeResult result;
        JPH::ConvexHullShape* shape = new JPH::ConvexHullShape(settings, result);

        if (result.HasError()) {
            IR_MSG(ERROR, "Physics couldn't create Convex Hull Shape: %s", result.GetError().c_str());
            delete shape;
            return nullptr;
        }

        Object* obj = MakeObject(shape, Shape::CONVEX, type, layer, pos, rot, 0);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    static struct {
        JPH::CompoundShapeSettings* settings = nullptr;
        bool isStatic = false;
    } s_CompObjectInfo;
    void BeginCompoundObject(bool bStatic)
    {
        if (s_CompObjectInfo.settings) {
            IR_MSG(FATAL, "An Compound Object was not finished before beginning another Compound Object");
        }

        s_CompObjectInfo.isStatic = bStatic;
        s_CompObjectInfo.settings = bStatic ?
            (JPH::CompoundShapeSettings*)new JPH::StaticCompoundShapeSettings :
            (JPH::CompoundShapeSettings*)new JPH::MutableCompoundShapeSettings;
    }

    void AddCompoundCube(
        const glm::vec3 scale,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::Vec3 jphSize = { scale.x, scale.y, scale.z };
        JPH::BoxShape* shape = new JPH::BoxShape(jphSize);
        s_CompObjectInfo.settings->AddShape({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, shape, extra);
    }

    void AddCompoundSphere(
        Float32 radius,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::SphereShape* shape = new JPH::SphereShape(radius);
        s_CompObjectInfo.settings->AddShape({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, shape, extra);
    }

    void AddCompoundCylinder(
        Float32 radius, Float32 height,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::CylinderShape* shape = new JPH::CylinderShape(height, radius);
        s_CompObjectInfo.settings->AddShape({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, shape, extra);
    }

    void AddCompoundCapsule(
        Float32 radius, Float32 height,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::CapsuleShape* shape = new JPH::CapsuleShape(height, radius);
        s_CompObjectInfo.settings->AddShape({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, shape, extra);
    }

    void AddCompoundConvexHull(
        const glm::vec3* points, UInt32 count,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        JPH::Array<JPH::Vec3> bruh;

        for (UInt32 i = 0; i < count; i++) {
            bruh.emplace_back(JPH::Vec3(points[i].x, points[i].y, points[i].z));
        }

        JPH::ConvexHullShapeSettings settings(bruh);

        JPH::Shape::ShapeResult result;
        JPH::ConvexHullShape* shape = new JPH::ConvexHullShape(settings, result);

        if (result.HasError()) {
            IR_MSG(ERROR, "Physics couldn't create Convex Hull Shape: %s", result.GetError().c_str());
            delete shape;
            return;
        }

        s_CompObjectInfo.settings->AddShape({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w }, shape, extra);
    }

    Object* EndCompoundObject(Type type, Layer layer,
        const glm::vec3& pos, const glm::quat& rot,
        UInt32 extra
    )
    {
        IR_DEFER({
            if (s_CompObjectInfo.settings) {
                delete s_CompObjectInfo.settings;
                s_CompObjectInfo.settings = nullptr;
            }
        });

        JPH::Shape::ShapeResult result;

        JPH::CompoundShape* shape = s_CompObjectInfo.isStatic ?
        (JPH::CompoundShape*) new JPH::StaticCompoundShape(*(JPH::StaticCompoundShapeSettings*)s_CompObjectInfo.settings, *s_TmpAlloc, result) :
        (JPH::CompoundShape*) new JPH::MutableCompoundShape(*(JPH::MutableCompoundShapeSettings*)s_CompObjectInfo.settings, result);

        if (result.HasError()) {
            IR_MSG(ERROR, "Physics couldn't create Convex Hull Shape: %s", result.GetError().c_str());
            delete shape;
            return nullptr;
        }

        Object* obj = MakeObject(shape, Shape::COMPOUND, type, layer, pos, rot, extra);

        if (!obj) {
            delete shape;
        }

        return obj;
    }

    void DestroyObject(Object* object)
    {
        if (!object || object->bodyId == INVALID_BODYID) {
            return;
        }

        s_BodyInterface->RemoveBody((JPH::BodyID)object->bodyId);
        s_BodyInterface->DestroyBody((JPH::BodyID)object->bodyId);

        bool canReduce = (object->index + 1) >= s_ObjectReach;
        s_ObjectReach = canReduce ? object->index : s_ObjectReach;

        s_ObjectCount--;

        object->bodyId = INVALID_BODYID;
    }

    void SetObjectPosition(Object* object, const glm::vec3& pos, bool activateNearby)
    {
        if (activateNearby) {
            JPH::AABox activatearea = s_BodyInterface->GetTransformedShape((JPH::BodyID)object->bodyId).GetWorldSpaceBounds();
            activatearea.ExpandBy({ 0.1f, 0.1f, 0.1f });
            s_BodyInterface->ActivateBodiesInAABox(activatearea, *(JPH::BroadPhaseLayerFilter*)&s_BPLayerInterface, {});
        }

        s_BodyInterface->SetPosition((JPH::BodyID)object->bodyId, { pos.x, pos.y, pos.z }, JPH::EActivation::Activate);
        object->pos = pos;
    }

    void SetObjectRotation(Object* object, const glm::quat& rot, bool activateNearby)
    {
        if (activateNearby) {
            JPH::AABox activatearea = s_BodyInterface->GetTransformedShape((JPH::BodyID)object->bodyId).GetWorldSpaceBounds();
            activatearea.ExpandBy({ 0.1f, 0.1f, 0.1f });
            s_BodyInterface->ActivateBodiesInAABox(activatearea, *(JPH::BroadPhaseLayerFilter*)&s_BPLayerInterface, {});
        }

        s_BodyInterface->SetRotation((JPH::BodyID)object->bodyId, { rot.x, rot.y, rot.z, rot.w }, JPH::EActivation::Activate);
        object->rot = rot;
    }

    void SetObjectVelocity(Object* object, const glm::vec3& vel)
    {
        s_BodyInterface->SetLinearVelocity((JPH::BodyID)object->bodyId, { vel.x, vel.y, vel.z });
        object->vel = vel;
    }

    void SetObjectAngularVelocity(Object* object, const glm::vec3& vel)
    {
        s_BodyInterface->SetAngularVelocity((JPH::BodyID)object->bodyId, { vel.x, vel.y, vel.z });
        object->angularVel = vel;
    }

    void AddObjectVelocity(Object* object, const glm::vec3& vel)
    {
        s_BodyInterface->AddLinearVelocity((JPH::BodyID)object->bodyId, { vel.x, vel.y, vel.z });
        object->vel += vel;
    }

    void AddObjectAngularVelocity(Object* object, const glm::vec3& vel)
    {
        glm::vec3 newVel = object->angularVel + vel;
        s_BodyInterface->SetAngularVelocity((JPH::BodyID)object->bodyId, { newVel.x, newVel.y, newVel.z });

        object->angularVel = newVel;
    }

    Object* GetObject(UInt32 index)
    {
        if (index >= s_ObjectReach) {
            return nullptr;
        }

        Object* obj = &s_Objects[index];
        return obj->bodyId != UINT32_MAX ? obj : nullptr;
    }

    static void jphTraceImpl(const char* inFMT, ...)
    {
        char buffer[1024];

        va_list list;
        va_start(list, inFMT);
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);

        IR_MSG(WARN, "Physics, %s", buffer);
    }
}
