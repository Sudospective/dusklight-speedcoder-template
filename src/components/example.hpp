#pragma once

#include "mods/svc/log.hpp"

#include "dst/component.hpp"

#include "entities/example.hpp"

class ExampleComponent : public DST::Component {
  public:
  ModResult OnInit() {
    mods::log::info("Example Component Init");
    DST_REGISTER_ENTITY(ExampleEntity);
    ExampleEntity e;
    AddEntity(e);
    return MOD_OK;
  }
  ModResult OnShutdown() {
    mods::log::info("Example Component Shutdown");
    DST_UNREGISTER_ENTITY(ExampleEntity);
    return MOD_OK;
  }
  ModResult OnUpdate() {
    return MOD_OK;
  }
};
