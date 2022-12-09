/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/adapter/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/adapter/gen-cpp2/module_constants.h"

#include <thrift/lib/cpp2/gen/module_constants_cpp.h>


namespace facebook { namespace thrift { namespace test {

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::int32_t> const& module_constants::var1() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::int32_t>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::std::int32_t(static_cast<::std::int32_t>(10)),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("foo"))),
                                            "facebook.com/thrift/test/var1"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::string> const& module_constants::var2() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::string>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::std::string(apache::thrift::StringTraits<std::string>::fromStringLiteral("20")),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("bar"))),
                                            "facebook.com/thrift/test/var2"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::facebook::thrift::test::MyStruct> const& module_constants::var3() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::facebook::thrift::test::MyStruct>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::facebook::thrift::test::MyStruct(::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::MyStruct>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::field>(static_cast<::std::int32_t>(30)), ::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::set_string, ::my::Adapter2, ::apache::thrift::FieldId{2}>(std::initializer_list<::std::string>{apache::thrift::StringTraits<std::string>::fromStringLiteral("10"),
  apache::thrift::StringTraits<std::string>::fromStringLiteral("20")}))),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("baz"))),
                                            "facebook.com/thrift/test/var3"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::int32_t> const& module_constants::var4() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::int32_t>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::std::int32_t(static_cast<::std::int32_t>(40)),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("foo2"))),
                                            "facebook.com/thrift/test/var4"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::string> const& module_constants::var5() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::std::string>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::std::string(apache::thrift::StringTraits<std::string>::fromStringLiteral("50")),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("bar2"))),
                                            "facebook.com/thrift/test/var5"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::facebook::thrift::test::MyStruct> const& module_constants::var6() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<MyVarAdapter, ::facebook::thrift::test::MyStruct>> const instance{
      folly::factory_constructor, [](){ return MyVarAdapter::fromThrift(::facebook::thrift::test::MyStruct(::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::MyStruct>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::field>(static_cast<::std::int32_t>(60)), ::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::set_string, ::my::Adapter2, ::apache::thrift::FieldId{2}>(std::initializer_list<::std::string>{apache::thrift::StringTraits<std::string>::fromStringLiteral("30"),
  apache::thrift::StringTraits<std::string>::fromStringLiteral("40")}))),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Config>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::path>(apache::thrift::StringTraits<std::string>::fromStringLiteral("baz2"))),
                                            "facebook.com/thrift/test/var6"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::int32_t> const& module_constants::timeout() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::int32_t>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::std::int32_t(static_cast<::std::int32_t>(42)),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Person>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::name>(apache::thrift::StringTraits<std::string>::fromStringLiteral("Foo"))),
                                            "facebook.com/thrift/test/timeout"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::string> const& module_constants::msg() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::string>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::std::string(apache::thrift::StringTraits<std::string>::fromStringLiteral("hello, world")),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Person>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::name>(apache::thrift::StringTraits<std::string>::fromStringLiteral("Bar"))),
                                            "facebook.com/thrift/test/msg"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::facebook::thrift::test::Person2> const& module_constants::person() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::facebook::thrift::test::Person2>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::facebook::thrift::test::Person2(::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Person2>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::name>(apache::thrift::StringTraits<std::string>::fromStringLiteral("DefaultName")))),
                                            ::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Person>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::name>(apache::thrift::StringTraits<std::string>::fromStringLiteral("NameFromAnnotation"))),
                                            "facebook.com/thrift/test/person"); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::int32_t> const& module_constants::timeout_no_transitive() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::int32_t>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::std::int32_t(static_cast<::std::int32_t>(420))); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::string> const& module_constants::msg_no_transitive() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::std::string>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::std::string(apache::thrift::StringTraits<std::string>::fromStringLiteral("hello, world 2"))); }};
  return *instance;
}

::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::facebook::thrift::test::Person2> const& module_constants::person_no_transitive() {
  static folly::Indestructible<::apache::thrift::adapt_detail::adapted_t<::apache::thrift::test::VariableAdapter, ::facebook::thrift::test::Person2>> const instance{
      folly::factory_constructor, [](){ return ::apache::thrift::test::VariableAdapter::fromThrift(::facebook::thrift::test::Person2(::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::Person2>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::name>(apache::thrift::StringTraits<std::string>::fromStringLiteral("DefaultName 2"))))); }};
  return *instance;
}

::facebook::thrift::test::AdaptedBool const& module_constants::type_adapted() {
  static folly::Indestructible<::facebook::thrift::test::AdaptedBool> const instance{::apache::thrift::test::TemplatedTestAdapter::fromThrift(bool(true))};
  return *instance;
}

::facebook::thrift::test::MoveOnly const& module_constants::nested_adapted() {
  static folly::Indestructible<::facebook::thrift::test::MoveOnly> const instance{::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::MoveOnly>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::ptr, ::apache::thrift::test::MoveOnlyAdapter, ::apache::thrift::FieldId{1}>(::apache::thrift::detail::make_structured_constant<::facebook::thrift::test::detail::HeapAllocated>()))};
  return *instance;
}

::std::vector<::facebook::thrift::test::AdaptedByte> const& module_constants::container_of_adapted() {
  static folly::Indestructible<::std::vector<::facebook::thrift::test::AdaptedByte>> const instance{std::initializer_list<::facebook::thrift::test::AdaptedByte>{::apache::thrift::test::TemplatedTestAdapter::fromThrift(::std::int8_t(static_cast<::std::int8_t>(1))),
  ::apache::thrift::test::TemplatedTestAdapter::fromThrift(::std::int8_t(static_cast<::std::int8_t>(2))),
  ::apache::thrift::test::TemplatedTestAdapter::fromThrift(::std::int8_t(static_cast<::std::int8_t>(3)))}};
  return *instance;
}

}}} // facebook::thrift::test
