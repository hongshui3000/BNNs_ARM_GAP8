
OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  L2_priv0           : ORIGIN = 0x1c000004, LENGTH = 0x00007ffc
  L2_priv0_aliased   : ORIGIN = 0x00000004, LENGTH = 0x00007ffc
  L2_priv1           : ORIGIN = 0x1c008000, LENGTH = 0x00008000
  L2_shared          : ORIGIN = 0x1c010000, LENGTH = 0x00070000
  L1                 : ORIGIN = 0x10000004, LENGTH = 0x00010000
  L1_aliased         : ORIGIN = 0x00000004, LENGTH = 0x0000fffc
}

__ZERO  = 0;
__USE_UART = 0;
__RT_DEBUG_CONFIG   = (0 << 8) | 0;
__FC   = 1;
__L2   = 0x80000;
__L1Cl = 0x10000;
__FETCH_ALL = 0x0;
__ACTIVE_FC = 0x1;
__rt_stack_size = 0x800;
__NB_ACTIVE_PE = 8;
__rt_platform = 0;
__rt_iodev = 0;
__rt_iodev_default_value = 0;
__rt_iodev_uart_value = 1;
__rt_iodev_uart_channel = 0;
__rt_iodev_uart_baudrate = 625000;
__rt_nb_cluster = 1;
__rt_nb_pe = 8;
__rt_cl_master_stack_size = 0x400;
__rt_cl_slave_stack_size = 0x400;
__rt_config = 0x1;
__rt_debug_init_config = 0x3;
__rt_debug_init_config_trace = 0x0;
GROUP( -lgloss -lgcc )

SECTIONS
{
  .vectors :
  {
    . = ALIGN(4);
    __irq_vector_base = .;KEEP(*(.vectors))
  } > L2_priv1

  .l1FcTiny : AT(ORIGIN(L2_priv0))
  {
    . = ALIGN(4);
    _l1FcShared_start = .;
    *(.l1FcTiny)
    *(.l1FcTiny.*)
    *(.fcTcdmTiny)
    *(.fcTcdmTiny.*)
    . = ALIGN(4);
    _l1FcShared_end = .;
  } > L2_priv0_aliased

  .data_tiny_fc (ADDR(.l1FcTiny) + SIZEOF(.l1FcTiny)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny))
  {
    . = ALIGN(4);
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > L2_priv0_aliased

  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text)
    *(.text.*)
    _etext  =  .;
    *(.lit)
    *(.shdata)
    _endtext = .;
    __L2_priv1_end = ABSOLUTE(.);
  } > L2_priv1

  __fc_code_end = ALIGN(8);

  .init (ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc))
  {
    . = ALIGN(4);
    KEEP( *(.init) )
  } > L2_priv0

  .fini (ADDR(.init) + SIZEOF(.init)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init))
  {
    . = ALIGN(4);
    KEEP( *(.fini) )
  } > L2_priv0

  .preinit_array (ADDR(.fini) + SIZEOF(.fini)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini))
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > L2_priv0

  .init_array (ADDR(.preinit_array) + SIZEOF(.preinit_array)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array))
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    __CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    KEEP(*(.ctors.start))
    KEEP(*(.ctors))
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    LONG(0)
    __CTOR_END__ = .;
    PROVIDE_HIDDEN (__init_array_end = .);
  } > L2_priv0

  .fini_array (ADDR(.init_array) + SIZEOF(.init_array)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array))
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    __DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    KEEP(*(.dtors.start))
    KEEP(*(.dtors))
    LONG(0)
    __DTOR_END__ = .;
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > L2_priv0

  .boot (ADDR(.fini_array) + SIZEOF(.fini_array)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array))
  {
    . = ALIGN(4);
    *(.boot)
    *(.boot.data)
  } > L2_priv0

  .rodata (ADDR(.boot) + SIZEOF(.boot)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot))
  {
    . = ALIGN(4);
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
  } > L2_priv0

  .got (ADDR(.rodata) + SIZEOF(.rodata)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata))
  {
    . = ALIGN(4);
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
  } > L2_priv0

  .shbss (ADDR(.got) + SIZEOF(.got)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got))
  {
    . = ALIGN(4);
    *(.shbss)
  } > L2_priv0

  .talias (ADDR(.shbss) + SIZEOF(.shbss)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss))
  {
  } > L2_priv0

  .gnu.offload_funcs (ADDR(.talias) + SIZEOF(.talias)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias))
  {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_funcs))
  } > L2_priv0

  .gnu.offload_vars (ADDR(.gnu.offload_funcs) + SIZEOF(.gnu.offload_funcs)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs))
  {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_vars))
  } > L2_priv0

  .stack (ADDR(.gnu.offload_vars) + SIZEOF(.gnu.offload_vars)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars))
  {
    . = ALIGN(4);
    . = ALIGN(16);
    . = . + 0x800;
    stack = .;
  } > L2_priv0

  .data_fc (ADDR(.stack) + SIZEOF(.stack)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack))
  {
    . = ALIGN(4);
    *(.data_fc)
    *(.data_fc.*)
  } > L2_priv0

  .data_fc_shared :
  {
    . = ALIGN(4);
    *(.data_fc_shared)
    *(.data_fc_shared.*)
  } > L2_shared

  .data_tiny_l1 : AT(_l1_preload_start_inL2)
  {
    . = ALIGN(4);
    _l1_preload_start = .;
    *(.data_tiny_l1)
    *(.data_tiny_l1.*)
    *(.data_alias_l1)
    *(.data_alias_l1.*)
  } > L1_aliased

  .tls (ADDR(.data_tiny_l1) + SIZEOF(.data_tiny_l1)): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1))
  {
    . = ALIGN(4);
    _tls_start = .;
    *(.tls)
    *(.tls.*)
    . = ALIGN(4);
    _tls_end = .;
  } > L1_aliased

  .heapsram (ADDR(.tls) + SIZEOF(.tls) * __NB_ACTIVE_PE): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE)
  {
    . = ALIGN(4);
    *(.heapsram)
    *(.heapsram.*)
  } > L1_aliased

  .l1cluster_g (ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram)): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram))
  {
    . = ALIGN(4);
    *(.l1cluster_g)
    *(.l1cluster_g.*)
    *(.data_l1)
    *(.data_l1.*)
    . = ALIGN(4);
    _libgomp_start = .;
    *(.libgomp)
    *(.libgomp.*)
    . = ALIGN(4);
    _heapsram_start = .;
  } > L1

  __l1_heap_start = ALIGN(4);

  __l1_heap_size = LENGTH(L1) - __l1_heap_start + ORIGIN(L1);

  _heapsram_size = LENGTH(L1) - _heapsram_start + ORIGIN(L1);

  _l1_preload_size = SIZEOF(.data_tiny_l1) + SIZEOF(.tls) + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g);

  .bss_l1 (ADDR(.l1cluster_g) + SIZEOF(.l1cluster_g)): AT(ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g))
  {
    . = ALIGN(4);
    *(.bss_l1)
    *(.bss_l1.*)
    _heapsram_start = .;
    __L1_end = ABSOLUTE(.);
  } > L1

  .data_tiny_l1_s (ADDR(.data_fc) + SIZEOF(.data_fc)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc))
  {
    . = ALIGN(4);
    _l1_preload_start_inL2 = .;
  } > L2_priv0

  .tls_s (ADDR(.data_fc) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1))
  {
  } > L2_priv0

  .heapsram_s (ADDR(.data_fc) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE)
  {
  } > L2_priv0

  .l1cluster_g_s (ADDR(.data_fc) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram))
  {
  } > L2_priv0

  .data (ADDR(.data_fc) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g))
  {
    . = ALIGN(4);
    sdata  =  .;
    _sdata  =  .;
    *(.data);
    *(.data.*)
    *(.sdata);
    *(.sdata.*)
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .;
  } > L2_priv0

  .heapl2ram (ADDR(.data) + SIZEOF(.data)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g) + SIZEOF(.data))
  {
    . = ALIGN(4);
    *(.heapl2ram)
    *(.fcTcdm)
    *(.fcTcdm.*)
    *(.fcTcdm_g)
    *(.fcTcdm_g.*)
  } > L2_priv0

  .bss (ADDR(.heapl2ram) + SIZEOF(.heapl2ram)): AT(ORIGIN(L2_priv0) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.rodata) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g) + SIZEOF(.data) + SIZEOF(.heapl2ram))
  {
    . = ALIGN(8);
    _bss_start = .;
    *(.bss)
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
    __L2_priv0_end = ABSOLUTE(.);
  } > L2_priv0

  __fc_data_end = ALIGN(8);

  .shared :
  {
    . = ALIGN(4);
    *(.l2_shared_data)
    *(.l2_shared_data.*)
    __L2_shared_end = ABSOLUTE(.);
  } > L2_shared

  __l2_data_end = ALIGN(8);

  __l2_priv0_heap_start = __fc_data_end;

  __l2_priv0_heap_size = LENGTH(L2_priv0) - __l2_priv0_heap_start + ORIGIN(L2_priv0);

  __l2_priv1_heap_start = __fc_code_end;

  __l2_priv1_heap_size = LENGTH(L2_priv1) - __l2_priv1_heap_start + ORIGIN(L2_priv1);

  __l2_shared_heap_start = __l2_data_end;

  __l2_shared_heap_size = LENGTH(L2_shared) - __l2_shared_heap_start + ORIGIN(L2_shared);

}
