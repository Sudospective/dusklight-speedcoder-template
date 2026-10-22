#pragma once

#include <vector>

#include "mods/svc/log.hpp"
#include "mods/svc/stage.h"

#include "dst/entity.hpp"

namespace DST {
  class Component {
    public:
    virtual ~Component() = default;
    virtual ModResult OnInit() { return MOD_OK; };
    virtual ModResult OnShutdown() { return MOD_OK; };
    virtual ModResult OnUpdate() { return MOD_OK; };

    ModResult Init() {
      return OnInit();
    }
    ModResult Shutdown() {
      ModResult res = OnShutdown();
      for (Entity* entity : entities) {
        delete entity;
      }
      return res;
    }
    ModResult Update() {
      return OnUpdate();
    }

    void AddEntity(Entity& entity) {
      const stage_actor_data_class params = entity.GetParams();
      if (svc_stage->add_actor(mod_ctx, "F_SP108", 0, -1, &params, sizeof(params), nullptr) != MOD_OK) {
        mods::log::error("Adding Entity failed");
        return;
      }
      entities.push_back(&entity);
      mods::log::info("Successfully added Entity");
    }

    protected:
    std::vector<Entity*> entities;
  };
}
