// RUN: %tpc_clang -S -O0 -march=gaudi2 %s -o - | FileCheck %s

// Copying a scalar to a squeeze counter needs a temporary, so copyPhysReg calls the
// register scavenger. Each store here reloads the counter at -O0, and a copy within
// the block is what reaches it.

void main(tensor input, int other_input, _Bool pred) {
  int5 index = {0};
  squeeze_cntr sqz = 0;
  st_tnsr_s_hwr(index, input, sqz, 0, pred, 0);
  st_tnsr_s_hwr(index, other_input, sqz, 0, pred, 0);
  st_tnsr_s_hwr_rmw(index, input, sqz, 1, 0, pred, 0);
  st_tnsr_s_hwr_rmw(index, other_input, sqz, 1, 0, pred, 0);
}

// CHECK:     mov.i32 [[ZERO:%S[0-9]+]], 0x0
// CHECK:     mov  %SQZ_CNTR0_HI, [[ZERO]]
// CHECK:     st_tnsr_s hw_reg
