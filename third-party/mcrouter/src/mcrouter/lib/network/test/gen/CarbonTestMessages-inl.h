/*
 *  Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 *  This source code is licensed under the MIT license found in the LICENSE
 *  file in the root directory of this source tree.
 *
 */

/*
 *  THIS FILE IS AUTOGENERATED. DO NOT MODIFY IT; ALL CHANGES WILL BE LOST IN
 *  VAIN.
 *
 *  @generated
 */

namespace facebook {
namespace memcache {
namespace test {
namespace thrift {

template <class Writer>
void ManyFields::serialize(Writer&& writer) const {
  writer.writeStructBegin();
  writer.writeField(1 /* field id */, buf1_ref());
  writer.writeField(2 /* field id */, buf2_ref());
  writer.writeField(3 /* field id */, buf3_ref());
  writer.writeField(4 /* field id */, buf4_ref());
  writer.writeField(5 /* field id */, buf5_ref());
  writer.writeField(6 /* field id */, buf6_ref());
  writer.writeField(7 /* field id */, buf7_ref());
  writer.writeField(8 /* field id */, buf8_ref());
  writer.writeField(9 /* field id */, buf9_ref());
  writer.writeField(10 /* field id */, buf10_ref());
  writer.writeField(11 /* field id */, buf11_ref());
  writer.writeField(12 /* field id */, buf12_ref());
  writer.writeField(13 /* field id */, buf13_ref());
  writer.writeField(14 /* field id */, buf14_ref());
  writer.writeField(15 /* field id */, buf15_ref());
  writer.writeField(16 /* field id */, buf16_ref());
  writer.writeField(17 /* field id */, buf17_ref());
  writer.writeField(18 /* field id */, buf18_ref());
  writer.writeField(19 /* field id */, buf19_ref());
  writer.writeField(20 /* field id */, buf20_ref());
  writer.writeField(21 /* field id */, buf21_ref());
  writer.writeField(22 /* field id */, buf22_ref());
  writer.writeField(23 /* field id */, buf23_ref());
  writer.writeField(24 /* field id */, buf24_ref());
  writer.writeField(25 /* field id */, buf25_ref());
  writer.writeField(26 /* field id */, buf26_ref());
  writer.writeField(27 /* field id */, buf27_ref());
  writer.writeField(28 /* field id */, buf28_ref());
  writer.writeField(29 /* field id */, buf29_ref());
  writer.writeField(30 /* field id */, buf30_ref());
  writer.writeField(31 /* field id */, buf31_ref());
  writer.writeField(32 /* field id */, buf32_ref());
  writer.writeField(33 /* field id */, buf33_ref());
  writer.writeField(34 /* field id */, buf34_ref());
  writer.writeField(35 /* field id */, buf35_ref());
  writer.writeField(36 /* field id */, buf36_ref());
  writer.writeField(37 /* field id */, buf37_ref());
  writer.writeField(38 /* field id */, buf38_ref());
  writer.writeField(39 /* field id */, buf39_ref());
  writer.writeField(40 /* field id */, buf40_ref());
  writer.writeFieldStop();
  writer.writeStructEnd();
}

template <class V>
void ManyFields::visitFields(V&& v) {
  if (!v.visitField(1, "buf1", *this->buf1_ref())) {
    return;
  }
  if (!v.visitField(2, "buf2", *this->buf2_ref())) {
    return;
  }
  if (!v.visitField(3, "buf3", *this->buf3_ref())) {
    return;
  }
  if (!v.visitField(4, "buf4", *this->buf4_ref())) {
    return;
  }
  if (!v.visitField(5, "buf5", *this->buf5_ref())) {
    return;
  }
  if (!v.visitField(6, "buf6", *this->buf6_ref())) {
    return;
  }
  if (!v.visitField(7, "buf7", *this->buf7_ref())) {
    return;
  }
  if (!v.visitField(8, "buf8", *this->buf8_ref())) {
    return;
  }
  if (!v.visitField(9, "buf9", *this->buf9_ref())) {
    return;
  }
  if (!v.visitField(10, "buf10", *this->buf10_ref())) {
    return;
  }
  if (!v.visitField(11, "buf11", *this->buf11_ref())) {
    return;
  }
  if (!v.visitField(12, "buf12", *this->buf12_ref())) {
    return;
  }
  if (!v.visitField(13, "buf13", *this->buf13_ref())) {
    return;
  }
  if (!v.visitField(14, "buf14", *this->buf14_ref())) {
    return;
  }
  if (!v.visitField(15, "buf15", *this->buf15_ref())) {
    return;
  }
  if (!v.visitField(16, "buf16", *this->buf16_ref())) {
    return;
  }
  if (!v.visitField(17, "buf17", *this->buf17_ref())) {
    return;
  }
  if (!v.visitField(18, "buf18", *this->buf18_ref())) {
    return;
  }
  if (!v.visitField(19, "buf19", *this->buf19_ref())) {
    return;
  }
  if (!v.visitField(20, "buf20", *this->buf20_ref())) {
    return;
  }
  if (!v.visitField(21, "buf21", *this->buf21_ref())) {
    return;
  }
  if (!v.visitField(22, "buf22", *this->buf22_ref())) {
    return;
  }
  if (!v.visitField(23, "buf23", *this->buf23_ref())) {
    return;
  }
  if (!v.visitField(24, "buf24", *this->buf24_ref())) {
    return;
  }
  if (!v.visitField(25, "buf25", *this->buf25_ref())) {
    return;
  }
  if (!v.visitField(26, "buf26", *this->buf26_ref())) {
    return;
  }
  if (!v.visitField(27, "buf27", *this->buf27_ref())) {
    return;
  }
  if (!v.visitField(28, "buf28", *this->buf28_ref())) {
    return;
  }
  if (!v.visitField(29, "buf29", *this->buf29_ref())) {
    return;
  }
  if (!v.visitField(30, "buf30", *this->buf30_ref())) {
    return;
  }
  if (!v.visitField(31, "buf31", *this->buf31_ref())) {
    return;
  }
  if (!v.visitField(32, "buf32", *this->buf32_ref())) {
    return;
  }
  if (!v.visitField(33, "buf33", *this->buf33_ref())) {
    return;
  }
  if (!v.visitField(34, "buf34", *this->buf34_ref())) {
    return;
  }
  if (!v.visitField(35, "buf35", *this->buf35_ref())) {
    return;
  }
  if (!v.visitField(36, "buf36", *this->buf36_ref())) {
    return;
  }
  if (!v.visitField(37, "buf37", *this->buf37_ref())) {
    return;
  }
  if (!v.visitField(38, "buf38", *this->buf38_ref())) {
    return;
  }
  if (!v.visitField(39, "buf39", *this->buf39_ref())) {
    return;
  }
  if (!v.visitField(40, "buf40", *this->buf40_ref())) {
    return;
  }
}

template <class V>
void ManyFields::visitFields(V&& v) const {
  if (!v.visitField(1, "buf1", *this->buf1_ref())) {
    return;
  }
  if (!v.visitField(2, "buf2", *this->buf2_ref())) {
    return;
  }
  if (!v.visitField(3, "buf3", *this->buf3_ref())) {
    return;
  }
  if (!v.visitField(4, "buf4", *this->buf4_ref())) {
    return;
  }
  if (!v.visitField(5, "buf5", *this->buf5_ref())) {
    return;
  }
  if (!v.visitField(6, "buf6", *this->buf6_ref())) {
    return;
  }
  if (!v.visitField(7, "buf7", *this->buf7_ref())) {
    return;
  }
  if (!v.visitField(8, "buf8", *this->buf8_ref())) {
    return;
  }
  if (!v.visitField(9, "buf9", *this->buf9_ref())) {
    return;
  }
  if (!v.visitField(10, "buf10", *this->buf10_ref())) {
    return;
  }
  if (!v.visitField(11, "buf11", *this->buf11_ref())) {
    return;
  }
  if (!v.visitField(12, "buf12", *this->buf12_ref())) {
    return;
  }
  if (!v.visitField(13, "buf13", *this->buf13_ref())) {
    return;
  }
  if (!v.visitField(14, "buf14", *this->buf14_ref())) {
    return;
  }
  if (!v.visitField(15, "buf15", *this->buf15_ref())) {
    return;
  }
  if (!v.visitField(16, "buf16", *this->buf16_ref())) {
    return;
  }
  if (!v.visitField(17, "buf17", *this->buf17_ref())) {
    return;
  }
  if (!v.visitField(18, "buf18", *this->buf18_ref())) {
    return;
  }
  if (!v.visitField(19, "buf19", *this->buf19_ref())) {
    return;
  }
  if (!v.visitField(20, "buf20", *this->buf20_ref())) {
    return;
  }
  if (!v.visitField(21, "buf21", *this->buf21_ref())) {
    return;
  }
  if (!v.visitField(22, "buf22", *this->buf22_ref())) {
    return;
  }
  if (!v.visitField(23, "buf23", *this->buf23_ref())) {
    return;
  }
  if (!v.visitField(24, "buf24", *this->buf24_ref())) {
    return;
  }
  if (!v.visitField(25, "buf25", *this->buf25_ref())) {
    return;
  }
  if (!v.visitField(26, "buf26", *this->buf26_ref())) {
    return;
  }
  if (!v.visitField(27, "buf27", *this->buf27_ref())) {
    return;
  }
  if (!v.visitField(28, "buf28", *this->buf28_ref())) {
    return;
  }
  if (!v.visitField(29, "buf29", *this->buf29_ref())) {
    return;
  }
  if (!v.visitField(30, "buf30", *this->buf30_ref())) {
    return;
  }
  if (!v.visitField(31, "buf31", *this->buf31_ref())) {
    return;
  }
  if (!v.visitField(32, "buf32", *this->buf32_ref())) {
    return;
  }
  if (!v.visitField(33, "buf33", *this->buf33_ref())) {
    return;
  }
  if (!v.visitField(34, "buf34", *this->buf34_ref())) {
    return;
  }
  if (!v.visitField(35, "buf35", *this->buf35_ref())) {
    return;
  }
  if (!v.visitField(36, "buf36", *this->buf36_ref())) {
    return;
  }
  if (!v.visitField(37, "buf37", *this->buf37_ref())) {
    return;
  }
  if (!v.visitField(38, "buf38", *this->buf38_ref())) {
    return;
  }
  if (!v.visitField(39, "buf39", *this->buf39_ref())) {
    return;
  }
  if (!v.visitField(40, "buf40", *this->buf40_ref())) {
    return;
  }
}

template <class Writer>
void McExpTestRequest::serialize(Writer&& writer) const {
  writer.writeStructBegin();
  writer.writeField(1 /* field id */, key_ref());
  writer.writeField(2 /* field id */, flags_ref());
  writer.writeField(3 /* field id */, deadlineMs_ref());
  writer.writeFieldStop();
  writer.writeStructEnd();
}

template <class V>
void McExpTestRequest::visitFields(V&& v) {
  if (!v.visitField(1, "key", *this->key_ref())) {
    return;
  }
  if (!v.visitField(2, "flags", *this->flags_ref())) {
    return;
  }
  if (!v.visitField(3, "deadlineMs", *this->deadlineMs_ref())) {
    return;
  }
}

template <class V>
void McExpTestRequest::visitFields(V&& v) const {
  if (!v.visitField(1, "key", *this->key_ref())) {
    return;
  }
  if (!v.visitField(2, "flags", *this->flags_ref())) {
    return;
  }
  if (!v.visitField(3, "deadlineMs", *this->deadlineMs_ref())) {
    return;
  }
}
} // namespace thrift
} // namespace test
} // namespace memcache
} // namespace facebook

namespace apache {
namespace thrift {
template <>
class Cpp2Ops<facebook::memcache::test::ManyFields> {
 public:
  typedef facebook::memcache::test::ManyFields Type;
  static constexpr protocol::TType thriftType() {
    return protocol::T_STRUCT;
  }
  template <class Protocol>
  static uint32_t write(Protocol* prot, const Type* value) {
    return value->write(prot);
  }
  template <class Protocol>
  static void read(Protocol* prot, Type* value) {
    value->read(prot);
  }
  template <class Protocol>
  static uint32_t serializedSize(Protocol* prot, const Type* value) {
    return value->serializedSize(prot);
  }
  template <class Protocol>
  static uint32_t serializedSizeZC(Protocol* prot, const Type* value) {
    return value->serializedSizeZC(prot);
  }
};

template <>
class Cpp2Ops<facebook::memcache::test::McExpTestRequest> {
 public:
  typedef facebook::memcache::test::McExpTestRequest Type;
  static constexpr protocol::TType thriftType() {
    return protocol::T_STRUCT;
  }
  template <class Protocol>
  static uint32_t write(Protocol* prot, const Type* value) {
    return value->write(prot);
  }
  template <class Protocol>
  static void read(Protocol* prot, Type* value) {
    value->read(prot);
  }
  template <class Protocol>
  static uint32_t serializedSize(Protocol* prot, const Type* value) {
    return value->serializedSize(prot);
  }
  template <class Protocol>
  static uint32_t serializedSizeZC(Protocol* prot, const Type* value) {
    return value->serializedSizeZC(prot);
  }
};
} // namespace thrift
} // namespace apache
