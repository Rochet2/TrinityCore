// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ets_types.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ets_types.pb.h"

#include <utility>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace bgs {
namespace protocol {

namespace {

const ::google::protobuf::Descriptor* TimeSeriesId_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TimeSeriesId_reflection_ = NULL;

}  // namespace

void protobuf_AssignDesc_ets_5ftypes_2eproto() {
  protobuf_AddDesc_ets_5ftypes_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ets_types.proto");
  GOOGLE_CHECK(file != NULL);
  TimeSeriesId_descriptor_ = file->message_type(0);
  static const int TimeSeriesId_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeSeriesId, epoch_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeSeriesId, position_),
  };
  TimeSeriesId_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      TimeSeriesId_descriptor_,
      TimeSeriesId::default_instance_,
      TimeSeriesId_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeSeriesId, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimeSeriesId, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(TimeSeriesId));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ets_5ftypes_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    TimeSeriesId_descriptor_, &TimeSeriesId::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ets_5ftypes_2eproto() {
  delete TimeSeriesId::default_instance_;
  delete TimeSeriesId_reflection_;
}

void protobuf_AddDesc_ets_5ftypes_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017ets_types.proto\022\014bgs.protocol\"/\n\014TimeS"
    "eriesId\022\r\n\005epoch\030\001 \001(\004\022\020\n\010position\030\002 \001(\004"
    "B\002H\002", 84);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ets_types.proto", &protobuf_RegisterTypes);
  TimeSeriesId::default_instance_ = new TimeSeriesId();
  TimeSeriesId::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ets_5ftypes_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ets_5ftypes_2eproto {
  StaticDescriptorInitializer_ets_5ftypes_2eproto() {
    protobuf_AddDesc_ets_5ftypes_2eproto();
  }
} static_descriptor_initializer_ets_5ftypes_2eproto_;
// ===================================================================

#ifndef _MSC_VER
const int TimeSeriesId::kEpochFieldNumber;
const int TimeSeriesId::kPositionFieldNumber;
#endif  // !_MSC_VER

TimeSeriesId::TimeSeriesId()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.TimeSeriesId)
}

void TimeSeriesId::InitAsDefaultInstance() {
}

TimeSeriesId::TimeSeriesId(const TimeSeriesId& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.TimeSeriesId)
}

void TimeSeriesId::SharedCtor() {
  _cached_size_ = 0;
  epoch_ = GOOGLE_ULONGLONG(0);
  position_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

TimeSeriesId::~TimeSeriesId() {
  // @@protoc_insertion_point(destructor:bgs.protocol.TimeSeriesId)
  SharedDtor();
}

void TimeSeriesId::SharedDtor() {
  if (this != default_instance_) {
  }
}

void TimeSeriesId::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TimeSeriesId::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TimeSeriesId_descriptor_;
}

const TimeSeriesId& TimeSeriesId::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ets_5ftypes_2eproto();
  return *default_instance_;
}

TimeSeriesId* TimeSeriesId::default_instance_ = NULL;

TimeSeriesId* TimeSeriesId::New() const {
  return new TimeSeriesId;
}

void TimeSeriesId::Swap(TimeSeriesId* other) {
  if (other != this) {
    GetReflection()->Swap(this, other);}
}

::google::protobuf::Metadata TimeSeriesId::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = TimeSeriesId_descriptor_;
  metadata.reflection = TimeSeriesId_reflection_;
  return metadata;
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)