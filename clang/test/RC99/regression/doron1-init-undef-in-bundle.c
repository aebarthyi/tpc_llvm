// RUN: %tpc_clang -S -O2 -march=doron1 %s -o - | FileCheck %s

// On doron1 this kernel gets INIT_UNDEF instructions, and the packetizer puts them
// in a bundle. They print nothing, so the printer has to skip them.

void main(_Bool x) {
  cache_invalidate(SW_RST_LU, x, 0);
}

// CHECK: cache_invalidate rst_lu %SP{{[0-9]+}}
