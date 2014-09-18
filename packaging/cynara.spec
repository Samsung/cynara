Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.3.0
Release:    1
Group:      Security/Access Control
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
Source1002:    libcynara-client.manifest
Source1003:    libcynara-client-async.manifest
Source1004:    libcynara-admin.manifest
Source1005:    cynara-tests.manifest
Source1006:    libcynara-client-commons.manifest
Source1007:    libcynara-commons.manifest
Source1008:    libcynara-creds-commons.manifest
Source1009:    libcynara-creds-dbus.manifest
Source1010:    libcynara-creds-socket.manifest
Source1011:    libcynara-session.manifest
Source1012:    libcynara-storage.manifest
Requires:      default-ac-domains
Requires(pre): pwdutils
Requires(post):   smack
Requires(postun): pwdutils
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(libsystemd-journal)
%{?systemd_requires}

%global user_name %{name}
%global group_name %{name}

%global state_path %{_localstatedir}/%{name}/
%global tests_dir %{_datarootdir}/%{name}/tests

%global build_type %{?build_type:%build_type}%{!?build_type:RELEASE}

%if %{?build_type} == "DEBUG"

BuildRequires: libdw-devel
BuildRequires: pkgconfig(libunwind)

%endif

%description
service, client libraries (libcynara-client, libcynara-client-async, libcynara-admin),
helper libraries (libcynara-session, libcynara-creds-common, libcynara-creds-dbus,
libcynara-creds-socket)
and tests (cynara-tests)

#######################################################
%package -n libcynara-client
Summary:    Cynara - client library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-client
client library for checking policies

%package -n libcynara-client-devel
Summary:    Cynara - client library (devel)
Requires:   libcynara-client = %{version}-%{release}
Requires:   libcynara-client-commons-devel = %{version}-%{release}

%description -n libcynara-client-devel
client library (devel) for checking policies

#######################################################
%package -n libcynara-client-async
Summary:    Cynara - asynchronous client library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-client-async
asynchronous client library for checking policies

%package -n libcynara-client-async-devel
Summary:    Cynara - asynchronous client library (devel)
Requires:   libcynara-client-async = %{version}-%{release}
Requires:   libcynara-client-commons-devel = %{version}-%{release}

%description -n libcynara-client-async-devel
asynchronous client library (devel) for checking policies

#######################################################
%package -n libcynara-client-commons
Summary:    Cynara - client commons library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-client-commons
client commons library with common functionalities

%package -n libcynara-client-commons-devel
Summary:    Cynara - client commons library (devel)
Requires:   libcynara-client-commons = %{version}-%{release}

%description -n libcynara-client-commons-devel
client commons library (devel) with common functionalities

#######################################################
%package -n libcynara-admin
Summary:    Cynara - admin client library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-admin
admin client library for setting, listing and removing policies

%package -n libcynara-admin-devel
Summary:    Cynara - admin client library (devel)
Requires:   libcynara-admin = %{version}-%{release}

%description -n libcynara-admin-devel
admin client library (devel) for setting, listing and removing policies

#######################################################
%package -n libcynara-storage
Summary:    Cynara - storage
Requires:   cynara = %{version}-%{release}

%description -n libcynara-storage
cynara common storage library with common storage functionalities

%package -n libcynara-storage-devel
Summary:    Cynara - storage-devel
Requires:   cynara = %{version}-%{release}

%description -n libcynara-storage-devel
cynara common storage library (devel) with common storage functionalities

#######################################################
%package -n libcynara-commons
Summary:    Cynara - cynara commons library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-commons
cynara common library with common functionalities

%package -n libcynara-commons-devel
Summary:    Cynara - cynara commons library (devel)
Requires:   libcynara-commons = %{version}-%{release}

%description -n libcynara-commons-devel
cynara common library (devel) with common functionalities

#######################################################
%package -n libcynara-plugin-devel
Summary:    Cynara - cynara plugin library (devel)
Requires:   libcynara-commons-devel = %{version}-%{release}

%description -n libcynara-plugin-devel
cynara plugin library (devel) with plugin definitions

#######################################################
%package -n libcynara-creds-commons
Summary:    Base library for cynara credentials helpers

%description -n libcynara-creds-commons
Base library for cynara credentials helpers

%package -n libcynara-creds-commons-devel
Summary:    Base library for cynara credentials helpers (devel)
Requires:   libcynara-creds-commons = %{version}-%{release}
Requires:   libcynara-client-commons-devel = %{version}-%{release}

%description -n libcynara-creds-commons-devel
Base library for cynara credentials helpers (devel)

#######################################################
%package -n libcynara-creds-dbus
Summary:    Cynara credentials helpers library for dbus clients
BuildRequires: pkgconfig(dbus-1)
Requires:   dbus

%description -n libcynara-creds-dbus
Cynara credentials helpers library for dbus clients

%package -n libcynara-creds-dbus-devel
Summary:    Cynara credentials helpers library for dbus clients (devel)
Requires:   libcynara-creds-dbus = %{version}-%{release}
Requires:   libcynara-creds-commons-devel = %{version}-%{release}

%description -n libcynara-creds-dbus-devel
Cynara credentials helpers library for dbus clients (devel)

#######################################################
%package -n libcynara-creds-socket
Summary:    Cynara credentials helpers library for socket clients

%description -n libcynara-creds-socket
Cynara credentials helpers library for socket clients

%package -n libcynara-creds-socket-devel
Summary:    Cynara credentials helpers library for socket clients (devel)
Requires:   libcynara-creds-socket = %{version}-%{release}
Requires:   libcynara-creds-commons-devel = %{version}-%{release}

%description -n libcynara-creds-socket-devel
Cynara credentials helpers library for socket clients (devel)

#######################################################
%package -n libcynara-session
Summary:    Cynara helper client session string creation library

%description -n libcynara-session
Cynara helper client session string creation library

%package -n libcynara-session-devel
Summary:    Cynara helper client session string creation library (devel)
Requires:   libcynara-session = %{version}-%{release}

%description -n libcynara-session-devel
Cynara helper client session string creation library (devel)

#######################################################
%package -n cynara-tests
Summary:    Cynara - cynara test binaries
BuildRequires: pkgconfig(gmock)

%description -n cynara-tests
Cynara tests

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
cp -a %{SOURCE1004} .
cp -a %{SOURCE1005} .
cp -a %{SOURCE1006} .
cp -a %{SOURCE1007} .
cp -a %{SOURCE1008} .
cp -a %{SOURCE1009} .
cp -a %{SOURCE1010} .
cp -a %{SOURCE1011} .
cp -a %{SOURCE1012} .
cp -a test/db/db* .

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export CXXFLAGS="$CXXFLAGS -DCYNARA_STATE_PATH=\\\"%{state_path}\\\" \
                           -DCYNARA_TESTS_DIR=\\\"%{tests_dir}\\\""
export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DBUILD_TESTS=ON \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install

rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/lib/systemd/system/sockets.target.wants
mkdir -p %{buildroot}/%{state_path}
mkdir -p %{buildroot}/%{tests_dir}
cp -a db* %{buildroot}/%{tests_dir}
ln -s ../cynara.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket

%pre
id -g %{group_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    groupadd %{group_name} -r > /dev/null 2>&1
fi

id -u %{user_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    useradd -d /var/lib/empty -s /sbin/nologin -r -g %{group_name} %{user_name} > /dev/null 2>&1
fi

%post
### Add file capabilities if needed
### setcap/getcap binary are useful. To use them you must install libcap and libcap-tools packages
### In such case uncomment Requires with those packages

systemctl daemon-reload

if [ $1 = 1 ]; then
    systemctl enable %{name}.service
fi

chsmack -a System %{state_path}

systemctl restart %{name}.service

%preun
if [ $1 = 0 ]; then
    # unistall
    systemctl stop cynara.service
fi

%postun
if [ $1 = 0 ]; then
    userdel -r %{user_name} > /dev/null 2>&1
    groupdel %{user_name} > /dev/null 2>&1
    systemctl daemon-reload
fi

%post -n libcynara-client -p /sbin/ldconfig

%postun -n libcynara-client -p /sbin/ldconfig

%post -n libcynara-client-async -p /sbin/ldconfig

%postun -n libcynara-client-async -p /sbin/ldconfig

%post -n libcynara-client-commons -p /sbin/ldconfig

%postun -n libcynara-client-commons -p /sbin/ldconfig

%post -n libcynara-admin -p /sbin/ldconfig

%postun -n libcynara-admin -p /sbin/ldconfig

%post -n libcynara-storage -p /sbin/ldconfig

%postun -n libcynara-storage -p /sbin/ldconfig

%post -n libcynara-storage-devel -p /sbin/ldconfig

%postun -n libcynara-storage-devel -p /sbin/ldconfig

%post -n libcynara-commons -p /sbin/ldconfig

%postun -n libcynara-commons -p /sbin/ldconfig

%post -n libcynara-client-devel -p /sbin/ldconfig

%postun -n libcynara-client-devel -p /sbin/ldconfig

%post -n libcynara-client-async-devel -p /sbin/ldconfig

%postun -n libcynara-client-async-devel -p /sbin/ldconfig

%post -n libcynara-client-commons-devel -p /sbin/ldconfig

%postun -n libcynara-client-commons-devel -p /sbin/ldconfig

%post -n libcynara-admin-devel -p /sbin/ldconfig

%postun -n libcynara-admin-devel -p /sbin/ldconfig

%post -n libcynara-commons-devel -p /sbin/ldconfig

%postun -n libcynara-commons-devel -p /sbin/ldconfig

%post -n libcynara-creds-commons -p /sbin/ldconfig

%postun -n libcynara-creds-commons -p /sbin/ldconfig

%post -n libcynara-creds-commons-devel -p /sbin/ldconfig

%postun -n libcynara-creds-commons-devel -p /sbin/ldconfig

%post -n libcynara-creds-dbus -p /sbin/ldconfig

%postun -n libcynara-creds-dbus -p /sbin/ldconfig

%post -n libcynara-creds-dbus-devel -p /sbin/ldconfig

%postun -n libcynara-creds-dbus-devel -p /sbin/ldconfig

%post -n libcynara-creds-socket -p /sbin/ldconfig

%postun -n libcynara-creds-socket -p /sbin/ldconfig

%post -n libcynara-creds-socket-devel -p /sbin/ldconfig

%postun -n libcynara-creds-socket-devel -p /sbin/ldconfig

%post -n libcynara-session -p /sbin/ldconfig

%postun -n libcynara-session -p /sbin/ldconfig

%post -n libcynara-session-devel -p /sbin/ldconfig

%postun -n libcynara-session-devel -p /sbin/ldconfig

%files -n cynara
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/cynara
%attr(-,root,root) /usr/lib/systemd/system/cynara.service
%attr(-,root,root) /usr/lib/systemd/system/cynara.target
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara-admin.socket
%dir %attr(700,cynara,cynara) %{state_path}

%files -n libcynara-client
%manifest libcynara-client.manifest
%license LICENSE
%{_libdir}/libcynara-client.so.*

%files -n libcynara-client-devel
%{_includedir}/cynara/cynara-client.h
%{_libdir}/pkgconfig/cynara-client.pc
%{_libdir}/libcynara-client.so

%files -n libcynara-client-async
%manifest libcynara-client-async.manifest
%license LICENSE
%{_libdir}/libcynara-client-async.so.*

%files -n libcynara-client-async-devel
%{_includedir}/cynara/cynara-client-async.h
%{_libdir}/pkgconfig/cynara-client-async.pc
%{_libdir}/libcynara-client-async.so

%files -n libcynara-client-commons
%manifest libcynara-client-commons.manifest
%license LICENSE
%{_libdir}/libcynara-client-commons.so.*

%files -n libcynara-client-commons-devel
%{_includedir}/cynara/cynara-client-error.h
%{_libdir}/libcynara-client-commons.so

%files -n libcynara-admin
%manifest libcynara-admin.manifest
%license LICENSE
%{_libdir}/libcynara-admin.so.*

%files -n libcynara-admin-devel
%{_includedir}/cynara/cynara-admin.h
%{_includedir}/cynara/cynara-admin-error.h
%{_includedir}/cynara/cynara-admin-types.h
%{_libdir}/libcynara-admin.so
%{_libdir}/pkgconfig/cynara-admin.pc

%files -n libcynara-storage
%manifest libcynara-storage.manifest
%license LICENSE
%{_libdir}/libcynara-storage.so.*

%files -n libcynara-storage-devel
%{_libdir}/libcynara-storage.so

%files -n libcynara-commons
%manifest libcynara-commons.manifest
%license LICENSE
%{_libdir}/libcynara-commons.so.*

%files -n libcynara-commons-devel
%{_includedir}/cynara/types/PolicyResult.h
%{_includedir}/cynara/types/PolicyType.h
%{_libdir}/libcynara-commons.so

%files -n libcynara-plugin-devel
%{_includedir}/cynara/cynara-plugin.h
%{_libdir}/pkgconfig/cynara-plugin.pc

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests
%attr(755,root,root) %{tests_dir}/db*/*

%files -n libcynara-creds-commons
%manifest libcynara-creds-commons.manifest
%license LICENSE
%{_libdir}/libcynara-creds-commons.so.*

%files -n libcynara-creds-commons-devel
%{_includedir}/cynara/cynara-creds-commons.h
%{_libdir}/libcynara-creds-commons.so
%{_libdir}/pkgconfig/cynara-creds-commons.pc

%files -n libcynara-creds-dbus
%manifest libcynara-creds-dbus.manifest
%license LICENSE
%{_libdir}/libcynara-creds-dbus.so.*

%files -n libcynara-creds-dbus-devel
%{_includedir}/cynara/cynara-creds-dbus.h
%{_libdir}/libcynara-creds-dbus.so
%{_libdir}/pkgconfig/cynara-creds-dbus.pc

%files -n libcynara-creds-socket
%manifest libcynara-creds-socket.manifest
%license LICENSE
%{_libdir}/libcynara-creds-socket.so.*

%files -n libcynara-creds-socket-devel
%{_includedir}/cynara/cynara-creds-socket.h
%{_libdir}/libcynara-creds-socket.so
%{_libdir}/pkgconfig/cynara-creds-socket.pc

%files -n libcynara-session
%manifest libcynara-session.manifest
%license LICENSE
%{_libdir}/libcynara-session.so.*

%files -n libcynara-session-devel
%{_includedir}/cynara/cynara-session.h
%{_libdir}/libcynara-session.so
%{_libdir}/pkgconfig/cynara-session.pc
