# Copyright © 2024 Anthoine Bourgeois
# SPDX-License-Identifier: MIT

cargs = [
  '-D_GNU_SOURCE',
  # This undef in needed to catch open syscall and not open64
  '-U_FILE_OFFSET_BITS',
  '-DLINUX',
  '-DDEBUG',
  ]

sgxversion = get_option('sgx_version')

conf_sgx = configuration_data()
conf_sgx.set('sgxversion', sgxversion)
if sgxversion == '530'
  conf_sgx.set('corerev', 110)
else
  conf_sgx.set('corerev', 100)
endif

configure_file(input : 'config_sgx.h.in',
               output : 'config_sgx.h',
               configuration : conf_sgx)

incdir = include_directories(['.', '../includes'])

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
