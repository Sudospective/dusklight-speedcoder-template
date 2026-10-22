#pragma once

#include "mods/svc/log.hpp"

#include "dst/component.hpp"

#include "entities/mine.hpp"

class MineCmp : public DST::Component {
  public:
  ModResult OnInit() {
    mods::log::info("Mine component init");
    DST_REGISTER_ENTITY(Mine);
    Mine mine;
    BE<cXyz> pos = {-14324.0f, 0.0f, 341.0f};
    BE<csXyz> ang = {0x2000, static_cast<s16>(-14335.0f * 100000), 0};
    mine.Position(pos);
    mine.Angle(ang);
    AddEntity(mine);
    return MOD_OK;
  }
  ModResult OnShutdown() {
    mods::log::info("Mine component shutdown");
    DST_UNREGISTER_ENTITY(Mine);
    return MOD_OK;
  }
  ModResult OnUpdate() {
    return MOD_OK;
  }
};
