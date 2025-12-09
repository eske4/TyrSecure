#message(STATUS "======================================")
#message(STATUS " Checking required packages")
#message(STATUS "======================================")

# ---- Check for pkg-config ----
find_package(PkgConfig)

if(PkgConfig_FOUND)
  #message(STATUS "✅ pkg-config found")
else()
  message(
    FATAL_ERROR
      "\n❌ pkg-config not found.\n"
      "It is required to locate system libraries.\n\n" "Install it with:\n"
      "  Arch: sudo pacman -S pkgconf\n")
endif()

# ---- Check for libbpf ----
pkg_check_modules(LIBBPF libbpf>=0.7)

if(LIBBPF_FOUND)
  #message(STATUS "✅ libbpf found")
  #message(STATUS "   Version   : ${LIBBPF_VERSION}")
  #message(STATUS "   Includes  : ${LIBBPF_INCLUDE_DIRS}")
  #message(STATUS "   Libraries : ${LIBBPF_LIBRARIES}")
else()
  message(FATAL_ERROR "\n❌ libbpf >= 0.7 not found (skeleton API required).\n"
                      "Install it with:\n" "  Arch: sudo pacman -S libbpf\n")
endif()

# add packages here
