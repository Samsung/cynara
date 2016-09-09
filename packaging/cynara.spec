Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.14.4
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1000:    %{name}-rpmlintrc
Source1001:    cynara.manifest
Requires:      default-ac-domains
Requires:      libcynara-commons = %{version}
Requires(post):   smack
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(libsmack)
BuildRequires: pkgconfig(cynara-commons)
%{?systemd_requires}

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
libcynara-creds-socket, libcynara-creds-self) and tests (cynara-tests)

%prep
%setup -q -n cynara-%{version}
cp -a %{SOURCE1001} .

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
        -DBUILD_WITH_SYSTEMD=ON \
        -DBUILD_SERVICE=ON \
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
        -DSOCKET_DIR:PATH=/run/%{name} \
        -DDB_FILES_SMACK_LABEL="System" \
        -DMONITORING=ON
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}%{_unitdir}/sockets.target.wants
ln -s ../cynara.socket %{buildroot}%{_unitdir}/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}%{_unitdir}/sockets.target.wants/cynara-admin.socket
ln -s ../cynara-agent.socket %{buildroot}%{_unitdir}/sockets.target.wants/cynara-agent.socket
ln -s ../cynara-monitor-get.socket %{buildroot}%{_unitdir}/sockets.target.wants/cynara-monitor-get.socket

%post
### Add file capabilities if needed
### setcap/getcap binary are useful. To use them you must install libcap and libcap-tools packages
### In such case uncomment Requires with those packages

systemctl daemon-reload

if [ $1 = 1 ]; then
    systemctl enable %{name}.service
fi

systemctl restart %{name}.service

%preun
if [ $1 = 0 ]; then
    # unistall
    systemctl stop cynara.service
fi

%files
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) %{_bindir}/cynara
%attr(-,root,root) %{_unitdir}/cynara.service
%attr(-,root,root) %{_unitdir}/cynara.target
%attr(-,root,root) %{_unitdir}/sockets.target.wants/cynara.socket
%attr(-,root,root) %{_unitdir}/cynara.socket
%attr(-,root,root) %{_unitdir}/sockets.target.wants/cynara-admin.socket
%attr(-,root,root) %{_unitdir}/cynara-admin.socket
%attr(-,root,root) %{_unitdir}/sockets.target.wants/cynara-agent.socket
%attr(-,root,root) %{_unitdir}/cynara-agent.socket
%attr(-,root,root) %{_unitdir}/sockets.target.wants/cynara-monitor-get.socket
%attr(-,root,root) %{_unitdir}/cynara-monitor-get.socket
%dir %attr(755,cynara,cynara) %{_libdir}/%{name}/plugin/service

