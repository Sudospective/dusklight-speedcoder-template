# Dusklight Speedcoder Template (DST)

A header-only CMake template for writing native C++ mods for [Dusklight](https://github.com/TwilitRealm/dusklight), with typed, explicit C++ ergonomics over the stock SDK's plain-C service layer.

DST wraps the SDK's ABI, never replaces it: services stay plain C underneath, but your code works with scoped enums, typed arguments, RAII helpers, and classes with virtual lifecycle hooks instead of (or alongside) raw C structs and static trampolines.

## What you get

- **`DST::Component`** — the unit of a mod. Derive it in a class, override `OnInit` / `OnUpdate` / `OnShutdown`, and return it from `dst_main()`. Several components can live in one mod.
- **`DST::Entity`** — a game-world object layered over the engine's actor system via composition (a heap-side C++ object bridged to the engine's actor block), with virtual hooks: `OnCreate`, `OnExecute`, `OnDraw`, `OnDelete`, `OnCreateHeap`, `OnHit`. No need to write per-class static trampolines, as DST provides the engine callbacks. You also have a few callbacks you can use for creating a heap and hit detection (`CreateHeapCallback`, `HitCallback`).
- **Registration** — DST generates a unique internal engine name per entity type, builds the profile, and provides stage injection: the spawn record is generated for you and added to a stage with one call from your component's `OnInit`.
- **RAII helpers for the common actor machinery:**
  - `DST::Model` — archive loading, solid-heap model creation, matrix update, and the standard draw sequence.
  - `DST::SphereCollider` wrapping the Cc hit system (`SetStts`, callbacks, and per-frame registration included; hits arrive at `OnHit`).
  - `DST::WorldCollider` wrapping the Bg actor collision (ground, walls, ceiling, water), returning per-frame ground/room facts for your execute logic to apply.
- **Windows toolchain fixes** — a MinGW `modmeta` section fix and full static runtime linking under GCC; `/Zc:__cplusplus` under MSVC. See *Platform notes*.

## Prerequisites

- A C++20 compiler: MSVC on Windows, MinGW GCC support in progress.
- CMake 3.26 or newer.
- Git (the Dusklight sources are fetched at configure time).
- Dusklight itself, to run the mod.

## Quick start

```sh
cmake -B build
cmake --build build
```

Copy the resulting `.dusk` from `build/` into your Dusklight `mods/` folder and launch the game.

### Choosing the Dusklight version

The template fetches Dusklight at configure time, pinned by `DUSKLIGHT_VERSION` (default: the latest stable tag):

```sh
cmake -B build -DDUSKLIGHT_VERSION=v2.0.3   # a release tag, or a full commit hash
```

To build against a Dusklight checkout you already have:

```sh
cmake -B build -DDUSKLIGHT_DIR=C:/path/to/dusklight
```

Note: when `DUSKLIGHT_DIR` points at an existing checkout, the fetch is skipped and `DUSKLIGHT_VERSION` is ignored.

If configure fails with an SSL/certificate error while fetching, CMake can't find a CA bundle on your system; the template auto-detects the common locations (including Git for Windows). You can also point it at one explicitly with `-DCMAKE_TLS_CAINFO=path/to/ca-bundle.crt`.

## Project layout

```
src/
  dst.cpp       <- DST entry point (`dst_main`)
  mod.cpp       <- Mod entry point: no need to touch this
  components/
    example.hpp <- Small Component with init/shutdown
    mine.hpp    <- Example mine Component
  entities/
    example.hpp <- Small Entity with create/execute
    mine.hpp    <- Example mine Entity
include/dst/    <- DST headers for the template itself
```

IMPORTANT: **don't import ActorService in your own code.** The template's `mod.cpp` owns the one import; a second import in your code is a duplicate-definition link error. The headers give you everything you need.

## A minimal mod with an Entity

```cpp
// src/entities/example.hpp
#pragma once

#include "dst/entity.hpp"

#include "mods/svc/log.hpp"

class ExampleEnt : public DST::Entity {
  public:
  int OnCreate() {
    mods::log::info("Example Entity");
    return 4;
  }

  inline static DST::EntityProfile profile = {
    .group = fopAc_ACTOR_e,
    .status = fopAcStts_UNK_0x40000_e | fopAcStts_UNK_0x4000_e | fopAcStts_CULL_e,
    .priority_group = 7,
    .draw_priority = fpcDwPi_OBJ_LBOX_e,
    .cull_type = fopAc_CULLBOX_0_e,
  }
};
DST_DEFINE_ENTITY(ExampleEnt);

// src/components/example.hpp
#pragma once

#include "dst/component.hpp"

#include "entities/example.hpp"

class ExampleCmp : public DST::Component {
  public:
  ModResult OnInit() {
    DST_REGISTER_ENTITY(ExampleEnt);
    ExampleEnt e;
    AddEntity(e);
    return MOD_OK;
  }
  ModResult OnShutdown() {
    DST_UNREGISTER_ENTITY(ExampleEnt)
    return MOD_OK;
  }
};

// src/dst.cpp
#include "components/example.hpp"

std::unique_ptr<DST::Component> dst_main() {
  return std::make_unique<ExampleEnt>();
}
```

Register and place it from a component's `OnInit`: DST builds the stage record from the profile and the spawn position/angle you give it, and adds it to the stage — the loader creates the actor, with full stage context, when a player enters.

## Vocabulary

- **Mod** — the `.dusk` distributable. **Component** — a unit of mod code with the lifecycle above. **Entity** — a DST game-world object (the engine's own word is *actor*; `Entity` is the DST layer over it).
- **`ActorHandle`** identifies a *registration*; **`ActorId`** identifies a *live instance*; **`ProfileName`** is the numeric ID used to spawn. These are common to mix up.
- **Engine names** — every entity type gets a generated unique internal name (7 characters) for the engine's profile table; your class/profile name is the human-facing key. Explicit engine-name override is planned for the rare case of matching an actor in the game's shipped stage data.
- **`children`** on an Entity are DST-level composition — parts of your object that you drive. They are *not* engine child actors; engine children are created through the actor service and are engine-driven. This merely allow you to group Entities together, where calling `Create`/`Delete`/`Execute`/`Draw` on the parent Entity calls it on all child Entities. `Hit`/`CreateHeap` does not call on children.

## Platform notes

- **Windows / MinGW GCC**: Nearly supported. The SDK's metadata macros rely on `__declspec(allocate)`, which MinGW GCC ignores, so the template emits its metadata with GNU section attributes instead (guarded to Windows + GCC). The mod is also linked with the full static runtime (`-static`): the host is MSVC-built and does not ship the MinGW runtime DLLs.
- **Windows / MSVC**: Supported. The template sets `/Zc:__cplusplus`: without it, MSVC reports a pre-C++11 `__cplusplus` under C++20, which trips legacy compatibility shims in aurora's `dolphin/types.h` (`nullptr` defined as `NULL`, `override`/`final` defined away) and corrupts standard-library internals in translation units that mix game headers with the STL.
- **`FEATURES game` under MinGW**: On Windows the game import library is MSVC-mode, so GCC mods cannot link non-inline game C++ *class* symbols. DST's entity design works within that boundary (C services + header-inline game code). Mods that need to link game class implementations directly currently require MSVC or clang-cl on Windows; Linux builds are unaffected.
- **Android**: The code is portable C++; an Android build produces an ARM64 `.so` via the NDK, and CI merges all platforms into one `.dusk`. Don't ship the Windows DLL in a `.dusk` meant for Android.

## License

This template is released under the CC0 1.0 Universal license. See `LICENSE` in this repository for details. Dusklight itself is fetched at configure time and is not vendored here; it remains under its own license. The original template code has no license, so CC0 1.0 Universal was provided for this one. If the original template gains a license that prevents this template from being released under CC0, please contact Sudospective on Discord (@sudospective).
