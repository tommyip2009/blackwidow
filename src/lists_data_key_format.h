//  Copyright (c) 2017-present The blackwidow Authors.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#ifndef SRC_LISTS_DATA_KEY_FORMAT_H_
#define SRC_LISTS_DATA_KEY_FORMAT_H_

#include <string>

namespace blackwidow {
class ListsDataKey {
 public:
  ListsDataKey(const Slice& key, int32_t version, uint32_t index) :
    start_(nullptr), key_(key), version_(version), index_(index) {
  }

  ~ListsDataKey() {
    if (start_ != space_) {
      delete start_;
    }
  }

  const Slice Encode() {
    size_t usize = key_.size();
    size_t needed = usize + sizeof(int32_t) * 3;
    char* dst;
    if (needed <= sizeof(space_)) {
      dst = space_;
    } else {
      dst = new char[needed];
    }
    start_ = dst;
    EncodeFixed32(dst, key_.size());
    dst += sizeof(int32_t);
    memcpy(dst, key_.data(), key_.size());
    dst += key_.size();
    EncodeFixed32(dst, version_);
    dst += sizeof(int32_t);
    EncodeFixed32(dst, index_);
    return Slice(start_, needed);
  }

 private:
  char space_[200];
  char* start_;
  Slice key_;
  int32_t version_;
  uint32_t index_;
};

class ParsedListsDataKey {
 public:
  explicit ParsedListsDataKey(const std::string* key) {
    const char* ptr = key->data();
    int32_t key_len = DecodeFixed32(ptr);
    ptr += sizeof(int32_t);
    key_ = Slice(ptr, key_len);
    ptr += key_len;
    version_ = DecodeFixed32(ptr);
    ptr += sizeof(int32_t);
    index_ = DecodeFixed32(ptr);
  }

  explicit ParsedListsDataKey(const Slice& key) {
    const char* ptr = key.data();
    int32_t key_len = DecodeFixed32(ptr);
    ptr += sizeof(int32_t);
    key_ = Slice(ptr, key_len);
    ptr += key_len;
    version_ = DecodeFixed32(ptr);
    ptr += sizeof(int32_t);
    index_ = DecodeFixed32(ptr);
  }

  virtual ~ParsedListsDataKey() = default;

  Slice key() {
    return key_;
  }

  int32_t version() {
    return version_;
  }

  uint32_t index() {
    return index_;
  }

 private:
  std::string* raw_key_;
  Slice key_;
  int32_t version_;
  uint32_t index_;
};

}  //  namespace blackwidow
#endif  // SRC_LISTS_DATA_KEY_FORMAT_H_