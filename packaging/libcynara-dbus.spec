Name:       libcynara-creds-dbus
Summary:    Cynara service with client libraries
Version:    0.14.4
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    cynara-%{version}.tar.gz
Source1000:    %{name}-rpmlintrc
Source1008:    libcynara-creds-dbus.manifest
Source1009:    libcynara-creds-gdbus.manifest
Requires:      default-ac-domains
Requires:      libcynara-commons = %{version}
Requires:      dbus
Requires:      libcynara-creds-commons = %{version}
Requires(pre): cynara-db-migration >= %{version}
Requires(post):   smack
Requires(postun): cynara-db-migration >= %{version}
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsmack)
BuildRequires: pkgconfig(dbus-1)
Summary:       Cynara credentials helpers library for dbus clients

%global project_name cynara

%if !%{defined build_type}
%define build_type RELEASE
%endif

%if %{?build_type} == "DEBUG"

BuildRequires: libdw-devel
BuildRequires: pkgconfig(libunwind)

%endif

%package -n libcynara-creds-dbus-devel
Summary:    Development files for dbus helpers library
Requires:   cynara-devel = %{version}
Requires:   libcynara-creds-dbus = %{version}
Requires:   pkgconfig(dbus-1)

%description -n libcynara-creds-dbus-devel
Development files for dbus helpers library

%package -n libcynara-creds-gdbus-devel
Summary:    Development files for gdbus helpers library
Requires:   cynara-devel = %{version}
Requires:   libcynara-creds-gdbus = %{version}
Requires:   pkgconfig(glib-2.0)

%description -n libcynara-creds-gdbus-devel
Development files for gdbus helpers library

%description -n libcynara-creds-dbus
Cynara credentials helpers library for dbus clients

%package -n libcynara-creds-gdbus
Summary:    Cynara credentials helpers library for gdbus client
BuildRequires: pkgconfig(gio-2.0)
Requires:   libcynara-creds-commons = %{version}

%description -n libcynara-creds-gdbus
Cynara credentials helpers library for gdbus clients

%prep
%setup -q -n cynara-%{version}
cp -a %{SOURCE1008} .
cp -a %{SOURCE1009} .

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DBUILD_TESTS=OFF \
        -DBUILD_DBUS=ON \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DLIB_DIR:PATH=%{_libdir} \
        -DBIN_DIR:PATH=%{_bindir} \
        -DSBIN_DIR:PATH=%{_sbindir} \
        -DSYS_CONFIG_DIR:PATH=%{_sysconfdir} \
        -DINCLUDE_DIR:PATH=%{_includedir} \
        -DLOCAL_STATE_DIR:PATH=%{_localstatedir} \
        -DDATA_ROOT_DIR:PATH=%{_datadir} \
        -DSYSTEMD_UNIT_DIR:PATH=%{_unitdir} \
        -DSOCKET_DIR:PATH=/run/%{project_name} \
        -DDB_FILES_SMACK_LABEL="System" \
        -DMONITORING=ON
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -n libcynara-creds-dbus -p /sbin/ldconfig

%postun -n libcynara-creds-dbus -p /sbin/ldconfig

%post -n libcynara-creds-gdbus -p /sbin/ldconfig

%postun -n libcynara-creds-gdbus -p /sbin/ldconfig

%files -n libcynara-creds-dbus-devel
%{_includedir}/cynara/cynara-creds-dbus.h
%{_libdir}/pkgconfig/cynara-creds-dbus.pc
%{_libdir}/libcynara-creds-dbus.so

%files -n libcynara-creds-gdbus-devel
%{_includedir}/cynara/cynara-creds-gdbus.h
%{_libdir}/pkgconfig/cynara-creds-gdbus.pc
%{_libdir}/libcynara-creds-gdbus.so

%files -n libcynara-creds-dbus
%manifest libcynara-creds-dbus.manifest
%license LICENSE
%{_libdir}/libcynara-creds-dbus.so.*

%files -n libcynara-creds-gdbus
%manifest libcynara-creds-gdbus.manifest
%license LICENSE
%{_libdir}/libcynara-creds-gdbus.so.*

