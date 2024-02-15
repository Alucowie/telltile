# Copyright © 2024 Anthoine Bourgeois
# SPDX-License-Identifier: MIT

project('telltile', 'c', license : 'MIT', version : '0.1')

add_project_arguments('-DLINUX', language : 'c')
add_project_arguments('-DDEBUG', language : 'c')

add_project_arguments('-DSGX530', language : 'c')
add_project_arguments('-DSGX_CORE_REV=110', language : 'c')
add_project_arguments('-DTRANSFER_QUEUE', language : 'c')
add_project_arguments('-DSUPPORT_SGX_NEW_STATUS_VALS', language : 'c')
add_project_arguments('-DSUPPORT_SGX_HWPERF', language : 'c')
add_project_arguments('-DSUPPORT_SGX', language : 'c')
add_project_arguments('-DSUPPORT_SGX530', language : 'c')
add_project_arguments('-DSUPPORT_HW_RECOVERY', language : 'c')
add_project_arguments('-DSUPPORT_ACTIVE_POWER_MANAGEMENT', language : 'c')
add_project_arguments('-DSUPPORT_PERCONTEXT_PB', language : 'c')
add_project_arguments('-DSUPPORT_SGX_PRIORITY_SCHEDULING', language : 'c')
add_project_arguments('-DSUPPORT_SGX_LOW_LATENCY_SCHEDULING', language : 'c')
add_project_arguments('-DPDS_BUILD_OPENGLES', language : 'c')
add_project_arguments('-DSUPPORT_OPENGL', language : 'c')
add_project_arguments('-DSUPPORT_OPENGLES2', language : 'c')
add_project_arguments('-DAPI_MODULES_RUNTIME_CHECKED', language : 'c')
add_project_arguments('-DEURASIA_TAG_TEXTURE_STATE_SIZE=3', language : 'c')
add_project_arguments('-DPVR_SECURE_HANDLES', language : 'c')
add_project_arguments('-DSGXTQ_SUBTILE_TWIDDLING', language : 'c')

subdir('wrap')
