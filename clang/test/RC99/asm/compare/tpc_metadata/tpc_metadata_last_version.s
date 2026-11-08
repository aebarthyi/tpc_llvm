// RUN: %clang -cc1as -triple tpc-none-none -filetype obj %s -o %t.o
// RUN: %metadata_disasm %t.o | FileCheck %s
// RUN: llvm-readelf -x .tpc_metadata %t.o | FileCheck %s --check-prefix=BYTES

.section .TPC_METADATA
version:				DD	20
specialFunctionUsed:	DB	1
printfUsed:				DB	0
scalarld:				DH	0101001011100011
LockUnLock:				DB	1
rmwStore:				DH	1010000100111110
march:					DW	1
mmioUsed:				DB	1
paramsNum:				DB	3
printfTensorID:			DB	3
numberOfThreads:		DB	4
directMMIOAccess:		DB	1
targetSpecific:			DB	1
dnorm:					DB	0
hashedISA:				DD	305419896
duplicateTensors:		DQ	72623859790382856
genAddrTensorMask:		DH	1000000000000010
regIDUsed:				DB	1
irf32Mode:				DB	0
partialStore:			DH	0100000000000001
exclusiveSwitch:		DH	0010000000000100
unsetSmallVLM:			DB	1
scalarSt:				DH	0001000000001000

// CHECK: .section .tpc_metadata
// CHECK-NEXT:	version: DD 20
// CHECK-NEXT:	specialFunctionUsed: DB 1
// CHECK-NEXT:	printfUsed: DB 0
// CHECK-NEXT:	lockUnLock: DB 1
// CHECK-NEXT:	mmioUsed: DB 1
// CHECK-NEXT:	march: DW 1
// CHECK-NEXT:	paramsNum: DB 3
// CHECK-NEXT:	printfTensorID: DB 3
// CHECK-NEXT:	numberOfThreads: DB 4
// CHECK-NEXT:	directMMIOAccess: DB 1
// CHECK-NEXT:	targetSpecific: DB 1
// CHECK-NEXT:	dnorm: DB 0
// CHECK-NEXT:	hashedISA: DD 305419896
// CHECK-NEXT:	scalarLd[1]: DB 1
// CHECK-NEXT:	scalarLd[3]: DB 1
// CHECK-NEXT:	scalarLd[6]: DB 1
// CHECK-NEXT:	scalarLd[8]: DB 1
// CHECK-NEXT:	scalarLd[9]: DB 1
// CHECK-NEXT:	scalarLd[10]: DB 1
// CHECK-NEXT:	scalarLd[14]: DB 1
// CHECK-NEXT:	scalarLd[15]: DB 1
// CHECK-NEXT:	rmwStore[0]: DB 1
// CHECK-NEXT:	rmwStore[2]: DB 1
// CHECK-NEXT:	rmwStore[7]: DB 1
// CHECK-NEXT:	rmwStore[10]: DB 1
// CHECK-NEXT:	rmwStore[11]: DB 1
// CHECK-NEXT:	rmwStore[12]: DB 1
// CHECK-NEXT:	rmwStore[13]: DB 1
// CHECK-NEXT:	rmwStore[14]: DB 1
// CHECK-NEXT:	duplicateTensors: DQ 72623859790382856
// CHECK-NEXT:	genAddrTensorMask[0]: DB 1
// CHECK-NEXT:	genAddrTensorMask[14]: DB 1
// CHECK-NEXT:	regIDUsed: DB 1
// CHECK-NEXT:	irf32Mode: DB 0
// CHECK-NEXT:	partialStore[1]: DB 1
// CHECK-NEXT:	partialStore[15]: DB 1
// CHECK-NEXT:	exclusiveSwitch[2]: DB 1
// CHECK-NEXT:	exclusiveSwitch[13]: DB 1
// CHECK-NEXT:	unsetSmallVLM: DB 1
// CHECK-NEXT:	scalarSt[3]: DB 1
// CHECK-NEXT:	scalarSt[12]: DB 1

// The offsets, which the SynapseAI tools read directly.
// BYTES:      0x00000000 14000000 01000101 01000000 03030401
// BYTES-NEXT: 0x00000010 01007856 34120000 00004ac7 857c0807
// BYTES-NEXT: 0x00000020 06050403 02010140 01000280 04200108
// BYTES-NEXT: 0x00000030 10000000 00000000 00000000 00000000

NOP;
