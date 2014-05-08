Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.0.1
Release:    1
Group:      Security/Access Control
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
Source1002:    libcynara-client.manifest
Source1003:    libcynara-admin.manifest
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsystemd-daemon)
%{?systemd_requires}

%description
service and client libraries (libcynara-client, libcynara-admin)

#######################################################
%package -n libcynara-client
Summary:    Cynara - client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: pkgconfig(security-server)

%description -n libcynara-client
client library for checking policies

%package -n libcynara-client-devel
Summary:    Cynara - client library (devel)
Requires:   libcynara-client = %{version}-%{release}

%description -n libcynara-client-devel
client library (devel) for checking policies

#######################################################
%package -n libcynara-admin
Summary:    Cynara - admin client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-admin
admin client library for setting, listing and removing policies

%package -n libcynara-admin-devel
Summary:    Cynara - admin client library (devel)
Requires:   libcynara-admin = %{version}-%{release}

%description -n libcynara-admin-devel
admin client library (devel) for setting, listing and removing policies

#######################################################
%package -n cynara-devel
Summary:    Cynara service (devel)
Requires:   cynara = %{version}-%{release}

%description -n cynara-devel
service (devel version)


%prep
%setup -q
cp -a %{SOURCE1001} .
cp -a %{SOURCE1002} .
cp -a %{SOURCE1003} .

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . -DVERSION=%{version} \
        -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/lib/systemd/system/multi-user.target.wants
ln -s ../cynara.service %{buildroot}/usr/lib/systemd/system/multi-user.target.wants/cynara.service
mkdir -p %{buildroot}/usr/lib/systemd/system/sockets.target.wants
ln -s ../cynara.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket

%post
systemctl daemon-reload
if [ $1 = 1 ]; then
    # installation
    systemctl start cynara.service
fi

if [ $1 = 2 ]; then
    # update
    systemctl restart cynara.service
fi

/sbin/ldconfig

%preun
if [ $1 = 0 ]; then
    # unistall
    systemctl stop cynara.service
fi

%postun
if [ $1 = 0 ]; then
    # unistall
    systemctl daemon-reload
fi
/sbin/ldconfig

%post -n libcynara-client -p /sbin/ldconfig

%postun -n libcynara-client -p /sbin/ldconfig

%post -n libcynara-admin -p /sbin/ldconfig

%postun -n libcynara-admin -p /sbin/ldconfig

%post -n libcynara-client-devel -p /sbin/ldconfig

%postun -n libcynara-client-devel -p /sbin/ldconfig

%post -n libcynara-admin-devel -p /sbin/ldconfig

%postun -n libcynara-admin-devel -p /sbin/ldconfig

%files -n cynara
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/cynara
%{_libdir}/libcynara-commons.so*
%attr(-,root,root) /usr/lib/systemd/system/multi-user.target.wants/cynara.service
%attr(-,root,root) /usr/lib/systemd/system/cynara.service
%attr(-,root,root) /usr/lib/systemd/system/cynara.target
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara-admin.socket

%files -n libcynara-client
%manifest libcynara-client.manifest
%license LICENSE
%defattr(-,root,root,-)
%{_libdir}/libcynara-client.so.*

%files -n libcynara-client-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-client.h
%{_libdir}/pkgconfig/cynara-client.pc
%{_libdir}/libcynara-client.so

%files -n libcynara-admin
%manifest libcynara-admin.manifest
%license LICENSE
%defattr(-,root,root,-)
%{_libdir}/libcynara-admin.so.*

%files -n libcynara-admin-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-admin.h
%{_libdir}/pkgconfig/cynara-admin.pc
%{_libdir}/libcynara-admin.so
