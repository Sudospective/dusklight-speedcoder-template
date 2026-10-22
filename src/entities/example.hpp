#pragma once

#include "mods/svc/log.hpp"

#include "dst/entity.hpp"

class ExampleEntity : public DST::Entity {
  public:
  int OnCreate() {
    mods::log::info("Example Entity Create");
    return 1;
  }
  int IsDelete() {
    mods::log::info("Example Entity IsDelete");
    return 1;
  }
  int OnDelete() {
    mods::log::info("Example Entity Delete");
    return 1;
  };
  int OnExecute() {
    mods::log::info("Example Entity Execute");
    return 1;
  }
  int OnDraw() {
    return 1;
  }
  inline static DST::EntityProfile profile = {
    .group = fopAc_ACTOR_e,
    .status = fopAcStts_UNK_0x40000_e | fopAcStts_UNK_0x4000_e | fopAcStts_CULL_e,
    .priority_group = 7,
    .draw_priority = fpcDwPi_OBJ_LBOX_e,
    .cull_type = fopAc_CULLBOX_0_e,
  };
};
DST_DEFINE_ENTITY(ExampleEntity);
