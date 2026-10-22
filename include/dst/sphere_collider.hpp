#pragma once

#include "f_op/f_op_actor.h"

#include "d/d_cc_d.h"

namespace DST {
  class SphereCollider {
    public:
    struct Config {
      bool atBit;
      bool tgBit;
      bool coBit;
      int weight;
      int init;
    };

    SphereCollider() {}
    explicit SphereCollider(Config cfg) : config(cfg) {}
    ~SphereCollider() {}
    SphereCollider(const SphereCollider&) = delete;
    SphereCollider& operator=(const SphereCollider&) = delete;

    void Init(fopAc_ac_c* actor, dCcD_SrcSph src) {
      ccStts.Init(30, 0xFF, actor);
      sphere.Set(src);
      sphere.SetStts(&ccStts);
      sphere.SetAtHitCallback(Entity::HitCallback);
      sphere.SetTgHitCallback(Entity::HitCallback);
      if (config.tgBit)
        sphere.OnTgSetBit();
      else
        sphere.OffTgSetBit();
      if (config.coBit)
        sphere.OnCoSetBit();
      else
        sphere.OffCoSetBit();
      if (config.atBit)
        sphere.OnAtSetBit();
      else
        sphere.OffAtSetBit();
    }
    void SetConfig(Config cfg) {
      config = cfg;
    }
    void Update(fopAc_ac_c* actor) {
      sphere.SetC(actor->current.pos);
      dComIfG_Ccsp()->Set(&sphere);
    }
    void SetRoomId(int id) {
      ccStts.SetRoomId(id);
    }

    private:
    dCcD_Sph sphere;
    dCcD_Stts ccStts;
    Config config;
  };
}

