/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/py3/src/empty.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/py3/gen-py3cpp/NullService.h"
#include "thrift/compiler/test/fixtures/py3/gen-py3cpp/NullService.tcc"
#include "thrift/compiler/test/fixtures/py3/gen-py3cpp/empty_metadata.h"
#include <thrift/lib/cpp2/gen/service_cpp.h>

std::unique_ptr<apache::thrift::AsyncProcessor> apache::thrift::ServiceHandler<::cpp2::NullService>::getProcessor() {
  return std::make_unique<::cpp2::NullServiceAsyncProcessor>(this);
}

apache::thrift::ServiceHandler<::cpp2::NullService>::CreateMethodMetadataResult apache::thrift::ServiceHandler<::cpp2::NullService>::createMethodMetadata() {
  return ::apache::thrift::detail::ap::createMethodMetadataMap<::cpp2::NullServiceAsyncProcessor>(getServiceRequestInfoMap().value().get());
}


std::optional<std::reference_wrapper<apache::thrift::ServiceRequestInfoMap const>> apache::thrift::ServiceHandler<::cpp2::NullService>::getServiceRequestInfoMap() const {
  return __fbthrift_serviceInfoHolder.requestInfoMap();
}

::cpp2::NullServiceServiceInfoHolder apache::thrift::ServiceHandler<::cpp2::NullService>::__fbthrift_serviceInfoHolder;



namespace cpp2 {


const char* NullServiceAsyncProcessor::getServiceName() {
  return "NullService";
}

void NullServiceAsyncProcessor::getServiceMetadata(apache::thrift::metadata::ThriftServiceMetadataResponse& response) {
  ::apache::thrift::detail::md::ServiceMetadata<::apache::thrift::ServiceHandler<::cpp2::NullService>>::gen(response);
}

void NullServiceAsyncProcessor::processSerializedCompressedRequestWithMetadata(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, const apache::thrift::AsyncProcessorFactory::MethodMetadata& methodMetadata, apache::thrift::protocol::PROTOCOL_TYPES protType, apache::thrift::Cpp2RequestContext* context, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  apache::thrift::detail::ap::process(this, iface_, std::move(req), std::move(serializedRequest), methodMetadata, protType, context, eb, tm);
}

void NullServiceAsyncProcessor::executeRequest(apache::thrift::ServerRequest&& request, const apache::thrift::AsyncProcessorFactory::MethodMetadata& methodMetadata) {
  apache::thrift::detail::ap::execute(this, std::move(request), apache::thrift::detail::ServerRequestHelper::protocol(request), methodMetadata);
}

const NullServiceAsyncProcessor::ProcessMap& NullServiceAsyncProcessor::getOwnProcessMap() {
  return kOwnProcessMap_;
}

const NullServiceAsyncProcessor::ProcessMap NullServiceAsyncProcessor::kOwnProcessMap_ {};

apache::thrift::ServiceRequestInfoMap const& NullServiceServiceInfoHolder::requestInfoMap() const {
  static folly::Indestructible<apache::thrift::ServiceRequestInfoMap> requestInfoMap{staticRequestInfoMap()};
  return *requestInfoMap;
}

apache::thrift::ServiceRequestInfoMap NullServiceServiceInfoHolder::staticRequestInfoMap() {
  apache::thrift::ServiceRequestInfoMap requestInfoMap = {
  };

  return requestInfoMap;
}
} // cpp2
