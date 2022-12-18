# norootforbuild





# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





%define major_version  0
%define minor_version  0
%define patch_version  1
%define full_version   %{major_version}.%{minor_version}.%{patch_version}
%define min_qt_version 5.15.0





Name:           libperceptualcolor-%{major_version}
Version:        %{full_version}
# Set “Release” to “0” as placeholder; OpenBuildService will substitute it
# with the correct number automatically:
Release:        0
# See https://fedoraproject.org/wiki/Packaging:LicensingGuidelines?rd=Packaging/LicensingGuidelines#Valid_License_Short_Names
# and https://en.opensuse.org/openSUSE:Packaging_guidelines#Spec_Files for
# details. In either case, MIT is a valid identifier.
License:        MIT
# See https://en.opensuse.org/openSUSE:Package_group_guidelines#Libraries
# for details about valid group names:
Group:          Development/Libraries/C++
Summary:        Library providing perceptually uniform color widgets
# TODO Summary(de.UTF-8):  German translation
URL:            https://github.com/sommerluk/perceptualcolor
Source0:        libperceptualcolor-%{major_version}-%{full_version}.tar.gz
BuildRoot:      %{_tmppath}/build
BuildRequires:  cmake >= 3.15
BuildRequires:  extra-cmake-modules
# The package “binutils” is required in order to get the “strip” command:
BuildRequires:  binutils
# BuildRequires everything that is necessary to work with the source package;
# this includes some packages like the Qt test framework, that are not
# strictly necessary for production builds:
BuildRequires:  cmake(Qt5Core) >= %{min_qt_version}
BuildRequires:  cmake(Qt5Gui) >= %{min_qt_version}
BuildRequires:  cmake(Qt5Widgets) >= %{min_qt_version}
BuildRequires:  cmake(Qt5LinguistTools) >= %{min_qt_version}
BuildRequires:  cmake(Qt5Test) >= %{min_qt_version}
BuildRequires:  cmake(Qt5Concurrent) >= %{min_qt_version}
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires:  lcms2-devel
%endif
%if 0%{?suse_version}
BuildRequires:  liblcms2-devel
%endif
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
# The OpenBuildService needs disambiguation between the glibc-langpack-en
# and glibc-all-langpacks packages, one of which is required by the
# OpenBuildService itself:
BuildRequires:  glibc-all-langpacks
%endif
%description
# When changing this, also update manually ../deb/debian/control
#<description> Start of description that is extracted for i18n
The library uses the CIE LCh/Lab color space as a perceptually uniform base.
The widgets provide various perceptually uniform visualizations either of
the sRGB gamut (build-in) or of an arbitrary gamut (loaded from an ICC file),
with focus on usability also for users that do not know about color spaces.
#</description>
# TODO %%description -l de.UTF-8
# TODO Description in German
%package        devel
Summary:        Development package for %{name}
%description    devel
This package includes the header files for developing applications
that use %{name}.
%package        internal
Summary:        Components for the internal development of %{name} itself
License:        MIT and CC0 and AGPLv3+
%description    internal
This package is not useful for programmers wishing to use %{name} in their
programs, but only for developing %{name} itself. It contains some internal
tools. It also contains a complete API documentation of %{name} – not only
for the public part, but also for the private part.
%package        doc
Summary:        Public API documentation of %{name}
%description    doc
Public API Documentation of %{name} for programmers wishing to use %{name}
in their programs.





%prep
# “-q” means quiet.
# “-n” sets the directory, which has to be the top-level directory contained
# in the source archive.
%setup -q -n libperceptualcolor
# TODO Use %%autosetup instead? Or get rid of it entirely?





%build
%cmake
# “cmake_build” will automatically use multi-thread
# building to speed up the compilation process:
%cmake_build
# Strip unneeded symbols from the library. Indeed, there is a complain from
# rpmlint for Opensuse if this stripping is not done. We only strip for the
# production library, but not for the internal tools. The “--strip-unneeded”
# option is save for both, static and dynamic libraries, under all conditions.
# See https://www.technovelty.org/linux/stripping-shared-libraries.html
%if 0%{?suse_version}
strip --strip-unneeded lib/libperceptualcolor-%{major_version}.so.%{full_version}
%endif





%install
%cmake_install





# The scriptlet in %%post is run after a package is installed.
# The scriptlet in %%postun is run after a package is uninstalled.
# NOTE Using the syntax with “-p” will add ldconfig as a dependency
# to the package, which is good.
%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%post -n %{name}-devel -p /sbin/ldconfig
%postun -n %{name}-devel -p /sbin/ldconfig
%post -n %{name}-internal -p /sbin/ldconfig
%postun -n %{name}-internal -p /sbin/ldconfig





%files
# Leave file permissions “as is”, but change owner and group to “root”:
%defattr(-,root,root)
%{_libdir}/libperceptualcolor-%{major_version}.so
%{_libdir}/libperceptualcolor-%{major_version}.so.%{major_version}
%attr(644, -, root) %{_libdir}/libperceptualcolor-%{major_version}.so.%{full_version}
%license LICENSE

%files devel
%defattr(-,root,root)
%dir %{_includedir}/PerceptualColor
%{_includedir}/PerceptualColor/*
%{_libdir}/cmake/perceptualcolor-0.cmake
%if 0%{?suse_version}
%{_libdir}/cmake/perceptualcolor-0-relwithdebinfo.cmake
%endif
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
%{_libdir}/cmake/perceptualcolor-0-release.cmake
%endif
%license LICENSE

%files internal
%defattr(-,root,root)
%{_bindir}/perceptualcolorpicker
%{_bindir}/perceptualcolorgeneratescreenshots
%{_bindir}/perceptualcolortest*
%{_libdir}/libperceptualcolorinternal-%{major_version}.so
%{_libdir}/libperceptualcolorinternal-%{major_version}.so.%{major_version}
%attr(644, -, root) %{_libdir}/libperceptualcolorinternal-%{major_version}.so.%{full_version}
%license LICENSE
# TODO Add the private API documentation

%files doc
%defattr(-,root,root)
%license LICENSE
# TODO Add the public API documentation: %%docdir …





%changelog
# TODO Is it possible to refer to an external file like CHANGELOG.txt instead
# of maintaining an own changelog for this .spec file?
* Sun Dec 12 2021 0.1
- fix bugs in the spec file
* Sat Dec 11 2021 0.0.1
- initial spec file
