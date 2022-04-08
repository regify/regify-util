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
mkdir build-so
(cd build-so && cmake -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_BUILD_TYPE=Release -DNAME_SOVERSION=%{sover}\
    -DCMAKE_SYSTEM_NAME=Linux -DNAME_VERSION=%{ver} \
    -DNAME_REVISION=%{buildrev} \
    -DBUILD_SHARED_LIBS=ON .. && make %{name})
# mkdir build-a
# (cd build-a && cmake -DCMAKE_VERBOSE_MAKEFILE=ON \
#     -DCMAKE_BUILD_TYPE=Release \
#     -DCMAKE_SYSTEM_NAME=Linux -DNAME_VERSION=%{ver} \
#     -DNAME_REVISION=%{buildrev} \
#     -DBUILD_SHARED_LIBS=OFF .. && make %{name})

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

install -d $RPM_BUILD_ROOT/%{_libdir}
install -m 755 build-so/%{libname}.so.%{sover} $RPM_BUILD_ROOT%{_libdir}/
(cd $RPM_BUILD_ROOT%{_libdir}/ && ln -s %{libname}.so.%{sover} %{libname}.so)

# devel
install -d $RPM_BUILD_ROOT/%{_libdir}
#install -m 755 build-a/%{libname}.a $RPM_BUILD_ROOT%{_libdir}/
install -d $RPM_BUILD_ROOT/%{_includedir}/%{name}
install -m 644 include/%{name}.h $RPM_BUILD_ROOT/%{_includedir}
install -m 644 include/%{name}/*.h $RPM_BUILD_ROOT/%{_includedir}/%{name}/
install -d $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/
install -m 644 build-so/%{name}.pc $RPM_BUILD_ROOT/%{_libdir}/pkgconfig/

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
%{_libdir}/%{libname}.so.%{sover}

%files devel
%defattr(-,root,root,-)
%{_includedir}/regify-util.h
%{_includedir}/regify-util/
#%{_libdir}/%{libname}.a
%{_libdir}/pkgconfig/regify-util.pc

%changelog
* Mon Jan 20 2020 Mario Theodoridis <mario.theodoridis@regify.com> 1.0.0
- initial RPM release
