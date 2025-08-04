//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of sycl device info types.
///
//===----------------------------------------------------------------------===//

#ifndef _LIBSYCL___IMPL_INFO_DEVICE_HPP
#define _LIBSYCL___IMPL_INFO_DEVICE_HPP

#include <cstdint>
#include <ur_api.h>

_LIBSYCL_BEGIN_NAMESPACE_SYCL

//A.3. Device information descriptors

namespace info {

// 4.6.4.7.1. Device type
enum class device_type : uint32_t {
  cpu = UR_DEVICE_TYPE_CPU,
  gpu = UR_DEVICE_TYPE_GPU,
  accelerator = UR_DEVICE_TYPE_FPGA,
  custom,
  automatic,
  host,
  all = UR_DEVICE_TYPE_ALL
};

// 4.6.4.7.2. Partition property
enum class partition_property : intptr_t {
  no_partition = 0,
  partition_equally = UR_DEVICE_PARTITION_EQUALLY,
  partition_by_counts = UR_DEVICE_PARTITION_BY_COUNTS,
  partition_by_affinity_domain = UR_DEVICE_PARTITION_BY_AFFINITY_DOMAIN,
  ext_intel_partition_by_cslice = UR_DEVICE_PARTITION_BY_CSLICE
};

// 4.6.4.7.3. Partition affinity domain
enum class partition_affinity_domain : intptr_t {
  not_applicable = 0,
  numa = UR_DEVICE_AFFINITY_DOMAIN_FLAG_NUMA,
  L4_cache = UR_DEVICE_AFFINITY_DOMAIN_FLAG_L4_CACHE,
  L3_cache = UR_DEVICE_AFFINITY_DOMAIN_FLAG_L3_CACHE,
  L2_cache = UR_DEVICE_AFFINITY_DOMAIN_FLAG_L2_CACHE,
  L1_cache = UR_DEVICE_AFFINITY_DOMAIN_FLAG_L1_CACHE,
  next_partitionable = UR_DEVICE_AFFINITY_DOMAIN_FLAG_NEXT_PARTITIONABLE
};

// 4.6.4.7.4. Floating point configuration
enum class fp_config : uint32_t {
  denorm = UR_DEVICE_FP_CAPABILITY_FLAG_DENORM,
  inf_nan = UR_DEVICE_FP_CAPABILITY_FLAG_INF_NAN,
  round_to_nearest = UR_DEVICE_FP_CAPABILITY_FLAG_ROUND_TO_NEAREST,
  round_to_zero = UR_DEVICE_FP_CAPABILITY_FLAG_ROUND_TO_ZERO,
  round_to_inf = UR_DEVICE_FP_CAPABILITY_FLAG_ROUND_TO_INF,
  fma = UR_DEVICE_FP_CAPABILITY_FLAG_FMA,
  correctly_rounded_divide_sqrt,
  soft_float
};

// 4.6.4.7.5. Local memory type
enum class local_mem_type : int { none, local, global };

// 4.6.4.7.6. Global memory cache type
enum class global_mem_cache_type : int { none, read_only, read_write };

// 4.6.4.7.7. Execution capability
// Deprecated by SYCL 2020.
enum class __SYCL2020_DEPRECATED("The info::execution_capability enumeration is deprecated by SYCL 2020") execution_capability : unsigned int {
  exec_kernel,
  exec_native_kernel
};

// ktikhomi: to be moved to a common place
#define __SYCL_PARAM_TRAITS_SPEC(DescType, Desc, ReturnT, UrCode)              \
  struct Desc {                                                                \
    using return_type = ReturnT;                                               \
  };

namespace device {

// 4.6.4.4. Information descriptors
template <int Dimensions = 3> struct max_work_item_sizes;

#define __SYCL_PARAM_TRAITS_DEPRECATED(Desc, Message)                        \
  struct __SYCL2020_DEPRECATED(Message) Desc;

#define __SYCL_PARAM_TRAITS_TEMPLATE_SPEC(DescType, Desc, ReturnT, UrCode)   \
template <> struct Desc {                                                    \
  using return_type = ReturnT;                                               \
};

#include <sycl/__impl/info/device_deprecated_2020.def>
#include <sycl/__impl/info/device.def>

#undef __SYCL_PARAM_TRAITS_TEMPLATE_SPEC
#undef __SYCL_PARAM_TRAITS_DEPRECATED
} // namespace device

} // namespace info

_LIBSYCL_END_NAMESPACE_SYCL

#endif // _LIBSYCL___IMPL_INFO_DEVICE_HPP
