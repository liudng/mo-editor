# SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
# SPDX-License-Identifier: GPL-3.0-or-later

Name:           hello
Version:        0.1.0
Release:        1%{?dist}
Summary:        A simple Qt6 text editor

License:        GPL-3.0-or-later
URL:            https://github.com/liudng/hello
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-linguist
BuildRequires:  qt6-qttools-devel
BuildRequires:  kf6-syntax-highlighting-devel
BuildRequires:  uchardet-devel
BuildRequires:  wayland-devel
BuildRequires:  wayland-protocols-devel
BuildRequires:  libxkbcommon-devel
BuildRequires:  desktop-file-utils

Requires:       qt6-qtbase
Requires:       kf6-syntax-highlighting
Requires:       uchardet

%description
Hello is a modern text editor built with Qt6 Widgets. It is designed to be
fast, lightweight, and friendly for both casual note-taking and source code
editing on Wayland-based Linux desktops.

Features:
  * Multi-tab interface
  * Syntax highlighting (200+ languages via KSyntaxHighlighting)
  * Automatic encoding detection (libuchardet)
  * Session restore
  * Find and replace
  * Wayland native

%prep
%autosetup

%build
%cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DHELLO_BUILD_TESTS=OFF
%cmake_build

%install
%cmake_install

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/io.github.liudng.hello.desktop || :

%files
%license LICENSE
%doc README.md
%{_bindir}/hello
%{_datadir}/applications/io.github.liudng.hello.desktop
%{_datadir}/metainfo/io.github.liudng.hello.metainfo.xml
%{_datadir}/dbus-1/services/io.github.liudng.hello.service
%{_datadir}/mime/packages/io.github.liudng.hello.xml
%{_datadir}/icons/hicolor/scalable/apps/io.github.liudng.hello.svg
%{_datadir}/hello/translations/hello_zh_CN.qm
%{_mandir}/man1/hello.1*

%changelog
* Mon Jul 21 2026 liudng <liudng@users.noreply.github.com> - 0.1.0-1
- Initial RPM release.
