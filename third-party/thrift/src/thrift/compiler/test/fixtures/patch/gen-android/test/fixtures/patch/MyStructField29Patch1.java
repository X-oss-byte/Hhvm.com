/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package test.fixtures.patch;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;
import java.util.Collections;
import java.util.BitSet;
import java.util.Arrays;
import com.facebook.thrift.*;
import com.facebook.thrift.annotations.*;
import com.facebook.thrift.async.*;
import com.facebook.thrift.meta_data.*;
import com.facebook.thrift.server.*;
import com.facebook.thrift.transport.*;
import com.facebook.thrift.protocol.*;

@SuppressWarnings({ "unused", "serial" })
public class MyStructField29Patch1 implements TBase, java.io.Serializable, Cloneable {
  private static final TStruct STRUCT_DESC = new TStruct("MyStructField29Patch1");
  private static final TField ASSIGN_FIELD_DESC = new TField("assign", TType.MAP, (short)1);
  private static final TField CLEAR_FIELD_DESC = new TField("clear", TType.BOOL, (short)2);
  private static final TField PATCH_PRIOR_FIELD_DESC = new TField("patchPrior", TType.MAP, (short)3);
  private static final TField ADD_FIELD_DESC = new TField("add", TType.MAP, (short)5);
  private static final TField PATCH_FIELD_DESC = new TField("patch", TType.MAP, (short)6);
  private static final TField REMOVE_FIELD_DESC = new TField("remove", TType.SET, (short)7);
  private static final TField PUT_FIELD_DESC = new TField("put", TType.MAP, (short)9);

  /**
   * Assigns to a (set) value.
   * 
   * If set, all other operations are ignored.
   * 
   * Note: Optional and union fields must be set before assigned.
   * 
   */
  public final Map<String,Integer> assign;
  /**
   * Clears a value. Applies first.
   */
  public final Boolean clear;
  /**
   * Patches any previously set values. Applies second.
   */
  public final Map<String,I32Patch> patchPrior;
  /**
   * Add the given values, if the keys are not already present. Applies forth.
   */
  public final Map<String,Integer> add;
  /**
   * Patches any set value, including newly set values. Applies last.
   */
  public final Map<String,I32Patch> patch;
  /**
   * Removes entries, if present. Applies third.
   */
  public final Set<String> remove;
  /**
   * Adds or replaces the given key/value pairs. Applies fifth.
   */
  public final Map<String,Integer> put;
  public static final int ASSIGN = 1;
  public static final int CLEAR = 2;
  public static final int PATCHPRIOR = 3;
  public static final int ADD = 5;
  public static final int PATCH = 6;
  public static final int REMOVE = 7;
  public static final int PUT = 9;

  public MyStructField29Patch1(
      Map<String,Integer> assign,
      Boolean clear,
      Map<String,I32Patch> patchPrior,
      Map<String,Integer> add,
      Map<String,I32Patch> patch,
      Set<String> remove,
      Map<String,Integer> put) {
    this.assign = assign;
    this.clear = clear;
    this.patchPrior = patchPrior;
    this.add = add;
    this.patch = patch;
    this.remove = remove;
    this.put = put;
  }

  /**
   * Performs a deep copy on <i>other</i>.
   */
  public MyStructField29Patch1(MyStructField29Patch1 other) {
    if (other.isSetAssign()) {
      this.assign = TBaseHelper.deepCopy(other.assign);
    } else {
      this.assign = null;
    }
    if (other.isSetClear()) {
      this.clear = TBaseHelper.deepCopy(other.clear);
    } else {
      this.clear = null;
    }
    if (other.isSetPatchPrior()) {
      this.patchPrior = TBaseHelper.deepCopy(other.patchPrior);
    } else {
      this.patchPrior = null;
    }
    if (other.isSetAdd()) {
      this.add = TBaseHelper.deepCopy(other.add);
    } else {
      this.add = null;
    }
    if (other.isSetPatch()) {
      this.patch = TBaseHelper.deepCopy(other.patch);
    } else {
      this.patch = null;
    }
    if (other.isSetRemove()) {
      this.remove = TBaseHelper.deepCopy(other.remove);
    } else {
      this.remove = null;
    }
    if (other.isSetPut()) {
      this.put = TBaseHelper.deepCopy(other.put);
    } else {
      this.put = null;
    }
  }

  public MyStructField29Patch1 deepCopy() {
    return new MyStructField29Patch1(this);
  }

  /**
   * Assigns to a (set) value.
   * 
   * If set, all other operations are ignored.
   * 
   * Note: Optional and union fields must be set before assigned.
   * 
   */
  public Map<String,Integer> getAssign() {
    return this.assign;
  }

  // Returns true if field assign is set (has been assigned a value) and false otherwise
  public boolean isSetAssign() {
    return this.assign != null;
  }

  /**
   * Clears a value. Applies first.
   */
  public Boolean isClear() {
    return this.clear;
  }

  // Returns true if field clear is set (has been assigned a value) and false otherwise
  public boolean isSetClear() {
    return this.clear != null;
  }

  /**
   * Patches any previously set values. Applies second.
   */
  public Map<String,I32Patch> getPatchPrior() {
    return this.patchPrior;
  }

  // Returns true if field patchPrior is set (has been assigned a value) and false otherwise
  public boolean isSetPatchPrior() {
    return this.patchPrior != null;
  }

  /**
   * Add the given values, if the keys are not already present. Applies forth.
   */
  public Map<String,Integer> getAdd() {
    return this.add;
  }

  // Returns true if field add is set (has been assigned a value) and false otherwise
  public boolean isSetAdd() {
    return this.add != null;
  }

  /**
   * Patches any set value, including newly set values. Applies last.
   */
  public Map<String,I32Patch> getPatch() {
    return this.patch;
  }

  // Returns true if field patch is set (has been assigned a value) and false otherwise
  public boolean isSetPatch() {
    return this.patch != null;
  }

  /**
   * Removes entries, if present. Applies third.
   */
  public Set<String> getRemove() {
    return this.remove;
  }

  // Returns true if field remove is set (has been assigned a value) and false otherwise
  public boolean isSetRemove() {
    return this.remove != null;
  }

  /**
   * Adds or replaces the given key/value pairs. Applies fifth.
   */
  public Map<String,Integer> getPut() {
    return this.put;
  }

  // Returns true if field put is set (has been assigned a value) and false otherwise
  public boolean isSetPut() {
    return this.put != null;
  }

  @Override
  public boolean equals(Object _that) {
    if (_that == null)
      return false;
    if (this == _that)
      return true;
    if (!(_that instanceof MyStructField29Patch1))
      return false;
    MyStructField29Patch1 that = (MyStructField29Patch1)_that;

    if (!TBaseHelper.equalsNobinary(this.isSetAssign(), that.isSetAssign(), this.assign, that.assign)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetClear(), that.isSetClear(), this.clear, that.clear)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetPatchPrior(), that.isSetPatchPrior(), this.patchPrior, that.patchPrior)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetAdd(), that.isSetAdd(), this.add, that.add)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetPatch(), that.isSetPatch(), this.patch, that.patch)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetRemove(), that.isSetRemove(), this.remove, that.remove)) { return false; }

    if (!TBaseHelper.equalsNobinary(this.isSetPut(), that.isSetPut(), this.put, that.put)) { return false; }

    return true;
  }

  @Override
  public int hashCode() {
    return Arrays.deepHashCode(new Object[] {assign, clear, patchPrior, add, patch, remove, put});
  }

  // This is required to satisfy the TBase interface, but can't be implemented on immutable struture.
  public void read(TProtocol iprot) throws TException {
    throw new TException("unimplemented in android immutable structure");
  }

  public static MyStructField29Patch1 deserialize(TProtocol iprot) throws TException {
    Map<String,Integer> tmp_assign = null;
    Boolean tmp_clear = null;
    Map<String,I32Patch> tmp_patchPrior = null;
    Map<String,Integer> tmp_add = null;
    Map<String,I32Patch> tmp_patch = null;
    Set<String> tmp_remove = null;
    Map<String,Integer> tmp_put = null;
    TField __field;
    iprot.readStructBegin();
    while (true)
    {
      __field = iprot.readFieldBegin();
      if (__field.type == TType.STOP) {
        break;
      }
      switch (__field.id)
      {
        case ASSIGN:
          if (__field.type == TType.MAP) {
            {
              TMap _map140 = iprot.readMapBegin();
              tmp_assign = new HashMap<String,Integer>(Math.max(0, 2*_map140.size));
              for (int _i141 = 0; 
                   (_map140.size < 0) ? iprot.peekMap() : (_i141 < _map140.size); 
                   ++_i141)
              {
                String _key142;
                Integer _val143;
                _key142 = iprot.readString();
                _val143 = iprot.readI32();
                tmp_assign.put(_key142, _val143);
              }
              iprot.readMapEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case CLEAR:
          if (__field.type == TType.BOOL) {
            tmp_clear = iprot.readBool();
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case PATCHPRIOR:
          if (__field.type == TType.MAP) {
            {
              TMap _map144 = iprot.readMapBegin();
              tmp_patchPrior = new HashMap<String,I32Patch>(Math.max(0, 2*_map144.size));
              for (int _i145 = 0; 
                   (_map144.size < 0) ? iprot.peekMap() : (_i145 < _map144.size); 
                   ++_i145)
              {
                String _key146;
                I32Patch _val147;
                _key146 = iprot.readString();
                _val147 = I32Patch.deserialize(iprot);
                tmp_patchPrior.put(_key146, _val147);
              }
              iprot.readMapEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case ADD:
          if (__field.type == TType.MAP) {
            {
              TMap _map148 = iprot.readMapBegin();
              tmp_add = new HashMap<String,Integer>(Math.max(0, 2*_map148.size));
              for (int _i149 = 0; 
                   (_map148.size < 0) ? iprot.peekMap() : (_i149 < _map148.size); 
                   ++_i149)
              {
                String _key150;
                Integer _val151;
                _key150 = iprot.readString();
                _val151 = iprot.readI32();
                tmp_add.put(_key150, _val151);
              }
              iprot.readMapEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case PATCH:
          if (__field.type == TType.MAP) {
            {
              TMap _map152 = iprot.readMapBegin();
              tmp_patch = new HashMap<String,I32Patch>(Math.max(0, 2*_map152.size));
              for (int _i153 = 0; 
                   (_map152.size < 0) ? iprot.peekMap() : (_i153 < _map152.size); 
                   ++_i153)
              {
                String _key154;
                I32Patch _val155;
                _key154 = iprot.readString();
                _val155 = I32Patch.deserialize(iprot);
                tmp_patch.put(_key154, _val155);
              }
              iprot.readMapEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case REMOVE:
          if (__field.type == TType.SET) {
            {
              TSet _set156 = iprot.readSetBegin();
              tmp_remove = new HashSet<String>(Math.max(0, 2*_set156.size));
              for (int _i157 = 0; 
                   (_set156.size < 0) ? iprot.peekSet() : (_i157 < _set156.size); 
                   ++_i157)
              {
                String _elem158;
                _elem158 = iprot.readString();
                tmp_remove.add(_elem158);
              }
              iprot.readSetEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        case PUT:
          if (__field.type == TType.MAP) {
            {
              TMap _map159 = iprot.readMapBegin();
              tmp_put = new HashMap<String,Integer>(Math.max(0, 2*_map159.size));
              for (int _i160 = 0; 
                   (_map159.size < 0) ? iprot.peekMap() : (_i160 < _map159.size); 
                   ++_i160)
              {
                String _key161;
                Integer _val162;
                _key161 = iprot.readString();
                _val162 = iprot.readI32();
                tmp_put.put(_key161, _val162);
              }
              iprot.readMapEnd();
            }
          } else {
            TProtocolUtil.skip(iprot, __field.type);
          }
          break;
        default:
          TProtocolUtil.skip(iprot, __field.type);
          break;
      }
      iprot.readFieldEnd();
    }
    iprot.readStructEnd();

    MyStructField29Patch1 _that;
    _that = new MyStructField29Patch1(
      tmp_assign
      ,tmp_clear
      ,tmp_patchPrior
      ,tmp_add
      ,tmp_patch
      ,tmp_remove
      ,tmp_put
    );
    _that.validate();
    return _that;
  }

  public void write(TProtocol oprot) throws TException {
    validate();

    oprot.writeStructBegin(STRUCT_DESC);
    if (this.assign != null) {
      if (isSetAssign()) {
        oprot.writeFieldBegin(ASSIGN_FIELD_DESC);
        {
          oprot.writeMapBegin(new TMap(TType.STRING, TType.I32, this.assign.size()));
          for (Map.Entry<String, Integer> _iter163 : this.assign.entrySet())          {
            oprot.writeString(_iter163.getKey());
            oprot.writeI32(_iter163.getValue());
          }
          oprot.writeMapEnd();
        }
        oprot.writeFieldEnd();
      }
    }
    if (this.clear != null) {
      oprot.writeFieldBegin(CLEAR_FIELD_DESC);
      oprot.writeBool(this.clear);
      oprot.writeFieldEnd();
    }
    if (this.patchPrior != null) {
      oprot.writeFieldBegin(PATCH_PRIOR_FIELD_DESC);
      {
        oprot.writeMapBegin(new TMap(TType.STRING, TType.STRUCT, this.patchPrior.size()));
        for (Map.Entry<String, I32Patch> _iter164 : this.patchPrior.entrySet())        {
          oprot.writeString(_iter164.getKey());
          _iter164.getValue().write(oprot);
        }
        oprot.writeMapEnd();
      }
      oprot.writeFieldEnd();
    }
    if (this.add != null) {
      oprot.writeFieldBegin(ADD_FIELD_DESC);
      {
        oprot.writeMapBegin(new TMap(TType.STRING, TType.I32, this.add.size()));
        for (Map.Entry<String, Integer> _iter165 : this.add.entrySet())        {
          oprot.writeString(_iter165.getKey());
          oprot.writeI32(_iter165.getValue());
        }
        oprot.writeMapEnd();
      }
      oprot.writeFieldEnd();
    }
    if (this.patch != null) {
      oprot.writeFieldBegin(PATCH_FIELD_DESC);
      {
        oprot.writeMapBegin(new TMap(TType.STRING, TType.STRUCT, this.patch.size()));
        for (Map.Entry<String, I32Patch> _iter166 : this.patch.entrySet())        {
          oprot.writeString(_iter166.getKey());
          _iter166.getValue().write(oprot);
        }
        oprot.writeMapEnd();
      }
      oprot.writeFieldEnd();
    }
    if (this.remove != null) {
      oprot.writeFieldBegin(REMOVE_FIELD_DESC);
      {
        oprot.writeSetBegin(new TSet(TType.STRING, this.remove.size()));
        for (String _iter167 : this.remove)        {
          oprot.writeString(_iter167);
        }
        oprot.writeSetEnd();
      }
      oprot.writeFieldEnd();
    }
    if (this.put != null) {
      oprot.writeFieldBegin(PUT_FIELD_DESC);
      {
        oprot.writeMapBegin(new TMap(TType.STRING, TType.I32, this.put.size()));
        for (Map.Entry<String, Integer> _iter168 : this.put.entrySet())        {
          oprot.writeString(_iter168.getKey());
          oprot.writeI32(_iter168.getValue());
        }
        oprot.writeMapEnd();
      }
      oprot.writeFieldEnd();
    }
    oprot.writeFieldStop();
    oprot.writeStructEnd();
  }

  @Override
  public String toString() {
    return toString(1, true);
  }

  @Override
  public String toString(int indent, boolean prettyPrint) {
    return TBaseHelper.toStringHelper(this, indent, prettyPrint);
  }

  public void validate() throws TException {
    // check for required fields
  }

}

