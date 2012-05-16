#
# Spec file for the iDigi package
#
Name:           idigi
Version:        1.1
Release:        1%{?dist}
Summary:        Library which are used to connect to the cloud

Group:         System Environment/Base
License:       GPLv2
URL:           http://www.idigi.com/idigiconnector
Source:        %{name}-%{version}.tar.gz
BuildRoot:     %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Vendor:        Digi International Inc.

%description
Library routines used to connect to the Digi cloud

%changelog
* Tue Apr 08 2012 Initial Release - 1.1-1
- Version 1.1 of the iDigi Connector software

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

# If it isn't in the file list, it won't be put in the package
%files
%defattr(-,root,root,-)
%{_libdir}/libidigi.so
%{_includedir}/idigi_types.h
%{_includedir}/idigi_connector.h


#%post -p /sbin/ldconfig
#%postun -p /sbin/ldconfig
