Summary: datadec - generate C code from recursive data declarations
Name: datadec
Version: 1.2
Release: 1
Group: Text/Utilities
Copyright: Public Domain
Source: http://csgsoft.doc.ic.ac.uk/datadec/datadec-1.0.tgz
BuildRoot: /tmp/datadec

%description
Datadec transforms functional (Hope/Miranda/Haskell) style
recursive data declarations into a definition/implementation pair of
ANSI C files containing type declarations, constructor functions,
deconstructor functions and printing functions for the recursive data types.

New in 1.2 (Summer 2014) 
experimental free functions (run datadec with new -f option)

%prep
%setup
%build
make

%install
PREFIX=$RPM_BUILD_ROOT make install

%files
%attr(-,root,root) %dir /usr
%attr(-,root,root) %dir /usr/local
%attr(-,root,root) %dir /usr/local/bin
%attr(-,root,root) %dir /usr/man
%attr(-,root,root) %dir /usr/man/man1
%attr(-,root,root) /usr/local/bin/datadec
%attr(-,root,root) /usr/man/man1/datadec.1.gz
