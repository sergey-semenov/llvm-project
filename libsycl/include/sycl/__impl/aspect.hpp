//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the SYCL enum class aspect that identify characteristics of the device.
///
//===----------------------------------------------------------------------===//

#ifndef _LIBSYCL___IMPL_ASPECT_HPP
#define _LIBSYCL___IMPL_ASPECT_HPP

#include <sycl/__impl/detail/macro_definitions.hpp>

_LIBSYCL_BEGIN_NAMESPACE_SYCL

// 4.6.4.5. Aspects

#define __SYCL_ASPECT(ASPECT, ID) ASPECT = ID,
#define __SYCL_ASPECT_DEPRECATED(ASPECT, ID, MESSAGE)                          \
  ASPECT __SYCL2020_DEPRECATED(MESSAGE) = ID,

enum class aspect {
#include <sycl/info/aspects.def>
};

#undef __SYCL_ASPECT_DEPRECATED
#undef __SYCL_ASPECT

_LIBSYCL_END_NAMESPACE_SYCL

#endif // _LIBSYCL___IMPL_ASPECT_HPP
