# To generate what is needed for bpf setup

# Directory paths
set(BPF_OUT ${CMAKE_BINARY_DIR}/bpfs)
set(VMLINUX_OUT ${CMAKE_BINARY_DIR}/bpfs/vmlinux.h)
set(SKEL_OUT_DIR ${CMAKE_BINARY_DIR}/skeletons)

# Create output directories
file(MAKE_DIRECTORY ${BPF_OUT})
file(MAKE_DIRECTORY ${SKEL_OUT_DIR})

execute_process(
  COMMAND bpftool btf dump file /sys/kernel/btf/vmlinux format c
  OUTPUT_FILE ${VMLINUX_OUT}
  WORKING_DIRECTORY ${BPF_OUT}
  RESULT_VARIABLE rv)

