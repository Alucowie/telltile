# Copyright © 2024 Anthoine Bourgeois
# SPDX-License-Identifier: MIT

# This undef in needed to catch open syscall and not open64
file_offset_bits_arg = '-U_FILE_OFFSET_BITS'

incdir = include_directories(['../includes'])

wrap_sources = [
  'wrap-util.c',
  'wrap-syscall.c']

dldep = dependency('dl')

shared_library('wrap', wrap_sources, c_args : [file_offset_bits_arg], include_directories : incdir, dependencies : dldep)
