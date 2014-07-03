Name:       cynara-tests
Summary:    Cynara tests
Version:    0.0.1
Release:    1
Group:      Development/Testing
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara-tests.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(gmock)

%description
Cynara tests

%global build_type %{?build_type:%build_type}%{!?build_type:RELEASE}

%prep
%setup -q
cp -a %{SOURCE1001} .

%build
%cmake test -DCMAKE_BUILD_TYPE=%{?build_type} \
            -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install
%make_install

%files
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests
