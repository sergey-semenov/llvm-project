// REQUIRES: any-device
// RUN: %clangxx %sycl_options %s -o %t.out
// RUN: %t.out

#include <sycl/sycl.hpp>

#include <cstddef>
#include <numeric>
#include <tuple>

using namespace sycl;

constexpr std::size_t DataSize = 1024;
constexpr std::size_t NumBytes = DataSize * sizeof(int);

// Assumes that the first and the last allocas in the tuple will be accessible
// on host.
template <typename... AllocFsT>
void test(queue &Q, std::tuple<AllocFsT...> AllocFs) {
  constexpr std::size_t NAllocations = std::tuple_size_v<decltype(AllocFs)>;
  static_assert(NAllocations > 1);

  std::vector<std::shared_ptr<int>> PtrPipeline;
  PtrPipeline.reserve(NAllocations);
  std::apply([&](auto &&...Fs) { ((PtrPipeline.push_back(Fs())), ...); },
             AllocFs);

  //std::iota(PtrPipeline[0].get(), PtrPipeline[0].get() + DataSize, 0);

  // TODO handle dependencies
  for (int I = 0; I < NAllocations - 1; ++I) {
    Q.memcpy(PtrPipeline[I + 1].get(), PtrPipeline[I].get(), NumBytes);
  }

  Q.wait();

  int *ResultPtr = PtrPipeline[NAllocations - 1].get();
  //for (int I = 0; I < DataSize; ++I)
  //  assert(ResultPtr[I] == I);
}

int main() {
  queue Q;

  auto HostAllocF = [&]() {
    return std::shared_ptr<int>(new int[DataSize],
                                [&](int *Ptr) { delete[] Ptr; });
  };
  auto USMDeleter = [&](int *Ptr) { free(Ptr, Q); };
  auto HostUSMAllocF = [&]() {
    return std::shared_ptr<int>(malloc_host<int>(DataSize, Q), USMDeleter);
  };
  auto DeviceUSMAllocF = [&]() {
    return std::shared_ptr<int>(malloc_device<int>(DataSize, Q), USMDeleter);
  };
  auto SharedUSMAllocF = [&]() {
    return std::shared_ptr<int>(malloc_shared<int>(DataSize, Q), USMDeleter);
  };

  // Device USM to host copy leads to memory corruption in liboffload
  test(Q, std::tuple(DeviceUSMAllocF, HostAllocF));

  return 0;
}
