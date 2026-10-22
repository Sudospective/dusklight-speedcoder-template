#pragma once

#include "dst/entity.hpp"

#include "d/d_com_inf_game.h"

namespace DST {
  class Model {
    public:
    struct Config {
      const char* archive;
      u32 res;
      u32 size;
      u32 createFlag = 0x80000;
      u32 mngFlag = 0x11000084;
      s32 lightType = 0x20;
    };

    Model() {}
    explicit Model(Config cfg) : config(cfg) {}
    ~Model() { dComIfG_resDelete(&phase, config.archive); }
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    /// @brief Load a model. Call this in OnCreate().
    /// @param actor An actor to load the model to
    /// @return The current phase step
    cPhs_Step Load(fopAc_ac_c* actor, const char* modelName) {
      config.archive = modelName;
      mods::log::info("Loading archive: {}", config.archive);
      cPhs_Step step = dComIfG_resLoad(&phase, config.archive);
      if (
        step == cPhs_COMPLEATE_e &&
        !fopAcM_entrySolidHeap(actor, Entity::CreateHeapCallback, config.size)
      ) return cPhs_ERROR_e;
      else if (step == cPhs_COMPLEATE_e) 
        mods::log::info("Model loaded");
      return step;
    }

    /// @brief Create the internal J3DModel. Call this in OnCreateHeap().
    /// @return `true` on success; `false` on failure
    bool Build(u32 modelRes) {
      mods::log::info("Building Model");
      config.res = modelRes;
      auto* data = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(config.archive, config.res)
      );
      if (data == nullptr) return false;
      model = mDoExt_J3DModel__create(data, config.createFlag, config.mngFlag);
      if (model) mods::log::info("Model build successful");
      return model != nullptr;
    }

    /// @brief Draw the model.
    /// @param actor The actor to draw the model to
    void Draw(fopAc_ac_c* actor) const {
      if (model == nullptr) return;
      g_env_light.settingTevStruct(config.lightType, &actor->current.pos, &actor->tevStr);
      g_env_light.setLightTevColorType_MAJI(model, &actor->tevStr);
      mDoExt_modelUpdateDL(model);
    }

    /// @brief Update the model's matrix.
    /// @param actor The actor to update the model to
    void Update(fopAc_ac_c* actor) {
      mDoMtx_stack_c::transS(
        actor->current.pos.x,
        actor->current.pos.y,
        actor->current.pos.z
      );
      mDoMtx_stack_c::ZXYrotM(actor->shape_angle);
      mDoMtx_stack_c::scaleM(actor->scale);
      model->setBaseTRMtx(mDoMtx_stack_c::get());
    }

    Mtx& BaseMtx() { return model->getBaseTRMtx(); }
    J3DModel* Get() const { return model; }

    private:
    Config config;
    request_of_phase_process_class phase {};
    J3DModel* model = nullptr;
  };
}
