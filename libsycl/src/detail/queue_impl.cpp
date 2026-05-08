//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <detail/queue_impl.hpp>

#include <detail/device_impl.hpp>
#include <detail/event_impl.hpp>
#include <detail/global_objects.hpp>

_LIBSYCL_BEGIN_NAMESPACE_SYCL

namespace detail {

QueueImpl::QueueImpl(DeviceImpl &deviceImpl, const async_handler &asyncHandler,
                     const property_list &propList, PrivateTag)
    : MIsInorder(false), MAsyncHandler(asyncHandler), MPropList(propList),
      MDevice(deviceImpl),
      MContext(MDevice.getPlatformImpl().getDefaultContext()) {
  callAndThrow(olCreateQueue, MDevice.getOLHandle(), &MOffloadQueue);
}

QueueImpl::~QueueImpl() {
  // TODO: consider where to report errors
  if (MOffloadQueue)
    std::ignore = olDestroyQueue(MOffloadQueue);
}

backend QueueImpl::getBackend() const noexcept { return MDevice.getBackend(); }

static ol_device_handle_t getHostOLDevice() {
  auto HostDeviceRange = getOffloadTopologies()[OL_PLATFORM_BACKEND_HOST].getDevices(0);
  assert(HostDeviceRange.size() == 1);
  return *HostDeviceRange.begin();
}

// Returns the {DeviceHandle, IsHostDevice} pair associated with the ptr.
static std::pair<ol_device_handle_t, bool> getAllocDevice(const void *ptr) {
  ol_device_handle_t Device{};
  [[maybe_unused]] ol_result_t Result =
      callNoCheck(olGetMemInfo, ptr, OL_MEM_INFO_DEVICE,
                  sizeof(ol_device_handle_t), &Device);
  if (detail::isFailed(Result)) {
    // If liboffload could not find the allocation, assume it is a host one.
    if (Result->Code == OL_ERRC_NOT_FOUND) {
      return {getHostOLDevice(), true};
    }
    checkAndThrow(Result);
  }

  assert(Device);
  return {Device, false};
}

std::shared_ptr<EventImpl>
QueueImpl::memcpy(void *Dest, const void *Src, std::size_t NumBytes,
                  const std::vector<event> &DepEvents) {
  if (!Dest || !Src) {
    throw sycl::exception(sycl::make_error_code(sycl::errc::invalid),
                          "Unexpected nullptr");
  }

  auto [DestOLDevice, IsDestOLDeviceHost] = getAllocDevice(Dest);
  auto [SrcOLDevice, IsSrcOLDeviceHost] = getAllocDevice(Src);
  const bool HostToHostDeviceCopy = IsDestOLDeviceHost && IsSrcOLDeviceHost;
  // host-to-host copy?
  // TODO handle event dependencies
  callAndThrow(olMemcpy, HostToHostDeviceCopy ? nullptr : MOffloadQueue, Dest, DestOLDevice, Src, SrcOLDevice,
               NumBytes);
  ol_event_handle_t NewEvent{};
  callAndThrow(olCreateEvent, MOffloadQueue, &NewEvent);
  return EventImpl::createEventWithHandle(NewEvent, MDevice.getPlatformImpl());
}

void QueueImpl::wait() { callAndThrow(olSyncQueue, MOffloadQueue); }

} // namespace detail
_LIBSYCL_END_NAMESPACE_SYCL
