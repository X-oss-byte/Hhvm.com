#
# Autogenerated by Thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#

import typing as _typing

import folly.iobuf as _fbthrift_iobuf
import thrift.py3.builder


import test.fixtures.basic.module.types as _test_fixtures_basic_module_types


class MyStruct_Builder(thrift.py3.builder.StructBuilder):
    MyIntField: _typing.Optional[int]
    MyStringField: _typing.Optional[str]
    MyDataField: _typing.Any
    myEnum: _typing.Optional[_test_fixtures_basic_module_types.MyEnum]
    oneway: _typing.Optional[bool]
    readonly: _typing.Optional[bool]
    idempotent: _typing.Optional[bool]
    floatSet: _typing.Optional[set]
    no_hack_codegen_field: _typing.Optional[str]

    def __iter__(self) -> _typing.Iterator[_typing.Tuple[str, _typing.Any]]: ...


class MyDataItem_Builder(thrift.py3.builder.StructBuilder):

    def __iter__(self) -> _typing.Iterator[_typing.Tuple[str, _typing.Any]]: ...


class MyUnion_Builder(thrift.py3.builder.StructBuilder):
    myEnum: _typing.Optional[_test_fixtures_basic_module_types.MyEnum]
    myStruct: _typing.Any
    myDataItem: _typing.Any
    floatSet: _typing.Optional[set]

    def __iter__(self) -> _typing.Iterator[_typing.Tuple[str, _typing.Any]]: ...


class ReservedKeyword_Builder(thrift.py3.builder.StructBuilder):
    reserved_field: _typing.Optional[int]

    def __iter__(self) -> _typing.Iterator[_typing.Tuple[str, _typing.Any]]: ...


class UnionToBeRenamed_Builder(thrift.py3.builder.StructBuilder):
    reserved_field: _typing.Optional[int]

    def __iter__(self) -> _typing.Iterator[_typing.Tuple[str, _typing.Any]]: ...


