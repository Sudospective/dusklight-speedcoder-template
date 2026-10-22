#include "mods/service.hpp"
#include "mods/svc/actor.h"
#include "mods/svc/hook.hpp"
#include "mods/svc/log.hpp"
#include "mods/svc/stage.h"

#include "dst/dst.hpp"

// This is Dusklight's entry point for a mod.
// You don't need to do anything in here.
// You can find the proper entry point in dst_main.cpp.

// DEFINE_MOD() is what usually goes here.
// As of v2.0.3, it causes a linkage error in GCC.
// The following code is placed so that GCC is
// able to properly build a bare mod.

// DEFINE_MOD();

#if defined(WIN32) && defined(__GNUC__) && !defined(__clang__)
extern "C" { \
__attribute__((section("modmeta$a"))) constinit unsigned long long dst_meta_begin = 0; \
__attribute__((section("modmeta$z"))) constinit unsigned long long dst_meta_end = 0; \
}
#endif

extern "C" {
MOD_EXPORT ModContext* mod_ctx = nullptr;
}

#if defined(DST_NEEDS_SDK_BOUNDS)
MOD_META_BOUNDS_DEFN
#endif

DST_META_RECORD static constinit ModMetaHeader mod_meta_header =
    ::mods::detail::make_header();

extern "C" {
MOD_EXPORT extern constinit const ModMeta mod_meta = {
    sizeof(ModMeta),
    DST_META_BEGIN,
    DST_META_END,
};
}

DST_IMPORT_SERVICE(LogService, svc_log);
DST_IMPORT_SERVICE(HookService, svc_hook);
DST_IMPORT_SERVICE(ActorService, svc_actor);
DST_IMPORT_SERVICE(StageService, svc_stage);

static std::unique_ptr<DST::Component> dst_cmp = nullptr;

extern "C" {
MOD_EXPORT ModResult mod_initialize(ModError*) {
    dst_cmp = dst_main();
    return dst_cmp ? dst_cmp->Init() : MOD_ERROR;
}

MOD_EXPORT ModResult mod_update(ModError*) {
    return dst_cmp ? dst_cmp->Update() : MOD_ERROR;
}

MOD_EXPORT ModResult mod_shutdown(ModError*) {
    return dst_cmp ? dst_cmp->Shutdown() : MOD_ERROR;
}
}
