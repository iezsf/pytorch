#pragma once

// STOP!!! Thinking of including this header directly?  Please
// read Note [TH abstraction violation]

#include "THTensor.h"
#include "THStorage.hpp"

#include <atomic>
#include <ATen/ATen.h>

typedef struct THTensor
{
    int64_t *size;
    int64_t *stride;
    int64_t dim_;

    // Note: storage->size may be greater than the recorded size
    // of a tensor
    THStorage *storage;
    ptrdiff_t storageOffset;
    std::atomic<int> refcount;

    char flag;

    template <typename T>
    inline T * data() const {
      return storage->data<T>() + storageOffset;
    }

    template <typename T>
    inline T * unsafe_data() const {
      return storage->unsafe_data<T>() + storageOffset;
    }

    // [NOTE: _dim() vs dim()]
    // _dim() returns the "old" TH dimension view where no dimensions represents an empty tensor.
    // dim()  returns the ATen view of the dimensionality, i.e. 0-sized dimensions are supported.
    inline int64_t _dim() const {
      return is_empty() ? 0 : dim_;
    }

    inline int64_t dim() const {
      return dim_;
    }

    // represents that numel() == 0.
    inline bool is_empty() const {
      for (int64_t i = 0; i < dim_; ++i) {
        if (size[i] == 0) {
          return true;  
        }
      }
      return false;
    }

    inline at::IntList sizes() {
      return at::IntList(size, dim_);
    }
} THTensor;

#include "generic/THTensorFastGetSet.hpp"
#include "THGenerateAllTypes.h"

TH_API void THTensor_free(THTensor *self);
