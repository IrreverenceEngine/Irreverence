#include <IR_EntitySys.hpp>

namespace IR::EntitySys {

	static BaseEntity* s_pEntities[MAX_ENTITIES];

    void Init()
    {
		for (Int64 i = 0; i < MAX_ENTITIES; i++) {
			s_pEntities[i] = nullptr;
		}
    }

    void Shutdown()
    {
		for (Int64 i = 0; i < MAX_ENTITIES; i++) {
			if (!s_pEntities[i]) {
				continue;
			}

			s_pEntities[i]->Destroy();
			delete s_pEntities[i];
			s_pEntities[i] = nullptr;
		}
    }

	bool Create(BaseEntity* entity)
	{
		if (!entity->Spawn()) {
			return false;
		}

		Int64 id = -1;

		for (Int64 i = 0; i < MAX_ENTITIES; i++) {
			if (s_pEntities[i]) {
				continue;
			}

			id = i;
			break;
		}

		if (id < 0) {
			return false; // No available slots
		}

		s_pEntities[id] = entity;
		entity->SetID(id);

		return true;
	}

	void Remove(Int64 id)
	{
		if (id < 0 || id >= MAX_ENTITIES || !s_pEntities[id]) {
			return; // Invalid ID or no entity at this ID
		}

		BaseEntity* entity = s_pEntities[id];
		entity->Destroy();
		delete entity;
		s_pEntities[id] = nullptr;
	}

    void Update(Float64 dt)
    {
		for (Int64 i = 0; i < MAX_ENTITIES; i++) {
			BaseEntity* entity = s_pEntities[i];

			if (!entity) {
				continue;
			}

			entity->Update(dt);
		}
    }

    void Render()
    {
		for (Int64 i = 0; i < MAX_ENTITIES; i++) {
			BaseEntity* entity = s_pEntities[i];

			if (!entity) {
				continue;
			}

			entity->Render();
		}
    }

}
