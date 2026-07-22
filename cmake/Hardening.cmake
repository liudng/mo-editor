# SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# Applies compile/link hardening flags to the given target.
# Note: _FORTIFY_SOURCE requires optimization (-O2 or higher), so it is
# only enabled for Release and RelWithDebInfo configurations.
function(apply_hardening target_name)
    add_library(mo_hardening INTERFACE)

    target_compile_options(mo_hardening INTERFACE
        -fstack-protector-strong
        -fcf-protection
        -fstack-clash-protection
        -fPIE
        $<$<CONFIG:Release,RelWithDebInfo>:-D_FORTIFY_SOURCE=3>
    )

    target_link_options(mo_hardening INTERFACE
        -Wl,-z,relro,-z,now
        -Wl,-z,noexecstack
        -pie
    )

    target_link_libraries(${target_name} PRIVATE mo_hardening)
endfunction()
