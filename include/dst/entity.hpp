#pragma once

#include <random>
#include <string>
#include <vector>

#include "d/d_cc_d.h"
#include "d/d_stage.h"

#include "mods/svc/actor.h"
#include "f_op/f_op_actor.h"
#include "f_op/f_op_actor_mng.h"

#define DST_REGISTER_ENTITY(EntityType) \
  std::memcpy( \
    (void*)EntityType##_dst_prof_desc.name, \
    (void*)EntityType::ID(), \
    sizeof(EntityType##_dst_prof_desc.name) \
  ); \
  if (svc_actor->register_actor(mod_ctx, \
    &EntityType##_dst_prof_desc, \
    &EntityType##_dst_prof_name, \
    &EntityType##_dst_handle \
  ) != MOD_OK) { \
    mods::log::error("Failed to register Entity"); \
  } \
  else { \
    mods::log::info("Entity successfully registered"); \
  }

#define DST_UNREGISTER_ENTITY(EntityType) \
  if (svc_actor->unregister_actor(mod_ctx, EntityType##_dst_handle) != MOD_OK) { \
    mods::log::error("Failed to unregister Entity"); \
  } \
  else { \
    mods::log::info("Entity successfully unregistered"); \
  }

#define DST_DEFINE_ENTITY(EntityType) \
  static_assert(std::is_base_of_v<::DST::Entity, const EntityType>, "Class must derive from DST::Entity"); \
  inline int EntityType##_dst_create(void* self) { \
    if (DST::Entity::Get(static_cast<fopAc_ac_c*>(self)) == nullptr) { \
      auto* entity = new EntityType(); \
      entity->actor = static_cast<fopAc_ac_c*>(self); \
      *reinterpret_cast<EntityType**>(static_cast<char*>(self) + sizeof(fopAc_ac_c)) = entity; \
      fopAcM_OnCondition(static_cast<fopAc_ac_c*>(self), fopAcCnd_INIT_e); \
    } \
    return DST::Entity::Get(static_cast<fopAc_ac_c*>(self))->OnCreate(); \
  } \
  inline int EntityType##_dst_isdelete(void* self) { \
    return DST::Entity::Get(static_cast<fopAc_ac_c*>(self))->IsDelete(); \
  } \
  inline int EntityType##_dst_delete(void* self) { \
    int res = DST::Entity::Get(static_cast<fopAc_ac_c*>(self))->OnDelete(); \
    delete DST::Entity::Get(static_cast<fopAc_ac_c*>(self)); \
    return res; \
  } \
  inline int EntityType##_dst_draw(void* self) { \
    return DST::Entity::Get(static_cast<fopAc_ac_c*>(self))->OnDraw(); \
  } \
  inline int EntityType##_dst_execute(void* self) { \
    return DST::Entity::Get(static_cast<fopAc_ac_c*>(self))->OnExecute(); \
  } \
  extern "C" { \
    inline ActorProfileDesc EntityType##_dst_prof_desc = { \
      .priority_group = EntityType::profile.priority_group, \
      .process_size = sizeof(fopAc_ac_c) + sizeof(void*), \
      .draw_priority = EntityType::profile.draw_priority, \
      .status = EntityType::profile.status, \
      .group = EntityType::profile.group, \
      .cull_type = EntityType::profile.cull_type, \
      .create_function = EntityType##_dst_create, \
      .delete_function = EntityType##_dst_delete, \
      .execute_function = EntityType##_dst_execute, \
      .is_delete_function = EntityType##_dst_isdelete, \
      .draw_function = EntityType##_dst_draw, \
    }; \
    inline ProfileName EntityType##_dst_prof_name; \
    inline ActorHandle EntityType##_dst_handle; \
  }

namespace DST {
  struct EntityProfile {
    uint8_t group;
    uint32_t status;
    uint16_t priority_group;
    int16_t draw_priority;
    uint8_t cull_type;
  };

  class Entity {
    public:
    std::string name;
    fopAc_ac_c* actor;

    virtual int OnCreate() { return 4; };
    virtual int OnCreateHeap() { return 1; };
    virtual int IsDelete() { return 1; };
    virtual int OnDelete() { return 1; };
    virtual int OnDraw() { return 1; };
    virtual void OnHit(dCcD_GObjInf*) {};
    virtual int OnExecute() { return 1; };

    int Create() {
      int res = OnCreate();
      for (Entity* subentity : subentities)
        subentity->Create();
      return res;
    }
    int Delete() {
      int res = OnDelete();
      for (Entity* subentity : subentities)
        subentity->Delete();
      return res;
    }
    int Draw() {
      int res = OnDraw();
      for (Entity* subentity : subentities)
        subentity->Draw();
      return res;
    }
    int Execute() {
      int res = OnExecute();
      for (Entity* subentity : subentities)
        subentity->Execute();
      return res;
    }
    inline static Entity* Get(fopAc_ac_c* actor) {
      return *reinterpret_cast<Entity**>(
        reinterpret_cast<char*>(actor) + sizeof(fopAc_ac_c)
      );
    }
    stage_actor_data_class& GetParams() {
      if (!generated[1]) {
        params = {
          .base = {
            .parameters = 0,
            .position = position,
            .angle = angle,
            .setID = 0xFFFF,
          },
        };
        std::memcpy(
          (void*)params.name,
          (void*)ID(),
          sizeof(params.name)
        );
        if (std::memcmp(
          (void*)params.name,
          (void*)ID(),
          sizeof(params.name)
        ) != 0)
          mods::log::error("ID mismatch: {}, {}", ID(), params.name);
        generated[1] = true;
        mods::log::info("Params generated");
      }
      return params;
    }
    static char* ID() {
      if (!generated[0]) {
        int offsets[3] = { 48, 65, 97 };
        int sizes[3] = { 10, 26, 26 };
        for (size_t i = 0; i < 7; i++) {
          int offset = offsets[std::rand() % 3];
          int size = sizes[std::rand() % 3];
          id[i] = std::rand() % size + offset;
        }
        id[7] = '\0';
        generated[0] = true;
        mods::log::info("ID generated: {}", id);
      }
      return id;
    }

    BE<cXyz>& Position() {
      return position;
    }
    void Position(BE<cXyz>& pos) {
      position = pos;
    }
    BE<csXyz>& Angle() {
      return angle;
    }
    void Angle(BE<csXyz>& ang) {
      angle = ang;
    }

    void SetCullBox(MtxP mtx, Vec min, Vec max) {
      fopAcM_SetMtx(actor, mtx);
      fopAcM_SetMin(actor, min.x, min.y, min.z);
      fopAcM_SetMax(actor, max.x, max.y, max.z);
    }
    void SyncAttention() {
      actor->eyePos = actor->attention_info.position = actor->current.pos;
      actor->attention_info.flags = 0;
    }
    void DeleteActor() {
      fopAcM_delete(actor);
    }

    static int CreateHeapCallback(fopAc_ac_c* a) {
      return dynamic_cast<Entity*>(Get(a))->OnCreateHeap();
    }
    static void HitCallback(fopAc_ac_c* tg, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf* atObj) {
      dynamic_cast<Entity*>(Get(tg))->OnHit(atObj);
    }

    void AddSubEntity(Entity* child) {
      subentities.push_back(child);
    }
    Entity* GetSubEntity(size_t entIdx) {
      if (subentities.empty()) return nullptr;
      return subentities[entIdx];
    }
    Entity* GetSubEntity(std::string entName) {
      if (subentities.empty()) return nullptr;
      for (Entity* subentity : subentities)
        if (subentity->name == entName) return subentity;
    }

    protected:
    BE<cXyz> position;
    BE<csXyz> angle;
    inline static bool generated[2] = { false, false };
    inline static char id[8];
    stage_actor_data_class params;
    std::vector<Entity*> subentities;
  };
}
