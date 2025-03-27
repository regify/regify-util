%define libname libregify-util

Summary: The regify utility package.
Name: regify-util
Version: %{ver}
Release: %{buildrev}
License: This project is released under the MIT License.
Group: Development/Libraries

Source0: %{name}-%{version}.tar.gz
URL: https://www.regify.com/
BuildRoot: %{_tmppath}/%{name}-%{version}-root-%(%{__id_u} -n)
Requires: libicu

%package devel
Group: Development/Libraries
Summary: regify utility headers
Requires: %{name} = %{version}-%{buildrev} libicu-devel

%description
The regify utility package contains general utility functions used in various
regify C packages.

%description devel
C header files for the regify utility package.

The regify utility package contains general utility functions used in various
regify C packages.

%prep
%setup -q

%build
mkdir build
(cd build && cmake -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux \
    -DREVISION=%{buildrev} -DVERSION=%{ver} \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIB_FOLDER=%{_lib} \
    -DCPACK_PACKAGE_FILE_NAME=%{name} -DDOCS=ON \
    %{cmakeparams} -DBUILD_SHARED_LIBS=ON .. && \
    make package)

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
bld=build/_CPack_Packages/lin.x86_64/ZIP/regify-util/

install -d $RPM_BUILD_ROOT/%{_libdir}
install -m 755 $bld/%{_lib}/%{libname}.so.1.0.0 $RPM_BUILD_ROOT%{_libdir}/
ln -s %{libname}.so.1.0.0 $RPM_BUILD_ROOT%{_libdir}/%{libname}.so.1
ln -s %{libname}.so.1 $RPM_BUILD_ROOT%{_libdir}/%{libname}.so

# devel
install -d $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 $bld/include/%{name}.h $RPM_BUILD_ROOT/%{_includedir}
install -m 644 $bld/include/%{name}/*.h $RPM_BUILD_ROOT/%{_includedir}/%{name}/

install -d $RPM_BUILD_ROOT/%{_libdir}
install -m 755 $bld/%{_lib}/%{libname}.a $RPM_BUILD_ROOT%{_libdir}/
install -d $RPM_BUILD_ROOT/%{_libdir}/cmake/%{name}
install -m 644 $bld/%{_lib}/cmake/%{name}/*.cmake $RPM_BUILD_ROOT/%{_libdir}/cmake/%{name}/
install -d $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/
install -m 644 $bld/%{_lib}/pkgconfig/%{name}.pc $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/

install -d $RPM_BUILD_ROOT/%{_datadir}/%{name}
install -m 644 $bld/share/%{name}/regify-util-license.txt $RPM_BUILD_ROOT/%{_datadir}/%{name}/

%post
ldconfig 

%postun
if [ $1 = '0' ]; then
  ldconfig 
fi

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_libdir}/%{libname}.so
%{_libdir}/%{libname}.so.*

%files devel
%defattr(-,root,root,-)
%{_datadir}/%{name}/
%{_includedir}/regify-util/
%{_includedir}/regify-util.h
%{_libdir}/cmake/%{name}/
%{_libdir}/%{libname}.a
%{_libdir}/pkgconfig/regify-util.pc

%changelog
* Wed Nov 30 2022 Mario Theodoridis <mario.theodoridis@regify.com> 1.3.0
- make lib relocatable
- more features

* Fri May 20 2022 Mario Theodoridis <mario.theodoridis@regify.com> 1.2.0
- adjust build for open sourced version

* Mon Jan 20 2020 Mario Theodoridis <mario.theodoridis@regify.com> 1.0.0
- initial RPM release
