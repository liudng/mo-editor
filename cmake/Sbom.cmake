# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Generates an SPDX 2.3 SBOM for the project using cdxgen (preferred) or
# syft if available. The result is written to
#   ${CMAKE_BINARY_DIR}/hello-${PROJECT_VERSION}.spdx.json
# and is produced when the `sbom` custom target is built.
function(generate_sbom)
    set(SBOM_OUTPUT
        "${CMAKE_BINARY_DIR}/hello-${PROJECT_VERSION}.spdx.json")

    find_program(CDXGEN_BIN NAMES cdxgen)
    find_program(SYFT_BIN NAMES syft)

    if(CDXGEN_BIN)
        message(STATUS "Using cdxgen for SBOM generation: ${CDXGEN_BIN}")
        add_custom_command(
            OUTPUT  ${SBOM_OUTPUT}
            COMMAND ${CDXGEN_BIN}
                    --type spdx-json
                    --output ${SBOM_OUTPUT}
                    ${CMAKE_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating SPDX 2.3 SBOM with cdxgen -> ${SBOM_OUTPUT}"
            VERBATIM
        )
        add_custom_target(sbom DEPENDS ${SBOM_OUTPUT})
    elseif(SYFT_BIN)
        message(STATUS "Using syft for SBOM generation: ${SYFT_BIN}")
        add_custom_command(
            OUTPUT  ${SBOM_OUTPUT}
            COMMAND ${SYFT_BIN}
                    spdx-json
                    --output ${SBOM_OUTPUT}
                    ${CMAKE_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating SPDX 2.3 SBOM with syft -> ${SBOM_OUTPUT}"
            VERBATIM
        )
        add_custom_target(sbom DEPENDS ${SBOM_OUTPUT})
    else()
        message(STATUS "SBOM tool not found, skipping")
    endif()
endfunction()
