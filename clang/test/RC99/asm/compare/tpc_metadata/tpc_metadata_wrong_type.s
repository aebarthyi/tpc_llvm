// RUN: not %clang -cc1as -triple tpc-none-none -filetype obj %s -o /dev/null 2>&1 >/dev/null | FileCheck %s

.tpc_metadata
specialFunctionUsed:	WRONG_TYPE	3
// The tokens come from a map, so their order is not stable.
// CHECK: 4:22: error: Unexpected token during parse tpc metadata: 'WRONG_TYPE'. Expected one of

NOP;
