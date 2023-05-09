

#pragma once

#include <fep3/fep3_errors.h>
#include "eclipse_cyclone_dds_include.h"

/**
 * Convert std timestamp into dds timestamp
 */
dds::core::Time convertTimestamp(const std::chrono::nanoseconds& timestamp);

/**
 * Convert dds timestamp into std timestamp
 */
std::chrono::nanoseconds convertTimestamp(const dds::core::Time& timestamp);

/**
 * Convert dds exception into fep3::Result
 */
fep3::Result convertExceptionToResult(const dds::core::Exception & exception);

/**
 * Convert std::exception into fep3::Result
 */
fep3::Result convertExceptionToResult(const std::exception& exception);
