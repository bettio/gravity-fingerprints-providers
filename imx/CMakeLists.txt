set(iMXProvider_SRCS
    imxprovider.cpp
)

add_library(imx-fingerprints-provider SHARED ${iMXProvider_SRCS})

target_link_libraries(imx-fingerprints-provider
                      Gravity::Fingerprints
                      Qt5::Core
                      HemeraQt5SDK::Core)

# Install phase
install(TARGETS imx-fingerprints-provider
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${GRAVITY_FINGERPRINTS_PROVIDERS_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${GRAVITY_FINGERPRINTS_PROVIDERS_DIR}" COMPONENT shlib
        COMPONENT hemera-fingerprints)
