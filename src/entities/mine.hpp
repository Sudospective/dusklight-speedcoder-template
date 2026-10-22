#pragma once

#include "dst/entity.hpp"
#include "dst/model.hpp"
#include "dst/sphere_collider.hpp"
#include "dst/world_collider.hpp"

#include "d/d_com_inf_game.h"
#include "res/Object/O_mD_jira.h"

class Mine : public DST::Entity {
  protected:
  DST::Model model;
  DST::SphereCollider sphere;
  DST::WorldCollider collider;
  Z2SoundObjSimple sound;
  s8 reverb;
  static constexpr u32 heap_size = ALIGN_NEXT(16832, 0x20);

  public:
  ~Mine() {
    sound.deleteObject();
  }
  int OnCreate() {
    mods::log::info("Mine OnCreate");
    cPhs_Step step = model.Load(actor, "O_mD_jira");

    if (step == cPhs_COMPLEATE_e) {
      collider.SetConfig({
        .wall = { 30.0f, 30.0f },
        .roofHeight = 60.0f,
        .waterOffset = 10000.0f,
        .waterMode = 2,
        .lineCheck = true,
      });
      collider.Init(actor);

      sphere.SetConfig({
        .atBit = true,
        .tgBit = false,
        .coBit = false,
        .weight = 30,
        .init = 0xFF,
      });
      sphere.Init(actor, {
        .mObjInf = {
          .mObj = {
            .mFlags = 0x0,
            .mSrcObjHitInf = {
              .mObjAt = {
                .mType = AT_TYPE_BOMB,
                .mAtp = 0x4,
                .mBase = {
                  .mSPrm = 0x1E,
                },
              },
              .mObjTg = {
                .mType = 0xD8FBFFEF,
                .mBase = {
                  .mSPrm = 0x11,
                },
              },
              .mObjCo = {
                .mBase = {
                  .mSPrm = 0x79,
                },
              },
            },
          },
          .mGObjAt = {
            .mSe = dCcD_SE_NONE,
            .mHitMark = 0x0,
            .mSpl = 0x1,
            .mMtrl = 0x0,
            .mBase = {
              .mGFlag = 0x0,
            },
          },
          .mGObjTg = {
            .mSe = dCcD_SE_NONE,
            .mHitMark = 0x0,
            .mSpl = 0x0,
            .mMtrl = 0x0,
            .mBase = {
              .mGFlag = 0x4,
            },
          },
          .mGObjCo = {
            .mBase = {
              .mGFlag = 0x0,
            },
          },
        },
        .mSphAttr = {
          .mSph = {
            .mCenter = { 0.0f, 0.0f, 0.0f },
            .mRadius = 80.0f,
          },
        },
      });

      SetCullBox(
        model.BaseMtx(),
        { -36.0f, 0.0f, -36.0f },
        { 36.0f, 66.0f, 36.0f }
      );

      Execute();
    }
    return step;
  }
  int IsDelete() {
    return 1;
  }
  int OnDelete() {
    return 1;
  }
  int OnExecute() {
    auto res = collider.Correct();
    sphere.Update(actor);
    if (res.grounded) {
      actor->tevStr.YukaCol = res.polyColor;
      actor->tevStr.room_no = res.roomNo;
      sphere.SetRoomId(res.roomNo);
      fopAcM_SetRoomNo(actor, res.roomNo);
      reverb = dComIfGp_getReverb(res.roomNo);
    }
    model.Update(actor);
    SyncAttention();
    return 1;
  }
  int OnDraw() {
    model.Draw(actor);
    return 1;
  }

  int OnCreateHeap() {
    mods::log::info("Mine OnCreateHeap");
    if (!model.Build(dRes_INDEX_O_MD_JIRA_BMD_O_MD_JIRAI_e)) return 0;
    sound.init(&actor->current.pos, 1);
    return 1;
  }
  void OnHit(dCcD_GObjInf*) {
    mods::log::info("Mine OnHit");
    static const u16 normalNameID[] = {
      0x161, 0x162, 0x163,
      0x164, 0x165, 0x166,
      0x167, 0x168, 0x1EC
    };
    for (size_t i = 0; i < ARRAY_SIZE(normalNameID); i++) {
      dComIfGp_particle_setColor(
        normalNameID[i],
        &actor->current.pos,
        &actor->tevStr,
        NULL,
        NULL,
        0.0f,
        0xFF,
        &actor->shape_angle,
        &actor->scale,
        NULL,
        -1,
        NULL
      );
    }
    sound.startSound(Z2SE_OBJ_BOMB_EXPLODE, 0, reverb);
    dComIfGp_getVibration().StartShock(4, 31, cXyz(0.0f, 1.0f, 0.0f));
    DeleteActor();
  }

  inline static DST::EntityProfile profile = {
    .group = fopAc_ACTOR_e,
    .status = fopAcStts_UNK_0x40000_e | fopAcStts_UNK_0x4000_e | fopAcStts_CULL_e,
    .priority_group = 7,
    .draw_priority = fpcDwPi_OBJ_LBOX_e,
    .cull_type = fopAc_CULLBOX_CUSTOM_e,
  };
};
DST_DEFINE_ENTITY(Mine);
