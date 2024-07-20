# Copyright © 2024 Anthoine Bourgeois
# SPDX-License-Identifier: MIT

cargs = [
  '-D_GNU_SOURCE',
  # This undef in needed to catch open syscall and not open64
  '-U_FILE_OFFSET_BITS',
  '-DLINUX',
  '-DDEBUG',
  # PowerVR SGX defines
  '-DSGX530',
  '-DSGX_CORE_REV=110',
  '-DTRANSFER_QUEUE',
  '-DSUPPORT_SGX_NEW_STATUS_VALS',
  '-DSUPPORT_SGX_HWPERF',
  '-DSUPPORT_SGX',
  '-DSUPPORT_SGX530',
  '-DSUPPORT_HW_RECOVERY',
  '-DSUPPORT_ACTIVE_POWER_MANAGEMENT',
  '-DSUPPORT_PERCONTEXT_PB',
  '-DSUPPORT_SGX_PRIORITY_SCHEDULING',
  '-DSUPPORT_SGX_LOW_LATENCY_SCHEDULING',
  '-DPDS_BUILD_OPENGLES',
  '-DSUPPORT_OPENGL',
  '-DSUPPORT_OPENGLES2',
  '-DAPI_MODULES_RUNTIME_CHECKED',
  '-DEURASIA_TAG_TEXTURE_STATE_SIZE=3',
  '-DPVR_SECURE_HANDLES',
  '-DSGXTQ_SUBTILE_TWIDDLING',
  ]

incdir = include_directories(['../includes'])

wrap_sources = [
  'wrap-util.c',
  'wrap-pvr-u.c',
  'wrap-pvr.c',
  'wrap-sgx.c',
  'wrap-pvr-mmap.c',
  'wrap-osfunc.c',
  'wrap-pvr-dc.c',
  'wrap-syscall.c']

dldep = dependency('dl')

shared_library('wrap', wrap_sources, c_args : cargs, include_directories : incdir, dependencies : dldep)
