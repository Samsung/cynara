Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.4.2
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1000:    %{name}-rpmlintrc
Source1001:    cynara.manifest
Source1002:    libcynara-client.manifest
Source1003:    libcynara-admin.manifest
Source1004:    cynara-tests.manifest
Source1005:    libcynara-agent.manifest
Source1006:    libcynara-commons.manifest
Source1007:    libcynara-creds-commons.manifest
Source1008:    libcynara-creds-dbus.manifest
Source1009:    libcynara-creds-socket.manifest
Source1010:    libcynara-session.manifest
Source1011:    cynara-db-migration.manifest
Requires:      default-ac-domains
Requires(pre): pwdutils
Requires(pre): cynara-db-migration >= %{version}-%{release}
Requires(post):   smack
Requires(postun): pwdutils
Requires(postun): cynara-db-migration >= %{version}-%{release}
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(libsystemd-journal)
%{?systemd_requires}

%global user_name %{name}
%global group_name %{name}

%global state_path %{_localstatedir}/%{name}/
%global lib_path %{_libdir}/%{name}/
%global tests_dir %{_datarootdir}/%{name}/tests/
%global conf_path %{_sysconfdir}/%{name}/

%if !%{defined build_type}
%define build_type RELEASE
%endif

%if %{?build_type} == "DEBUG"

BuildRequires: libdw-devel
BuildRequires: pkgconfig(libunwind)

%endif

%description
service, client libraries (libcynara-client, libcynara-admin),
agent library, helper libraries (libcynara-session, libcynara-creds-common, libcynara-creds-dbus,
libcynara-creds-socket) and tests (cynara-tests)

%package devel
Summary:    Cynara development files
Requires:   libcynara-admin = %{version}-%{release}
Requires:   libcynara-agent = %{version}-%{release}
Requires:   libcynara-client = %{version}-%{release}
Requires:   libcynara-commons = %{version}-%{release}
Requires:   libcynara-creds-commons = %{version}-%{release}
Requires:   libcynara-creds-dbus = %{version}-%{release}
Requires:   libcynara-creds-socket = %{version}-%{release}
Requires:   libcynara-session = %{version}-%{release}
Requires:   pkgconfig(dbus-1)
Obsoletes:  libcynara-admin-devel
Obsoletes:  libcynara-agent-devel
Obsoletes:  libcynara-client-async-devel
Obsoletes:  libcynara-client-commons-devel
Obsoletes:  libcynara-client-devel
Obsoletes:  libcynara-commons-devel
Obsoletes:  libcynara-creds-commons-devel
Obsoletes:  libcynara-creds-dbus-devel
Obsoletes:  libcynara-creds-socket-devel
Obsoletes:  libcynara-plugin-devel
Obsoletes:  libcynara-session-devel
Obsoletes:  libcynara-storage-devel

%description devel
Cynara development files

%package tests
Summary:    Cynara - cynara test binaries
BuildRequires: pkgconfig(gmock)

%description tests
Cynara tests

%package -n libcynara-client
Summary:    Cynara - client libraries
Requires:   cynara = %{version}-%{release}
Obsoletes:  libcynara-client-commons
Obsoletes:  libcynara-client-async

%description -n libcynara-client
Client libraries for checking policies: synchronous and asynchronous

%package -n libcynara-admin
Summary:    Cynara - admin client library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-admin
admin client library for setting, listing and removing policies

%package -n libcynara-agent
Summary:    Cynara - agent client library
Requires:   cynara = %{version}-%{release}

%description -n libcynara-agent
agent client library for communication with cynara service and plugins

%package -n libcynara-commons
Summary:    Cynara - cynara commons library
Obsoletes:  libcynara-storage

%description -n libcynara-commons
cynara common library with common functionalities

%package -n libcynara-creds-commons
Summary:    Base library for cynara credentials helpers

%description -n libcynara-creds-commons
Base library for cynara credentials helpers

%package -n libcynara-creds-dbus
Summary:    Cynara credentials helpers library for dbus clients
BuildRequires: pkgconfig(dbus-1)
Requires:   dbus

%description -n libcynara-creds-dbus
Cynara credentials helpers library for dbus clients

%package -n libcynara-creds-socket
Summary:    Cynara credentials helpers library for socket clients

%description -n libcynara-creds-socket
Cynara credentials helpers library for socket clients

%package -n libcynara-session
Summary:    Cynara helper client session string creation library

%description -n libcynara-session
Cynara helper client session string creation library

%package -n cynara-db-migration
Summary:    Migration tools for Cynara's database

%description -n cynara-db-migration
Migration tools for Cynara's database

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
cp -a test/db/db* .

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export CXXFLAGS="$CXXFLAGS -DCYNARA_STATE_PATH=\\\"%{state_path}\\\" \
                           -DCYNARA_LIB_PATH=\\\"%{lib_path}\\\" \
                           -DCYNARA_TESTS_DIR=\\\"%{tests_dir}\\\" \
                           -DCYNARA_CONFIGURATION_DIR=\\\"%{conf_path}\\\""
export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DBUILD_TESTS=ON \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/%{conf_path}
cp ./conf/creds.conf %{buildroot}/%{conf_path}/creds.conf

mkdir -p %{buildroot}/usr/lib/systemd/system/sockets.target.wants
mkdir -p %{buildroot}/%{state_path}
mkdir -p %{buildroot}/%{tests_dir}/empty_db
mkdir -p %{buildroot}/%{lib_path}/plugin/client
mkdir -p %{buildroot}/%{lib_path}/plugin/service

cp -a db* %{buildroot}/%{tests_dir}
ln -s ../cynara.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket
ln -s ../cynara-agent.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-agent.socket

%pre
id -g %{group_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    groupadd %{group_name} -r > /dev/null 2>&1
fi

id -u %{user_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    useradd -d /var/lib/empty -s /sbin/nologin -r -g %{group_name} %{user_name} > /dev/null 2>&1
fi

if [ $1 -gt 1 ] ; then
    OLDVERSION="$(rpm -q --qf '%%{version}' %{name})"
    %{_sbindir}/cynara/cynara-db-migration.sh upgrade -f ${OLDVERSION} -t %{version}
else
    %{_sbindir}/cynara/cynara-db-migration.sh install -t %{version}
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
    %{_sbindir}/cynara/cynara-db-migration.sh uninstall -f %{version}
    userdel -r %{user_name} > /dev/null 2>&1
    groupdel %{user_name} > /dev/null 2>&1
    systemctl daemon-reload
fi

%post -n libcynara-client -p /sbin/ldconfig

%postun -n libcynara-client -p /sbin/ldconfig

%post -n libcynara-admin -p /sbin/ldconfig

%postun -n libcynara-admin -p /sbin/ldconfig

%post -n libcynara-agent -p /sbin/ldconfig

%postun -n libcynara-agent -p /sbin/ldconfig

%post -n libcynara-commons -p /sbin/ldconfig

%postun -n libcynara-commons -p /sbin/ldconfig

%post -n libcynara-creds-commons -p /sbin/ldconfig

%postun -n libcynara-creds-commons -p /sbin/ldconfig

%post -n libcynara-creds-dbus -p /sbin/ldconfig

%postun -n libcynara-creds-dbus -p /sbin/ldconfig

%post -n libcynara-creds-socket -p /sbin/ldconfig

%postun -n libcynara-creds-socket -p /sbin/ldconfig

%post -n libcynara-session -p /sbin/ldconfig

%postun -n libcynara-session -p /sbin/ldconfig

%files
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/cynara
%attr(-,root,root) /usr/lib/systemd/system/cynara.service
%attr(-,root,root) /usr/lib/systemd/system/cynara.target
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara-admin.socket
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara-agent.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara-agent.socket
%dir %attr(700,cynara,cynara) %{state_path}
%dir %attr(755,cynara,cynara) %{lib_path}/plugin/service

%files -n cynara-devel
%{_includedir}/cynara/*.h
%{_includedir}/cynara/plugin/*.h
%{_includedir}/cynara/types/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/*.so

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests
%attr(755,root,root) %{tests_dir}/db*/*
%dir %attr(755,root,root) %{tests_dir}/empty_db

%files -n libcynara-client
%manifest libcynara-client.manifest
%license LICENSE
%{_libdir}/libcynara-client.so.*
%{_libdir}/libcynara-client-async.so.*
%{_libdir}/libcynara-client-commons.so.*
%dir %attr(755,cynara,cynara) %{lib_path}/plugin/client

%files -n libcynara-admin
%manifest libcynara-admin.manifest
%license LICENSE
%{_libdir}/libcynara-admin.so.*

%files -n libcynara-agent
%manifest libcynara-agent.manifest
%license LICENSE
%{_libdir}/libcynara-agent.so.*

%files -n libcynara-commons
%manifest libcynara-commons.manifest
%license LICENSE
%{_libdir}/libcynara-commons.so.*
%{_libdir}/libcynara-storage.so.*

%files -n libcynara-creds-commons
%manifest libcynara-creds-commons.manifest
%license LICENSE
%{_libdir}/libcynara-creds-commons.so.*
%{conf_path}creds.conf

%files -n libcynara-creds-dbus
%manifest libcynara-creds-dbus.manifest
%license LICENSE
%{_libdir}/libcynara-creds-dbus.so.*

%files -n libcynara-creds-socket
%manifest libcynara-creds-socket.manifest
%license LICENSE
%{_libdir}/libcynara-creds-socket.so.*

%files -n libcynara-session
%manifest libcynara-session.manifest
%license LICENSE
%{_libdir}/libcynara-session.so.*

%files -n cynara-db-migration
%manifest cynara-db-migration.manifest
%attr(700,root,root) %{_sbindir}/cynara/cynara-db-migration.sh
