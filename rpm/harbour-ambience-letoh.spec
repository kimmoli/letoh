#
# LeTOH spec File
# 

Name:       harbour-ambience-letoh

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    LeTOH Control Application
Version:    0.8.0
Release:    devel
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/kimmoli/letoh
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 0.0.10
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Multimedia)
BuildRequires:  pkgconfig(libiphb)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  desktop-file-utils

Requires: ambienced

Obsoletes: harbour-letoh

%description
Disco LED TOH Otherhalf (letoh)


%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 SPECVERSION=%{version}
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/harbour-letoh
%attr(755,root,root) %{_bindir}/harbour-letohd
%{_sysconfdir}/systemd/system/
%{_sysconfdir}/udev/rules.d/
%{_sysconfdir}/dbus-1/system.d/
%{_datadir}/dbus-1/interfaces/
%{_datadir}/icons/hicolor/86x86/apps/
%{_datadir}/applications/
%{_datadir}/harbour-letoh/
%{_datadir}/harbour-letoh/qml/
%{_datadir}/ambience/%{name}/
%{_datadir}/ambience/%{name}/images/

%pre
# In case of update, stop and disable first
if [ "$1" = "2" ]; then
  systemctl stop harbour-letohd.service
  systemctl disable harbour-letohd.service
  udevadm control --reload
fi

%post
#reload dbus config
dbus-send --system --type=method_call --dest=org.freedesktop.DBus / org.freedesktop.DBus.ReloadConfig
#reload udev rules
udevadm control --reload
# if letohd is connected, start daemon now
if [ -e /sys/devices/platform/toh-core.0/vendor ]; then
 if grep -q 19276 /sys/devices/platform/toh-core.0/vendor ; then
  if grep -q 5 /sys/devices/platform/toh-core.0/product ; then
   systemctl start harbour-letohd.service
  fi
 fi
fi
%_ambience_post

%preun
# in case of complete removal, stop and disable
if [ "$1" = "0" ]; then
  systemctl stop harbour-letohd.service
  systemctl disable harbour-letohd.service
  udevadm control --reload
fi
