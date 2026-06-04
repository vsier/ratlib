# SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)

project("ratlib_project" C)

include(GNUInstallDirs)

option(RAT_INSTALL "Generate ratlib install rules." ${PROJECT_IS_TOP_LEVEL})
option(RAT_BUILD_TESTS "Build ratlib tests." OFF)

add_subdirectory("rat")

if (RAT_BUILD_TESTS)
	enable_testing()
	add_subdirectory("tests")
endif()

if (RAT_INSTALL)
	install(FILES "LICENSE" "README.md"
		DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/ratlib")
endif()
