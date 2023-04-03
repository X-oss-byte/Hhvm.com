/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.service_schema;

import java.util.*;

public class PrimitivesServiceReactiveAsyncWrapper 
  implements PrimitivesService.Async {
  private PrimitivesService.Reactive _delegate;

  public PrimitivesServiceReactiveAsyncWrapper(PrimitivesService.Reactive _delegate) {
    
    this._delegate = _delegate;
  }

  public PrimitivesServiceReactiveAsyncWrapper(org.apache.thrift.ProtocolId _protocolId, reactor.core.publisher.Mono<? extends com.facebook.thrift.client.RpcClient> _rpcClient, Map<String, String> _headers, Map<String, String> _persistentHeaders) {
    this(new PrimitivesServiceReactiveClient(_protocolId, _rpcClient, _headers, _persistentHeaders));
  }

  @java.lang.Override
  public void close() {
    _delegate.dispose();
  }

  @java.lang.Override
  public com.google.common.util.concurrent.ListenableFuture<Long> init(final long param0, final long param1) {
      return com.facebook.thrift.util.FutureUtil.toListenableFuture(_delegate.init(param0, param1));
  }

  @java.lang.Override
  public com.google.common.util.concurrent.ListenableFuture<Long> init(
    final long param0,
    final long param1,
    com.facebook.thrift.client.RpcOptions rpcOptions) {
      return com.facebook.thrift.util.FutureUtil.toListenableFuture(_delegate.init(param0,param1, rpcOptions));
  }

  @java.lang.Override
  public com.google.common.util.concurrent.ListenableFuture<com.facebook.thrift.client.ResponseWrapper<Long>> initWrapper(
    final long param0,
    final long param1,
    com.facebook.thrift.client.RpcOptions rpcOptions) {
    return com.facebook.thrift.util.FutureUtil.toListenableFuture(_delegate.initWrapper(param0,param1, rpcOptions));
  }

}
