/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.adapter;

import com.facebook.swift.codec.*;
import com.facebook.swift.codec.ThriftField.Requiredness;
import com.facebook.swift.codec.ThriftField.Recursiveness;
import com.google.common.collect.*;
import java.util.*;
import javax.annotation.Nullable;
import org.apache.thrift.*;
import org.apache.thrift.transport.*;
import org.apache.thrift.protocol.*;
import static com.google.common.base.MoreObjects.toStringHelper;
import static com.google.common.base.MoreObjects.ToStringHelper;

@SwiftGenerated
@com.facebook.swift.codec.ThriftStruct(value="MyAnnotation", builder=MyAnnotation.Builder.class)
public final class MyAnnotation implements com.facebook.thrift.payload.ThriftSerializable {

    @ThriftConstructor
    public MyAnnotation(
        @com.facebook.swift.codec.ThriftField(value=1, name="signature", requiredness=Requiredness.NONE) final String signature,
        @com.facebook.swift.codec.ThriftField(value=2, name="color", requiredness=Requiredness.NONE) final test.fixtures.adapter.Color color
    ) {
        this.signature = signature;
        this.color = color;
    }
    
    @ThriftConstructor
    protected MyAnnotation() {
      this.signature = null;
      this.color = test.fixtures.adapter.Color.RED;
    }
    
    public static class Builder {
    
        private String signature = null;
        private test.fixtures.adapter.Color color = test.fixtures.adapter.Color.RED;
    
        @com.facebook.swift.codec.ThriftField(value=1, name="signature", requiredness=Requiredness.NONE)
        public Builder setSignature(String signature) {
            this.signature = signature;
            return this;
        }
    
        public String getSignature() { return signature; }
    
            @com.facebook.swift.codec.ThriftField(value=2, name="color", requiredness=Requiredness.NONE)
        public Builder setColor(test.fixtures.adapter.Color color) {
            this.color = color;
            return this;
        }
    
        public test.fixtures.adapter.Color getColor() { return color; }
    
        public Builder() { }
        public Builder(MyAnnotation other) {
            this.signature = other.signature;
            this.color = other.color;
        }
    
        @ThriftConstructor
        public MyAnnotation build() {
            MyAnnotation result = new MyAnnotation (
                this.signature,
                this.color
            );
            return result;
        }
    }
    
    public static final Map<String, Integer> NAMES_TO_IDS = new HashMap();
    public static final Map<String, Integer> THRIFT_NAMES_TO_IDS = new HashMap();
    public static final Map<Integer, TField> FIELD_METADATA = new HashMap<>();
    private static final TStruct STRUCT_DESC = new TStruct("MyAnnotation");
    private final String signature;
    public static final int _SIGNATURE = 1;
    private static final TField SIGNATURE_FIELD_DESC = new TField("signature", TType.STRING, (short)1);
        private final test.fixtures.adapter.Color color;
    public static final int _COLOR = 2;
    private static final TField COLOR_FIELD_DESC = new TField("color", TType.I32, (short)2);
    static {
      NAMES_TO_IDS.put("signature", 1);
      THRIFT_NAMES_TO_IDS.put("signature", 1);
      FIELD_METADATA.put(1, SIGNATURE_FIELD_DESC);
      NAMES_TO_IDS.put("color", 2);
      THRIFT_NAMES_TO_IDS.put("color", 2);
      FIELD_METADATA.put(2, COLOR_FIELD_DESC);
      com.facebook.thrift.type.TypeRegistry.add(new com.facebook.thrift.type.Type(
        new com.facebook.thrift.type.UniversalName("facebook.com/thrift/test/MyAnnotation"), 
        MyAnnotation.class, MyAnnotation::read0));
    }
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=1, name="signature", requiredness=Requiredness.NONE)
    public String getSignature() { return signature; }
    
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=2, name="color", requiredness=Requiredness.NONE)
    public test.fixtures.adapter.Color getColor() { return color; }
    
    @java.lang.Override
    public String toString() {
        ToStringHelper helper = toStringHelper(this);
        helper.add("signature", signature);
        helper.add("color", color);
        return helper.toString();
    }
    
    @java.lang.Override
    public boolean equals(java.lang.Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
    
        MyAnnotation other = (MyAnnotation)o;
    
        return
            Objects.equals(signature, other.signature) &&
            Objects.equals(color, other.color) &&
            true;
    }
    
    @java.lang.Override
    public int hashCode() {
        return Arrays.deepHashCode(new java.lang.Object[] {
            signature,
            color
        });
    }
    
    
    public static com.facebook.thrift.payload.Reader<MyAnnotation> asReader() {
      return MyAnnotation::read0;
    }
    
    public static MyAnnotation read0(TProtocol oprot) throws TException {
      TField __field;
      oprot.readStructBegin(MyAnnotation.NAMES_TO_IDS, MyAnnotation.THRIFT_NAMES_TO_IDS, MyAnnotation.FIELD_METADATA);
      MyAnnotation.Builder builder = new MyAnnotation.Builder();
      while (true) {
        __field = oprot.readFieldBegin();
        if (__field.type == TType.STOP) { break; }
        switch (__field.id) {
        case _SIGNATURE:
          if (__field.type == TType.STRING) {
            String signature = oprot.readString();
            builder.setSignature(signature);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _COLOR:
          if (__field.type == TType.I32) {
            test.fixtures.adapter.Color color = test.fixtures.adapter.Color.fromInteger(oprot.readI32());
            builder.setColor(color);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        default:
          TProtocolUtil.skip(oprot, __field.type);
          break;
        }
        oprot.readFieldEnd();
      }
      oprot.readStructEnd();
      return builder.build();
    }
    
    public void write0(TProtocol oprot) throws TException {
      oprot.writeStructBegin(STRUCT_DESC);
      if (signature != null) {
        oprot.writeFieldBegin(SIGNATURE_FIELD_DESC);
        oprot.writeString(this.signature);
        oprot.writeFieldEnd();
      }
      oprot.writeFieldBegin(COLOR_FIELD_DESC);
      oprot.writeI32(this.color == null ? 0 : com.facebook.thrift.util.EnumUtil.getValue(this.color));
      oprot.writeFieldEnd();
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }
    
    private static class _MyAnnotationLazy {
        private static final MyAnnotation _DEFAULT = new MyAnnotation.Builder().build();
    }
    
    public static MyAnnotation defaultInstance() {
        return  _MyAnnotationLazy._DEFAULT;
    }
}
