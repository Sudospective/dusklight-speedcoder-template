#pragma once

#include "f_op/f_op_actor.h"
#include "d/d_bg_s.h"

namespace DST {
  class WorldCollider {
    public:
    struct Config {
      f32 wall[2];
      f32 roofHeight;
      f32 waterOffset;
      int waterMode;
      bool lineCheck;
    };
    struct GroundResult {
      bool grounded;
      f32 height;
      int roomNo;
      s32 polyColor;
    };

    WorldCollider() {}
    explicit WorldCollider(Config cfg) : config(cfg) {}
    WorldCollider(const WorldCollider&) = delete;
    WorldCollider& operator=(const WorldCollider&) = delete;

    void SetConfig(Config cfg) {
      config = cfg;
    }
    void Init(fopAc_ac_c* actor) {
      acchCir.SetWall(config.wall[0], config.wall[1]);
      acch.Set(actor, 1, &acchCir);
      acch.ClrWaterNone();
      acch.SetRoofCrrHeight(config.roofHeight);
      acch.SetWaterCheckOffset(config.waterOffset);
      acch.SetWtrChkMode(config.waterMode);
      if (config.lineCheck)
        acch.OnLineCheck();
      else
        acch.OffLineCheck();
    }
    GroundResult Correct() {
      acch.CrrPos(dComIfG_Bgsp());
      return {
        .grounded = acch.GetGroundH() != -G_CM3D_F_INF,
        .height = acch.GetGroundH(),
        .roomNo = dComIfG_Bgsp().GetRoomId(acch.m_gnd),
        .polyColor = dComIfG_Bgsp().GetPolyColor(acch.m_gnd)
      };
    }
    f32 GetGroundH() {
      return acch.GetGroundH();
    }

    private:
    dBgS_ObjAcch acch;
    dBgS_AcchCir acchCir;
    Config config;
  };
}
