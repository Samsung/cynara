Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.2.2
Release:    1
Group:      Security/Access Control
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
Source1002:    libcynara-client.manifest
Source1003:    libcynara-admin.manifest
Source1004:    cynara-tests.manifest
Source1005:    libcynara-client-commons.manifest
Source1006:    libcynara-commons.manifest
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

%global build_type %{?build_type:%build_type}%{!?build_type:RELEASE}

%if %{?build_type} == "DEBUG"

BuildRequires: pkgconfig(libunwind)

%endif

%description
service, client libraries (libcynara-client, libcynara-admin)
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

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export CXXFLAGS="$CXXFLAGS -DCYNARA_STATE_PATH=\\\"%{state_path}\\\""
export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install

rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/lib/systemd/system/sockets.target.wants
mkdir -p %{buildroot}/%{state_path}
ln -s ../cynara.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket

%pre
id -g %{group_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    groupadd %{group_name} -r > /dev/null 2>&1
fi

id -u %{user_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    useradd -m %{user_name} -r > /dev/null 2>&1
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

%post -n libcynara-client-commons -p /sbin/ldconfig

%postun -n libcynara-client-commons -p /sbin/ldconfig

%post -n libcynara-admin -p /sbin/ldconfig

%postun -n libcynara-admin -p /sbin/ldconfig

%post -n libcynara-commons -p /sbin/ldconfig

%postun -n libcynara-commons -p /sbin/ldconfig

%post -n libcynara-client-devel -p /sbin/ldconfig

%postun -n libcynara-client-devel -p /sbin/ldconfig

%post -n libcynara-client-commons-devel -p /sbin/ldconfig

%postun -n libcynara-client-commons-devel -p /sbin/ldconfig

%post -n libcynara-admin-devel -p /sbin/ldconfig

%postun -n libcynara-admin-devel -p /sbin/ldconfig

%post -n libcynara-commons-devel -p /sbin/ldconfig

%postun -n libcynara-commons-devel -p /sbin/ldconfig

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
%{_libdir}/libcynara-admin.so
%{_libdir}/pkgconfig/cynara-admin.pc

%files -n libcynara-commons
%manifest libcynara-commons.manifest
%license LICENSE
%{_libdir}/libcynara-commons.so.*

%files -n libcynara-commons-devel
%{_libdir}/libcynara-commons.so

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests
