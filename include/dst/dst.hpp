#pragma once

#if defined(_WIN32) && defined(__GNUC__) && !defined(__clang__)
#include <bits/unique_ptr.h>
#endif

#include "dst/component.hpp"

#if defined(_WIN32) && defined(__GNUC__) && !defined(__clang__)
#define DST_META_RECORD __attribute__((section("modmeta$d"), used))
#define DST_META_BEGIN (&dst_meta_begin)
#define DST_META_END (&dst_meta_end)
#define DST_IMPORT_SERVICE_EX( \
    service_type, variable, service_id_value, major_value, min_minor_value, flags_value) \
    const service_type* variable = nullptr; \
    DST_META_RECORD static constinit ModMetaImport mod_meta_import_##variable = { \
        {sizeof(ModMetaImport), MOD_META_IMPORT, static_cast<uint8_t>(flags_value)}, \
        static_cast<uint16_t>(major_value), \
        static_cast<uint16_t>(min_minor_value), \
        &(variable), \
        ::mods::detail::make_service_id(service_id_value), \
    }
    
#define DST_IMPORT_SERVICE_VERSION(service_type, variable, min_minor_value)                            \
    DST_IMPORT_SERVICE_EX(service_type, variable, ::mods::ServiceTraits<service_type>::id,             \
        ::mods::ServiceTraits<service_type>::major_version, min_minor_value,                       \
        SERVICE_IMPORT_REQUIRED)

#define DST_IMPORT_SERVICE(service_type, variable)                                                     \
    DST_IMPORT_SERVICE_VERSION(                                                                        \
        service_type, variable, ::mods::ServiceTraits<service_type>::minor_version)

#define DST_IMPORT_OPTIONAL_SERVICE_VERSION(service_type, variable, min_minor_value)                   \
    DST_IMPORT_SERVICE_EX(service_type, variable, ::mods::ServiceTraits<service_type>::id,             \
        ::mods::ServiceTraits<service_type>::major_version, min_minor_value,                       \
        SERVICE_IMPORT_OPTIONAL)

#define DST_IMPORT_OPTIONAL_SERVICE(service_type, variable)                                            \
    DST_IMPORT_OPTIONAL_SERVICE_VERSION(                                                               \
        service_type, variable, ::mods::ServiceTraits<service_type>::minor_version)

#define DST_EXPORT_SERVICE_AS(instance, service_id_value)                                              \
    DST_META_RECORD static constinit ModMetaExport mod_meta_export_##instance = {                  \
        {sizeof(ModMetaExport), MOD_META_EXPORT, SERVICE_EXPORT_STATIC},                           \
        (instance).header.major_version,                                                           \
        (instance).header.minor_version,                                                           \
        &(instance),                                                                               \
        ::mods::detail::make_service_id(service_id_value),                                         \
    }
#define DST_EXPORT_SERVICE(instance)                                                                   \
    DST_EXPORT_SERVICE_AS(instance, ::mods::ServiceTraits<std::remove_cv_t<decltype(instance)>>::id)

#define DST_EXPORT_DEFERRED_SERVICE(token, service_id_value, major_value, minor_value)                 \
    DST_META_RECORD static constinit ModMetaExport mod_meta_export_##token = {                     \
        {sizeof(ModMetaExport), MOD_META_EXPORT, SERVICE_EXPORT_DEFERRED},                         \
        static_cast<uint16_t>(major_value),                                                        \
        static_cast<uint16_t>(minor_value),                                                        \
        nullptr,                                                                                   \
        ::mods::detail::make_service_id(service_id_value),                                         \
    }
#else
#define DST_META_RECORD MOD_META_RECORD
#define DST_META_BEGIN MOD_META_BOUNDS_BEGIN
#define DST_META_END MOD_META_BOUNDS_END
#define DST_NEEDS_SDK_BOUNDS 1
#define DST_IMPORT_SERVICE IMPORT_SERVICE
#endif

namespace DST {}

std::unique_ptr<DST::Component> dst_main();
