#pragma once

#include <IR_Common.hpp>
#include <IR_Entity.hpp>

namespace IR::EntitySys {
	constexpr auto MAX_ENTITIES = 1 << 16;

    void Init();
    void Shutdown();

	template <typename T>
	concept Entity = requires(T a) {
		{ (BaseEntity)a };
	};

	bool Create(BaseEntity* entity);
	void Remove(Int64 id);
	inline void Remove(BaseEntity* entity) { Remove(entity->GetID()); }

	template <Entity T>
	T* Create()
	{
		T* entity = new T();

		if (!Create((BaseEntity*)entity)) {
			delete entity;
			return nullptr;
		}

		return entity;
	}

    void Update(Float64 dt);
    void Render();
}
