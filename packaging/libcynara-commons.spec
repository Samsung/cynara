Name:       libcynara-commons
Summary:    Cynara service with client libraries
Version:    0.13.0
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    cynara-%{version}.tar.gz
Source1000:    %{name}-rpmlintrc
Source1002:    libcynara-client.manifest
Source1003:    libcynara-admin.manifest
Source1004:    cynara-tests.manifest
Source1005:    libcynara-agent.manifest
Source1006:    libcynara-commons.manifest
Source1007:    libcynara-creds-commons.manifest
Source1010:    libcynara-creds-socket.manifest
Source1011:    libcynara-session.manifest
Source1012:    cynara-db-migration.manifest
Source1013:    cyad.manifest
Source1014:    cynara-db-chsgen.manifest
Source1015:    libcynara-monitor.manifest
Requires:      default-ac-domains
Requires(post):   smack
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsmack)
Summary:       Cynara - cynara commons library
Obsoletes:     libcynara-storage

%if !%{defined build_type}
%define build_type RELEASE
%endif

%if %{?build_type} == "DEBUG"

BuildRequires: libdw-devel
BuildRequires: pkgconfig(libunwind)

%endif

%global project_name cynara

%package -n cynara-devel
Summary:    Cynara development files
Requires:   libcynara-admin = %{version}
Requires:   libcynara-agent = %{version}
Requires:   libcynara-client = %{version}
Requires:   libcynara-commons = %{version}
Requires:   libcynara-creds-commons = %{version}
Requires:   libcynara-creds-socket = %{version}
Requires:   libcynara-session = %{version}
Obsoletes:  libcynara-admin-devel
Obsoletes:  libcynara-agent-devel
Obsoletes:  libcynara-client-async-devel
Obsoletes:  libcynara-client-commons-devel
Obsoletes:  libcynara-client-devel
Obsoletes:  libcynara-commons-devel
Obsoletes:  libcynara-creds-commons-devel
Obsoletes:  libcynara-creds-socket-devel
Obsoletes:  libcynara-plugin-devel
Obsoletes:  libcynara-session-devel
Obsoletes:  libcynara-storage-devel

%description -n cynara-devel
Cynara development files

%package -n cynara-tests
Summary:    Cynara - cynara test binaries
BuildRequires: pkgconfig(gmock)

%description -n cynara-tests
Cynara tests

%package -n libcynara-client
Summary:    Cynara - client libraries
Obsoletes:  libcynara-client-commons
Obsoletes:  libcynara-client-async

%description -n libcynara-client
Client libraries for checking policies: synchronous and asynchronous

%package -n libcynara-admin
Summary:    Cynara - admin client library

%description -n libcynara-admin
admin client library for setting, listing and removing policies

%package -n libcynara-agent
Summary:    Cynara - agent client library

%description -n libcynara-agent
agent client library for communication with cynara service and plugins

%description -n libcynara-commons
cynara common library with common functionalities

%package -n libcynara-creds-commons
Summary:    Base library for cynara credentials helpers
Requires:   libcynara-commons = %{version}

%description -n libcynara-creds-commons
Base library for cynara credentials helpers

%package -n libcynara-creds-socket
Summary:    Cynara credentials helpers library for socket clients
Requires:   libcynara-creds-commons = %{version}

%description -n libcynara-creds-socket
Cynara credentials helpers library for socket clients

%package -n libcynara-session
Summary:    Cynara helper client session string creation library
Requires:   libcynara-commons = %{version}

%description -n libcynara-session
Cynara helper client session string creation library

%package -n cynara-db-migration
Summary:    Migration tools for Cynara's database
Requires:   findutils

%description -n cynara-db-migration
Migration tools for Cynara's database

%package -n cyad
Summary: Cynara's command-line tool
Requires:   libcynara-admin = %{version}
Requires:   libcynara-commons = %{version}

%description -n cyad
Command-line tool to manage Cynara's database

%package -n libcynara-monitor
Summary:    Cynara - monitor client library

%description -n libcynara-monitor
Monitor client library for obtaining check logs

%prep
%setup -q -n cynara-%{version}
cp -a %{SOURCE1002} .
cp -a %{SOURCE1003} .
cp -a %{SOURCE1004} .
cp -a %{SOURCE1005} .
cp -a %{SOURCE1006} .
cp -a %{SOURCE1007} .
cp -a %{SOURCE1010} .
cp -a %{SOURCE1011} .
cp -a %{SOURCE1012} .
cp -a %{SOURCE1013} .
cp -a %{SOURCE1014} .
cp -a %{SOURCE1015} .

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DBUILD_TESTS=ON \
        -DBUILD_COMMONS=ON \
        -DBUILD_WITH_SYSTEMD=OFF \
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
        -DDB_FILES_SMACK_LABEL="System"
make %{?jobs:-j%jobs}

%install
#rm -rf %{buildroot}
%make_install

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

%post -n libcynara-creds-socket -p /sbin/ldconfig

%postun -n libcynara-creds-socket -p /sbin/ldconfig

%post -n libcynara-session -p /sbin/ldconfig

%postun -n libcynara-session -p /sbin/ldconfig

%post -n libcynara-monitor -p /sbin/ldconfig

%postun -n libcynara-monitor -p /sbin/ldconfig

%files -n cynara-devel
%{_includedir}/cynara/*.h
%{_includedir}/cynara/attributes/*.h
%{_includedir}/cynara/log/*.h
%{_includedir}/cynara/plugin/*.h
%{_includedir}/cynara/types/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/*.so

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) %{_bindir}/cynara-tests
%attr(755,root,root) %{_bindir}/cynara-db-migration-tests
%attr(755,root,root) %{_datarootdir}/%{project_name}/tests/db*/*
%dir %attr(755,root,root) %{_datarootdir}/%{project_name}/tests/empty_db

%files -n libcynara-client
%manifest libcynara-client.manifest
%license LICENSE
%{_libdir}/libcynara-client.so.*
%{_libdir}/libcynara-client-async.so.*
%{_libdir}/libcynara-client-commons.so.*
%dir %attr(755,cynara,cynara) %{_libdir}/%{project_name}/plugin/client

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
%dir %attr(700,cynara,cynara) %{_localstatedir}/cynara

%files -n libcynara-creds-commons
%manifest libcynara-creds-commons.manifest
%license LICENSE
%{_libdir}/libcynara-creds-commons.so.*
%{_sysconfdir}/%{project_name}/creds.conf

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
%manifest cynara-db-chsgen.manifest
%attr(700,root,root) %{_sbindir}/cynara-db-migration
%attr(700,root,root) %{_sbindir}/cynara-db-chsgen

%files -n cyad
%manifest cyad.manifest
%attr(700,root,root) %{_sbindir}/cyad

%files -n libcynara-monitor
%manifest libcynara-monitor.manifest
%license LICENSE
%{_libdir}/libcynara-monitor.so.*
