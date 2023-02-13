/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/mcpp2-compare/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/mcpp2-compare/gen-cpp2/EmptyService.h"
#include "thrift/compiler/test/fixtures/mcpp2-compare/gen-cpp2/EmptyService.tcc"
#include "thrift/compiler/test/fixtures/mcpp2-compare/gen-cpp2/module_metadata.h"
#include <thrift/lib/cpp2/gen/service_cpp.h>

std::unique_ptr<apache::thrift::AsyncProcessor> apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>::getProcessor() {
  return std::make_unique<::some::valid::ns::EmptyServiceAsyncProcessor>(this);
}

apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>::CreateMethodMetadataResult apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>::createMethodMetadata() {
  return ::apache::thrift::detail::ap::createMethodMetadataMap<::some::valid::ns::EmptyServiceAsyncProcessor>(getServiceRequestInfoMap().value().get());
}


std::optional<std::reference_wrapper<apache::thrift::ServiceRequestInfoMap const>> apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>::getServiceRequestInfoMap() const {
  return __fbthrift_serviceInfoHolder.requestInfoMap();
}

::some::valid::ns::EmptyServiceServiceInfoHolder apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>::__fbthrift_serviceInfoHolder;



namespace some { namespace valid { namespace ns {


const char* EmptyServiceAsyncProcessor::getServiceName() {
  return "EmptyService";
}

void EmptyServiceAsyncProcessor::getServiceMetadata(apache::thrift::metadata::ThriftServiceMetadataResponse& response) {
  ::apache::thrift::detail::md::ServiceMetadata<::apache::thrift::ServiceHandler<::some::valid::ns::EmptyService>>::gen(response);
}

void EmptyServiceAsyncProcessor::processSerializedCompressedRequestWithMetadata(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, const apache::thrift::AsyncProcessorFactory::MethodMetadata& methodMetadata, apache::thrift::protocol::PROTOCOL_TYPES protType, apache::thrift::Cpp2RequestContext* context, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  apache::thrift::detail::ap::process(this, iface_, std::move(req), std::move(serializedRequest), methodMetadata, protType, context, eb, tm);
}

void EmptyServiceAsyncProcessor::executeRequest(apache::thrift::ServerRequest&& request, const apache::thrift::AsyncProcessorFactory::MethodMetadata& methodMetadata) {
  apache::thrift::detail::ap::execute(this, std::move(request), apache::thrift::detail::ServerRequestHelper::protocol(request), methodMetadata);
}

const EmptyServiceAsyncProcessor::ProcessMap& EmptyServiceAsyncProcessor::getOwnProcessMap() {
  return kOwnProcessMap_;
}

const EmptyServiceAsyncProcessor::ProcessMap EmptyServiceAsyncProcessor::kOwnProcessMap_ {};

apache::thrift::ServiceRequestInfoMap const& EmptyServiceServiceInfoHolder::requestInfoMap() const {
  static folly::Indestructible<apache::thrift::ServiceRequestInfoMap> requestInfoMap{staticRequestInfoMap()};
  return *requestInfoMap;
}

apache::thrift::ServiceRequestInfoMap EmptyServiceServiceInfoHolder::staticRequestInfoMap() {
  apache::thrift::ServiceRequestInfoMap requestInfoMap = {
  };

  return requestInfoMap;
}
}}} // some::valid::ns
