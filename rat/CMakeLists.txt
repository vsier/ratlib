# SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
# SPDX-License-Identifier: MIT

project("rat" C)

include(GenerateExportHeader)
include(GNUInstallDirs)

if (NOT DEFINED RAT_INSTALL)
	option(RAT_INSTALL "Generate ratlib install rules." ${PROJECT_IS_TOP_LEVEL})
endif()

add_library(rat STATIC)
target_compile_features(rat PUBLIC c_std_99)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(rat PRIVATE "/W4" "/wd4200")
endif()


set(ratGeneratedHeadersDir "${CMAKE_CURRENT_BINARY_DIR}/include/rat")

set(ratConfigCachelineSize 64 CACHE STRING "rat cacheline size")
string(REGEX REPLACE "[^A-Za-z0-9_]" "_" ratConfigSystemProcessor "${CMAKE_SYSTEM_PROCESSOR}")
configure_file("include/rat/rat_config.h.in" "${ratGeneratedHeadersDir}/rat_config.h" @ONLY)

generate_export_header(rat
	EXPORT_FILE_NAME "${ratGeneratedHeadersDir}/rat_export.h")


target_sources(rat
	PUBLIC FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
	FILES
	"${ratGeneratedHeadersDir}/rat_config.h"
	"${ratGeneratedHeadersDir}/rat_export.h"
	"include/rat/common.h"
	"include/rat/extra.h"
	"include/rat/ralc.h"
	"include/rat/ralcArena.h"
	"include/rat/ralcCustom.h"
	"include/rat/ralcGen.h"
	"include/rat/ralcPool.h"
	"include/rat/rarr.h"
	"include/rat/rbit.h"
	"include/rat/rcmp.h"
	"include/rat/rcrc.h"
	"include/rat/rdyn.h"
	"include/rat/rhash.h"
	"include/rat/rlist.h"
	"include/rat/rmap.h"
	"include/rat/rring.h"
	PRIVATE
	"src/rat_ralc.c"
	"src/rat_ralcArena.c"
	"src/rat_ralcCustom.c"
	"src/rat_ralcGen.c"
	"src/rat_ralcPool.c"
	"src/rat_rarr.c"
	"src/rat_rcmp.c"
	"src/rat_rcrc.c"
	"src/rat_rdyn.c"
	"src/rat_rhash.c"
	"src/rat_rlist.c"
	"src/rat_rmap.c"
	"src/rat_rring.c"
)

if (RAT_INSTALL)
	install(TARGETS rat
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()
