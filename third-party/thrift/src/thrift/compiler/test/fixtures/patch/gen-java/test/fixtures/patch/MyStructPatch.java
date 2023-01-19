/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.patch;

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
@com.facebook.swift.codec.ThriftStruct(value="MyStructPatch", builder=MyStructPatch.Builder.class)
public final class MyStructPatch implements com.facebook.thrift.payload.ThriftSerializable {

    @ThriftConstructor
    public MyStructPatch(
        @com.facebook.swift.codec.ThriftField(value=1, name="assign", requiredness=Requiredness.OPTIONAL) final test.fixtures.patch.MyStruct assign,
        @com.facebook.swift.codec.ThriftField(value=2, name="clear", requiredness=Requiredness.TERSE) final boolean clear,
        @com.facebook.swift.codec.ThriftField(value=3, name="patchPrior", requiredness=Requiredness.TERSE) final test.fixtures.patch.MyStructFieldPatch patchPrior,
        @com.facebook.swift.codec.ThriftField(value=5, name="ensure", requiredness=Requiredness.TERSE) final test.fixtures.patch.MyStructEnsureStruct ensure,
        @com.facebook.swift.codec.ThriftField(value=6, name="patch", requiredness=Requiredness.TERSE) final test.fixtures.patch.MyStructFieldPatch patch
    ) {
        this.assign = assign;
        this.clear = clear;
        this.patchPrior = patchPrior;
        this.ensure = ensure;
        this.patch = patch;
    }
    
    @ThriftConstructor
    protected MyStructPatch() {
      this.assign = null;
      this.clear = false;
      this.patchPrior = test.fixtures.patch.MyStructFieldPatch.defaultInstance();
      this.ensure = test.fixtures.patch.MyStructEnsureStruct.defaultInstance();
      this.patch = test.fixtures.patch.MyStructFieldPatch.defaultInstance();
    }
    
    public static class Builder {
    
        private test.fixtures.patch.MyStruct assign = null;
        private boolean clear = false;
        private test.fixtures.patch.MyStructFieldPatch patchPrior = test.fixtures.patch.MyStructFieldPatch.defaultInstance();
        private test.fixtures.patch.MyStructEnsureStruct ensure = test.fixtures.patch.MyStructEnsureStruct.defaultInstance();
        private test.fixtures.patch.MyStructFieldPatch patch = test.fixtures.patch.MyStructFieldPatch.defaultInstance();
    
        @com.facebook.swift.codec.ThriftField(value=1, name="assign", requiredness=Requiredness.OPTIONAL)
        public Builder setAssign(test.fixtures.patch.MyStruct assign) {
            this.assign = assign;
            return this;
        }
    
        public test.fixtures.patch.MyStruct getAssign() { return assign; }
    
            @com.facebook.swift.codec.ThriftField(value=2, name="clear", requiredness=Requiredness.TERSE)
        public Builder setClear(boolean clear) {
            this.clear = clear;
            return this;
        }
    
        public boolean isClear() { return clear; }
    
            @com.facebook.swift.codec.ThriftField(value=3, name="patchPrior", requiredness=Requiredness.TERSE)
        public Builder setPatchPrior(test.fixtures.patch.MyStructFieldPatch patchPrior) {
            this.patchPrior = patchPrior;
            return this;
        }
    
        public test.fixtures.patch.MyStructFieldPatch getPatchPrior() { return patchPrior; }
    
            @com.facebook.swift.codec.ThriftField(value=5, name="ensure", requiredness=Requiredness.TERSE)
        public Builder setEnsure(test.fixtures.patch.MyStructEnsureStruct ensure) {
            this.ensure = ensure;
            return this;
        }
    
        public test.fixtures.patch.MyStructEnsureStruct getEnsure() { return ensure; }
    
            @com.facebook.swift.codec.ThriftField(value=6, name="patch", requiredness=Requiredness.TERSE)
        public Builder setPatch(test.fixtures.patch.MyStructFieldPatch patch) {
            this.patch = patch;
            return this;
        }
    
        public test.fixtures.patch.MyStructFieldPatch getPatch() { return patch; }
    
        public Builder() { }
        public Builder(MyStructPatch other) {
            this.assign = other.assign;
            this.clear = other.clear;
            this.patchPrior = other.patchPrior;
            this.ensure = other.ensure;
            this.patch = other.patch;
        }
    
        @ThriftConstructor
        public MyStructPatch build() {
            MyStructPatch result = new MyStructPatch (
                this.assign,
                this.clear,
                this.patchPrior,
                this.ensure,
                this.patch
            );
            return result;
        }
    }
        
    public static final Map<String, Integer> NAMES_TO_IDS = new HashMap();
    public static final Map<String, Integer> THRIFT_NAMES_TO_IDS = new HashMap();
    public static final Map<Integer, TField> FIELD_METADATA = new HashMap<>();
    private static final TStruct STRUCT_DESC = new TStruct("MyStructPatch");
    private final test.fixtures.patch.MyStruct assign;
    public static final int _ASSIGN = 1;
    private static final TField ASSIGN_FIELD_DESC = new TField("assign", TType.STRUCT, (short)1);
        private final boolean clear;
    public static final int _CLEAR = 2;
    private static final TField CLEAR_FIELD_DESC = new TField("clear", TType.BOOL, (short)2);
        private final test.fixtures.patch.MyStructFieldPatch patchPrior;
    public static final int _PATCHPRIOR = 3;
    private static final TField PATCH_PRIOR_FIELD_DESC = new TField("patchPrior", TType.STRUCT, (short)3);
        private final test.fixtures.patch.MyStructEnsureStruct ensure;
    public static final int _ENSURE = 5;
    private static final TField ENSURE_FIELD_DESC = new TField("ensure", TType.STRUCT, (short)5);
        private final test.fixtures.patch.MyStructFieldPatch patch;
    public static final int _PATCH = 6;
    private static final TField PATCH_FIELD_DESC = new TField("patch", TType.STRUCT, (short)6);
    static {
      NAMES_TO_IDS.put("assign", 1);
      THRIFT_NAMES_TO_IDS.put("assign", 1);
      FIELD_METADATA.put(1, ASSIGN_FIELD_DESC);
      NAMES_TO_IDS.put("clear", 2);
      THRIFT_NAMES_TO_IDS.put("clear", 2);
      FIELD_METADATA.put(2, CLEAR_FIELD_DESC);
      NAMES_TO_IDS.put("patchPrior", 3);
      THRIFT_NAMES_TO_IDS.put("patchPrior", 3);
      FIELD_METADATA.put(3, PATCH_PRIOR_FIELD_DESC);
      NAMES_TO_IDS.put("ensure", 5);
      THRIFT_NAMES_TO_IDS.put("ensure", 5);
      FIELD_METADATA.put(5, ENSURE_FIELD_DESC);
      NAMES_TO_IDS.put("patch", 6);
      THRIFT_NAMES_TO_IDS.put("patch", 6);
      FIELD_METADATA.put(6, PATCH_FIELD_DESC);
      com.facebook.thrift.type.TypeRegistry.add(new com.facebook.thrift.type.Type(
        new com.facebook.thrift.type.UniversalName("test.dev/fixtures/patch/MyStructPatch"), 
        MyStructPatch.class, MyStructPatch::read0));
    }
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=1, name="assign", requiredness=Requiredness.OPTIONAL)
    public test.fixtures.patch.MyStruct getAssign() { return assign; }
    
    
    
    @com.facebook.swift.codec.ThriftField(value=2, name="clear", requiredness=Requiredness.TERSE)
    public boolean isClear() { return clear; }
    
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=3, name="patchPrior", requiredness=Requiredness.TERSE)
    public test.fixtures.patch.MyStructFieldPatch getPatchPrior() { return patchPrior; }
    
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=5, name="ensure", requiredness=Requiredness.TERSE)
    public test.fixtures.patch.MyStructEnsureStruct getEnsure() { return ensure; }
    
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=6, name="patch", requiredness=Requiredness.TERSE)
    public test.fixtures.patch.MyStructFieldPatch getPatch() { return patch; }
    
    @java.lang.Override
    public String toString() {
        ToStringHelper helper = toStringHelper(this);
        helper.add("assign", assign);
        helper.add("clear", clear);
        helper.add("patchPrior", patchPrior);
        helper.add("ensure", ensure);
        helper.add("patch", patch);
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
    
        MyStructPatch other = (MyStructPatch)o;
    
        return
            Objects.equals(assign, other.assign) &&
            Objects.equals(clear, other.clear) &&
            Objects.equals(patchPrior, other.patchPrior) &&
            Objects.equals(ensure, other.ensure) &&
            Objects.equals(patch, other.patch) &&
            true;
    }
    
    @java.lang.Override
    public int hashCode() {
        return Arrays.deepHashCode(new java.lang.Object[] {
            assign,
            clear,
            patchPrior,
            ensure,
            patch
        });
    }
    
    
    public static com.facebook.thrift.payload.Reader<MyStructPatch> asReader() {
      return MyStructPatch::read0;
    }
    
    public static MyStructPatch read0(TProtocol oprot) throws TException {
      TField __field;
      oprot.readStructBegin(MyStructPatch.NAMES_TO_IDS, MyStructPatch.THRIFT_NAMES_TO_IDS, MyStructPatch.FIELD_METADATA);
      MyStructPatch.Builder builder = new MyStructPatch.Builder();
      while (true) {
        __field = oprot.readFieldBegin();
        if (__field.type == TType.STOP) { break; }
        switch (__field.id) {
        case _ASSIGN:
          if (__field.type == TType.STRUCT) {
            test.fixtures.patch.MyStruct assign = test.fixtures.patch.MyStruct.read0(oprot);
            builder.setAssign(assign);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _CLEAR:
          if (__field.type == TType.BOOL) {
            boolean clear = oprot.readBool();
            builder.setClear(clear);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _PATCHPRIOR:
          if (__field.type == TType.STRUCT) {
            test.fixtures.patch.MyStructFieldPatch patchPrior = test.fixtures.patch.MyStructFieldPatch.read0(oprot);
            builder.setPatchPrior(patchPrior);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _ENSURE:
          if (__field.type == TType.STRUCT) {
            test.fixtures.patch.MyStructEnsureStruct ensure = test.fixtures.patch.MyStructEnsureStruct.read0(oprot);
            builder.setEnsure(ensure);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _PATCH:
          if (__field.type == TType.STRUCT) {
            test.fixtures.patch.MyStructFieldPatch patch = test.fixtures.patch.MyStructFieldPatch.read0(oprot);
            builder.setPatch(patch);
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
      int structStart = 0;
      int pos = 0;
      com.facebook.thrift.protocol.ByteBufTProtocol p = (com.facebook.thrift.protocol.ByteBufTProtocol) oprot;
      if (assign != null) {
        oprot.writeFieldBegin(ASSIGN_FIELD_DESC);
        this.assign.write0(oprot);
        oprot.writeFieldEnd();
      }
      if (!com.facebook.thrift.util.IntrinsicDefaults.isDefault(clear)) {
        oprot.writeFieldBegin(CLEAR_FIELD_DESC);
        oprot.writeBool(this.clear);
        oprot.writeFieldEnd();
      };
      java.util.Objects.requireNonNull(patchPrior, "patchPrior must not be null");
      structStart = p.mark();
        oprot.writeFieldBegin(PATCH_PRIOR_FIELD_DESC);
        pos = p.mark();
        this.patchPrior.write0(oprot);
        if (p.mark() - pos > p.getEmptyStructSize()) {
          p.writeFieldEnd();    
        } else {
          p.rollback(structStart);
        }    
      java.util.Objects.requireNonNull(ensure, "ensure must not be null");
      structStart = p.mark();
        oprot.writeFieldBegin(ENSURE_FIELD_DESC);
        pos = p.mark();
        this.ensure.write0(oprot);
        if (p.mark() - pos > p.getEmptyStructSize()) {
          p.writeFieldEnd();    
        } else {
          p.rollback(structStart);
        }    
      java.util.Objects.requireNonNull(patch, "patch must not be null");
      structStart = p.mark();
        oprot.writeFieldBegin(PATCH_FIELD_DESC);
        pos = p.mark();
        this.patch.write0(oprot);
        if (p.mark() - pos > p.getEmptyStructSize()) {
          p.writeFieldEnd();    
        } else {
          p.rollback(structStart);
        }    
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }
    
    private static class _MyStructPatchLazy {
        private static final MyStructPatch _DEFAULT = new MyStructPatch.Builder().build();
    }
    
    public static MyStructPatch defaultInstance() {
        return  _MyStructPatchLazy._DEFAULT;
    }
}
