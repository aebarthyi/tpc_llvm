# TPC fork tooling

Downstream-only tooling for maintaining this TPC fork of LLVM. Nothing in this
directory exists upstream, so upstream merges never touch it, and it is not part
of the LLVM CMake build.

## Differential tests (`difftest/`)

`difftest/run.py` checks the fork compiler for functional correctness against
Habana's closed-source `tpc-clang`. It compiles every kernel in
`difftest/kernels/` with both compilers, runs the objects on the TPC simulator
with identical, fixed-seed inputs, and compares the output tensors bit for bit.

### Setup

It needs an extracted `habanalabs-tools` package (closed compiler + simulator),
located through `TPC_HOME`:

```sh
source ~/tpc-1.24.2/env.sh        # sets TPC_HOME, PATH, LD_LIBRARY_PATH
```

The habanatools headers and libraries are Intel-confidential; they are used in
place and must never be copied into this repository. `run.py` builds its small
simulator driver (`sim_runner.cpp`) against them on first use.

### Running

```sh
# Candidate = build/bin/tpc-clang by default.
tpc/difftest/run.py

# During the upstream port: compare a new build against the last good one, so
# pre-existing differences from Habana's compiler are not reported as failures.
tpc/difftest/run.py --candidate build-port/bin/tpc-clang --baseline build/bin/tpc-clang

# A/B the same compiler with different flags.
tpc/difftest/run.py --candidate-flags=-fno-unroll-loops

# Only some kernels or architectures.
tpc/difftest/run.py --arch gaudi2 tpc/difftest/kernels/add_f32.c

# Another optimization level (the default is -O2). -O0 and -O1 take different
# paths through the pipeline, so check them when changing it.
tpc/difftest/run.py --cflags=-O0

# Compile parity: does every RC99 lit-test source still compile (or still fail
# to compile) the same way Habana's compiler and the baseline do?
tpc/difftest/run.py --compile-parity --baseline build/bin/tpc-clang
```

Results, one line per kernel and architecture:

| Verdict | Meaning |
|---------|---------|
| `PASS`  | Candidate output equals the reference output. |
| `KNOWN` | Candidate differs from the reference exactly like the baseline does (a pre-existing difference). |
| `FAIL`  | Anything else, including a change in the candidate's `.tpc_metadata` relative to a baseline with the same metadata version. |
| `SKIP`  | The reference compiler cannot build the kernel for that architecture. |

`run.py` exits non-zero on any `FAIL`. Work files (objects, inputs, outputs) are
kept under `build/tpc-difftest/<kernel>/<arch>/` for debugging.

### Writing a kernel

A kernel is a TPC-C file whose tensors and launch geometry are declared in
`// DIFFTEST:` comments, much like lit `RUN:` lines:

```c
// DIFFTEST: arch gaudi gaudi2             simulated archs: gaudi, gaudi2, gaudi3
// DIFFTEST: in  f32 256 6 3 range=-4,4    input tensor: dtype, dims (dim 0 first), value range
// DIFFTEST: out f32 256 6 3               output tensor
// DIFFTEST: index-space 4 6 3             index-space geometry (missing dims = 1)
// DIFFTEST: scalar f32 0.5                scalar kernel parameter (f32, i32, u32), in order
// DIFFTEST: tolerance ulp 2               optional: allow small float differences
// DIFFTEST: cflags -O1                    optional: extra flags for every compiler
```

As with the graph compiler, inputs come before outputs: list all `in` lines
before any `out` line, in the same order as the kernel's `tensor` parameters.
Data types: `f32 bf16 f16 i32 u32 i16 u16 i8 u8 f8_152 f8_143`. Unwritten
output elements keep the fill byte `0xCD`.

Keep each kernel focused on one feature, so a failure points at a cause.

Habana's compiler is the reference for all behaviour, including where it differs
from C. For example, with a non-constant divisor both compilers take the absolute
value with `abs.i32`, which saturates, so `INT_MIN % 3` gives -1 rather than the
-2 that C expects. `abs_i32`, `sdiv_vec_edge_i32` and `srem_vec_edge_i32` cover
such inputs.

### Known issues

* **Old baselines.** Fork compilers older than the version 20 `.tpc_metadata`
  change write version 12, which current habanatools cannot run: the missing
  `irf32Mode` makes the simulator assume irf44 mode. `run.py` runs such a baseline
  with the reference object's metadata section, and does not compare its metadata
  with the candidate's.
* **Metadata the fork does not compute.** `targetSpecific`, `regIDUsed`,
  `duplicateTensors`, `exclusiveSwitch` and `unsetSmallVLM` stay 0, and
  `hashedISA` does not match Habana's values. The simulator accepts this, but
  nothing has checked it against the graph compiler or hardware.
* **Unsigned vector `/` and `%` with an operand of 2^31 or more.** Both compilers
  take the absolute value of the operands with `abs.i32`, as they do for signed
  values, so these operands act as negative numbers and the results differ from C.
  Scalar unsigned `/` and `%` are correct. `udiv_vec_u32` and `urem_vec_u32` record
  Habana's behaviour.
* **`scalar_math_i32` at `-O0` on gaudi2.** The fork miscompiles it: every output
  element is wrong against a host computation, while Habana's compiler is
  correct. Reproduce with `--cflags=-O0`; expected `KNOWN` until fixed.
* **`doron1`.** The fork asserts (`hasDoron1()`) for this arch, which accounts
  for nearly all failures in the fork's own lit suite.
