set(installdir "/opt/miktex/2.9.0000")
if(CPACK_GENERATOR MATCHES "DEB")
  set(CPACK_PACKAGING_INSTALL_PREFIX "${installdir}")
elseif(CPACK_GENERATOR MATCHES "RPM")
  set(CPACK_PACKAGING_INSTALL_PREFIX "${installdir}")
endif()
