//===- ValueTypes.td - ValueType definitions ---------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Value types - These values correspond to the register types defined in the
// MachineValueTypes.h file.  If you update anything here, you must update it
// there as well!
//
//===----------------------------------------------------------------------===//

class ValueType<int size, int value> {
  string Namespace = "MVT";
  int Size = size;
  int Value = value;
}

def OtherVT : ValueType<0,   1>;  // "Other" value
def i1      : ValueType<1,   2>;  // One bit boolean value
def i2      : ValueType<2,   3>;  // 2-bit integer value
def i4      : ValueType<4,   4>;  // 4-bit integer value
def i8      : ValueType<8,   5>;  // 8-bit integer value
def i16     : ValueType<16,  6>;  // 16-bit integer value
def i32     : ValueType<32,  7>;  // 32-bit integer value
def i64     : ValueType<64,  8>;  // 64-bit integer value
def i128    : ValueType<128, 9>;  // 128-bit integer value

def f8_143  : ValueType<16,  10>;  // 8-bit floating point value with 3-bit mantissa (TPC)
def f8_152  : ValueType<16,  11>;  // 8-bit floating point value with 2-bit mantissa (TPC)
def bf16    : ValueType<16,  12>;  // 16-bit brain floating point value
def f16     : ValueType<16,  13>;  // 16-bit floating point value
def f32     : ValueType<32,  14>;  // 32-bit floating point value
def f64     : ValueType<64,  15>;  // 64-bit floating point value
def f80     : ValueType<80,  16>;  // 80-bit floating point value
def f128    : ValueType<128, 17>;  // 128-bit floating point value
def ppcf128 : ValueType<128, 18>;  // PPC 128-bit floating point value

def v1i1    : ValueType<1,    19>;  //    1 x i1 vector value
def v2i1    : ValueType<2,    20>;  //    2 x i1 vector value
def v4i1    : ValueType<4,    21>;  //    4 x i1 vector value
def v8i1    : ValueType<8,    22>;  //    8 x i1 vector value
def v16i1   : ValueType<16,   23>;  //   16 x i1 vector value
def v32i1   : ValueType<32,   24>;  //   32 x i1 vector value
def v64i1   : ValueType<64,   25>;  //   64 x i1 vector value
def v128i1  : ValueType<128,  26>;  //  128 x i1 vector value
def v256i1  : ValueType<256,  27>;  //  256 x i1 vector value
def v512i1  : ValueType<512,  28>;  //  512 x i1 vector value
def v1024i1 : ValueType<1024, 29>;  // 1024 x i1 vector value
def v2048i1 : ValueType<2048, 30>;  // 2048 x i1 vector value

def v128i2  : ValueType<256,  31>;   //  128 x i2 vector value
def v256i2  : ValueType<512,  32>;   //  256 x i2 vector value

def v64i4   : ValueType<256,  33>;   //   64 x i4 vector value
def v128i4  : ValueType<512,  34>;   //  128 x i4 vector value

def v1i8    : ValueType<8,    35>;  //    1 x i8 vector value
def v2i8    : ValueType<16,   36>;  //    2 x i8 vector value
def v4i8    : ValueType<32,   37>;  //    4 x i8 vector value
def v8i8    : ValueType<64,   38>;  //    8 x i8 vector value
def v16i8   : ValueType<128,  39>;  //   16 x i8 vector value
def v32i8   : ValueType<256,  40>;  //   32 x i8 vector value
def v64i8   : ValueType<512,  41>;  //   64 x i8 vector value
def v128i8  : ValueType<1024, 42>;  //  128 x i8 vector value
def v256i8  : ValueType<2048, 43>;  //  256 x i8 vector value
def v512i8  : ValueType<4096, 44>;  //  512 x i8 vector value
def v1024i8 : ValueType<8192, 45>;  // 1024 x i8 vector value

def v1i16   : ValueType<16,   46>;  //   1 x i16 vector value
def v2i16   : ValueType<32,   47>;  //   2 x i16 vector value
def v3i16   : ValueType<48,   48>;  //   3 x i16 vector value
def v4i16   : ValueType<64,   49>;  //   4 x i16 vector value
def v8i16   : ValueType<128,  50>;  //   8 x i16 vector value
def v16i16  : ValueType<256,  51>;  //  16 x i16 vector value
def v32i16  : ValueType<512,  52>;  //  32 x i16 vector value
def v64i16  : ValueType<1024, 53>;  //  64 x i16 vector value
def v128i16 : ValueType<2048, 54>;  // 128 x i16 vector value
def v256i16 : ValueType<4096, 55>;  // 256 x i16 vector value
def v512i16 : ValueType<8192, 56>;  // 512 x i16 vector value

def v1i32    : ValueType<32,    57>;  //    1 x i32 vector value
def v2i32    : ValueType<64,    58>;  //    2 x i32 vector value
def v3i32    : ValueType<96,    59>;  //    3 x i32 vector value
def v4i32    : ValueType<128,   60>;  //    4 x i32 vector value
def v5i32    : ValueType<160,   61>;  //    5 x i32 vector value
def v6i32    : ValueType<192,   62>;  //    6 x f32 vector value
def v7i32    : ValueType<224,   63>;  //    7 x f32 vector value
def v8i32    : ValueType<256,   64>;  //    8 x i32 vector value
def v9i32    : ValueType<288,   65>;  //    9 x i32 vector value
def v10i32   : ValueType<320,   66>;  //   10 x i32 vector value
def v11i32   : ValueType<352,   67>;  //   11 x i32 vector value
def v12i32   : ValueType<384,   68>;  //   12 x i32 vector value
def v16i32   : ValueType<512,   69>;  //   16 x i32 vector value
def v32i32   : ValueType<1024,  70>;  //   32 x i32 vector value
def v64i32   : ValueType<2048,  71>;  //   64 x i32 vector value
def v128i32  : ValueType<4096,  72>;  //  128 x i32 vector value
def v256i32  : ValueType<8192,  73>;  //  256 x i32 vector value
def v512i32  : ValueType<16384, 74>;  //  512 x i32 vector value
def v1024i32 : ValueType<32768, 75>;  // 1024 x i32 vector value
def v2048i32 : ValueType<65536, 76>;  // 2048 x i32 vector value

def v1i64   : ValueType<64,    77>;  //   1 x i64 vector value
def v2i64   : ValueType<128,   78>;  //   2 x i64 vector value
def v3i64   : ValueType<192,   79>;  //   3 x i64 vector value
def v4i64   : ValueType<256,   80>;  //   4 x i64 vector value
def v8i64   : ValueType<512,   81>;  //   8 x i64 vector value
def v16i64  : ValueType<1024,  82>;  //  16 x i64 vector value
def v32i64  : ValueType<2048,  83>;  //  32 x i64 vector value
def v64i64  : ValueType<4096,  84>;  //  64 x i64 vector value
def v128i64 : ValueType<8192,  85>;  // 128 x i64 vector value
def v256i64 : ValueType<16384, 86>;  // 256 x i64 vector value

def v1i128 : ValueType<128, 87>;  //  1 x i128 vector value

def v256f8_143  : ValueType<2048, 88>;  //  256 x f8_143 vector value (TPC)
def v512f8_143  : ValueType<4096, 89>;  //  512 x f8_143 vector value (TPC)
def v1024f8_143 : ValueType<8192, 90>;  // 1024 x f8_143 vector value (TPC)
def v256f8_152  : ValueType<2048, 91>;  //  256 x f8_152 vector value (TPC)
def v512f8_152  : ValueType<4096, 92>;  //  512 x f8_152 vector value (TPC)
def v1024f8_152 : ValueType<8192, 93>;  // 1024 x f8_152 vector value (TPC)

def v1f16    : ValueType<16,     94>;  //    1 x f16 vector value
def v2f16    : ValueType<32,     95>;  //    2 x f16 vector value
def v3f16    : ValueType<48,     96>;  //    3 x f16 vector value
def v4f16    : ValueType<64,     97>;  //    4 x f16 vector value
def v8f16    : ValueType<128,    98>;  //    8 x f16 vector value
def v16f16   : ValueType<256,    99>;  //   16 x f16 vector value
def v32f16   : ValueType<512,   100>;  //   32 x f16 vector value
def v64f16   : ValueType<1024,  101>;  //   64 x f16 vector value
def v128f16  : ValueType<2048,  102>;  //  128 x f16 vector value
def v256f16  : ValueType<4096,  103>;  //  256 x f16 vector value
def v512f16  : ValueType<8192,  104>;  //  512 x f16 vector value

def v2bf16   : ValueType<32,    105>;  //    2 x bf16 vector value
def v3bf16   : ValueType<48,    106>;  //    3 x bf16 vector value
def v4bf16   : ValueType<64,    107>;  //    4 x bf16 vector value
def v8bf16   : ValueType<128,   108>;  //    8 x bf16 vector value
def v16bf16  : ValueType<256,   109>;  //   16 x bf16 vector value
def v32bf16  : ValueType<512,   110>;  //   32 x bf16 vector value
def v64bf16  : ValueType<1024,  111>;  //   64 x bf16 vector value
def v128bf16 : ValueType<2048,  112>;  //  128 x bf16 vector value
def v256bf16 : ValueType<4096,   113>;  //  256 x bf16 vector value (TPC)

def v1f32    : ValueType<32,    114>;  //    1 x f32 vector value
def v2f32    : ValueType<64,    115>;  //    2 x f32 vector value
def v3f32    : ValueType<96,    116>;  //    3 x f32 vector value
def v4f32    : ValueType<128,   117>;  //    4 x f32 vector value
def v5f32    : ValueType<160,   118>;  //    5 x f32 vector value
def v6f32    : ValueType<192,   119>;  //    6 x f32 vector value
def v7f32    : ValueType<224,   120>;  //    7 x f32 vector value
def v8f32    : ValueType<256,   121>;  //    8 x f32 vector value
def v9f32    : ValueType<288,   122>;  //    9 x f32 vector value
def v10f32   : ValueType<320,   123>;  //   10 x f32 vector value
def v11f32   : ValueType<352,   124>;  //   11 x f32 vector value
def v12f32   : ValueType<384,   125>;  //   12 x f32 vector value
def v16f32   : ValueType<512,   126>;  //   16 x f32 vector value
def v32f32   : ValueType<1024,  127>;  //   32 x f32 vector value
def v64f32   : ValueType<2048,  128>;  //   64 x f32 vector value
def v128f32  : ValueType<4096,  129>;  //  128 x f32 vector value
def v256f32  : ValueType<8192,  130>;  //  256 x f32 vector value
def v512f32  : ValueType<16384, 131>;  //  512 x f32 vector value
def v1024f32 : ValueType<32768, 132>;  // 1024 x f32 vector value
def v2048f32 : ValueType<65536, 133>;  // 2048 x f32 vector value

def v1f64    : ValueType<64,    134>;  //    1 x f64 vector value
def v2f64    : ValueType<128,   135>;  //    2 x f64 vector value
def v3f64    : ValueType<192,   136>;  //    3 x f64 vector value
def v4f64    : ValueType<256,   137>;  //    4 x f64 vector value
def v8f64    : ValueType<512,   138>;  //    8 x f64 vector value
def v16f64   : ValueType<1024,  139>;  //   16 x f64 vector value
def v32f64   : ValueType<2048,  140>;  //   32 x f64 vector value
def v64f64   : ValueType<4096,  141>;  //   64 x f64 vector value
def v128f64  : ValueType<8192,  142>;  //  128 x f64 vector value
def v256f64  : ValueType<16384, 143>;  //  256 x f64 vector value

def nxv1i1  : ValueType<1,  144>;  // n x  1 x i1  vector value
def nxv2i1  : ValueType<2,  145>;  // n x  2 x i1  vector value
def nxv4i1  : ValueType<4,  146>;  // n x  4 x i1  vector value
def nxv8i1  : ValueType<8,  147>;  // n x  8 x i1  vector value
def nxv16i1 : ValueType<16, 148>;  // n x 16 x i1  vector value
def nxv32i1 : ValueType<32, 149>;  // n x 32 x i1  vector value
def nxv64i1 : ValueType<64, 150>;  // n x 64 x i1  vector value

def nxv1i8  : ValueType<8,   151>;  // n x  1 x i8  vector value
def nxv2i8  : ValueType<16,  152>;  // n x  2 x i8  vector value
def nxv4i8  : ValueType<32,  153>;  // n x  4 x i8  vector value
def nxv8i8  : ValueType<64,  154>;  // n x  8 x i8  vector value
def nxv16i8 : ValueType<128, 155>;  // n x 16 x i8  vector value
def nxv32i8 : ValueType<256, 156>;  // n x 32 x i8  vector value
def nxv64i8 : ValueType<512, 157>;  // n x 64 x i8  vector value

def nxv1i16  : ValueType<16,  158>;  // n x  1 x i16 vector value
def nxv2i16  : ValueType<32,  159>;  // n x  2 x i16 vector value
def nxv4i16  : ValueType<64,  160>;  // n x  4 x i16 vector value
def nxv8i16  : ValueType<128, 161>;  // n x  8 x i16 vector value
def nxv16i16 : ValueType<256, 162>;  // n x 16 x i16 vector value
def nxv32i16 : ValueType<512, 163>;  // n x 32 x i16 vector value

def nxv1i32  : ValueType<32,   164>;  // n x  1 x i32 vector value
def nxv2i32  : ValueType<64,   165>;  // n x  2 x i32 vector value
def nxv4i32  : ValueType<128,  166>;  // n x  4 x i32 vector value
def nxv8i32  : ValueType<256,  167>;  // n x  8 x i32 vector value
def nxv16i32 : ValueType<512,  168>;  // n x 16 x i32 vector value
def nxv32i32 : ValueType<1024, 169>;  // n x 32 x i32 vector value

def nxv1i64  : ValueType<64,   170>;  // n x  1 x i64 vector value
def nxv2i64  : ValueType<128,  171>;  // n x  2 x i64 vector value
def nxv4i64  : ValueType<256,  172>;  // n x  4 x i64 vector value
def nxv8i64  : ValueType<512,  173>;  // n x  8 x i64 vector value
def nxv16i64 : ValueType<1024, 174>;  // n x 16 x i64 vector value
def nxv32i64 : ValueType<2048, 175>;  // n x 32 x i64 vector value

def nxv1f16  : ValueType<16,  176>;  // n x  1 x  f16 vector value
def nxv2f16  : ValueType<32,  177>;  // n x  2 x  f16 vector value
def nxv4f16  : ValueType<64,  178>;  // n x  4 x  f16 vector value
def nxv8f16  : ValueType<128, 179>;  // n x  8 x  f16 vector value
def nxv16f16 : ValueType<256, 180>;  // n x 16 x  f16 vector value
def nxv32f16 : ValueType<512, 181>;  // n x 32 x  f16 vector value

def nxv1bf16 : ValueType<16,  182>;  // n x  1 x bf16 vector value
def nxv2bf16 : ValueType<32,  183>;  // n x  2 x bf16 vector value
def nxv4bf16 : ValueType<64,  184>;  // n x  4 x bf16 vector value
def nxv8bf16 : ValueType<128, 185>;  // n x  8 x bf16 vector value
def nxv16bf16 : ValueType<256, 186>;  // n x 16 x bf16 vector value
def nxv32bf16 : ValueType<512, 187>;  // n x 32 x bf16 vector value

def nxv1f32  : ValueType<32,  188>;  // n x  1 x  f32 vector value
def nxv2f32  : ValueType<64,  189>;  // n x  2 x  f32 vector value
def nxv4f32  : ValueType<128, 190>;  // n x  4 x  f32 vector value
def nxv8f32  : ValueType<256, 191>;  // n x  8 x  f32 vector value
def nxv16f32 : ValueType<512, 192>;  // n x 16 x  f32 vector value

def nxv1f64  : ValueType<64,  193>;  // n x  1 x  f64 vector value
def nxv2f64  : ValueType<128, 194>;  // n x  2 x  f64 vector value
def nxv4f64  : ValueType<256, 195>;  // n x  4 x  f64 vector value
def nxv8f64  : ValueType<512, 196>;  // n x  8 x  f64 vector value

def x86mmx    : ValueType<64,   197>;  // X86 MMX value
def FlagVT    : ValueType<0,    198>;  // Pre-RA sched glue
def isVoid    : ValueType<0,    199>;  // Produces no value
def untyped   : ValueType<8,    200>;  // Produces an untyped value
def funcref   : ValueType<0,    201>;  // WebAssembly's funcref type
def externref : ValueType<0,    202>;  // WebAssembly's externref type
def x86amx    : ValueType<8192, 203>;  // X86 AMX value
def i64x8     : ValueType<512,  204>;  // 8 Consecutive GPRs (AArch64)

def token      : ValueType<0, 248>;  // TokenTy
def MetadataVT : ValueType<0, 249>;  // Metadata

// Pseudo valuetype mapped to the current pointer size to any address space.
// Should only be used in TableGen.
def iPTRAny    : ValueType<0, 250>;

// Pseudo valuetype to represent "vector of any size"
def vAny       : ValueType<0, 251>;

// Pseudo valuetype to represent "float of any format"
def fAny       : ValueType<0, 252>;

// Pseudo valuetype to represent "integer of any bit width"
def iAny       : ValueType<0, 253>;

// Pseudo valuetype mapped to the current pointer size.
def iPTR       : ValueType<0, 254>;

// Pseudo valuetype to represent "any type of any size".
def Any        : ValueType<0, 255>;

/// This class is for targets that want to use pointer types in patterns
/// with the GlobalISelEmitter.  Targets must define their own pointer
/// derived from this class.  The scalar argument should be an
/// integer type with the same bit size as the pointer.
/// e.g. def p0 : PtrValueType <i64, 0>;

class PtrValueType <ValueType scalar, int addrspace> :
    ValueType<scalar.Size, scalar.Value> {
  int AddrSpace = addrspace;
}
