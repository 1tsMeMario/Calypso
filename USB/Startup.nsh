echo -off

cls

if exist fs0:\AthenaEFI.efi then
fs0:
load AthenaEFI.efi
goto EXIT
endif

if exist fs1:\AthenaEFI.efi then
fs1:
load AthenaEFI.efi
goto EXIT
endif

if exist fs2:\AthenaEFI.efi then
fs2:
load AthenaEFI.efi
goto EXIT
endif

if exist fs3:\AthenaEFI.efi then
fs3:
load AthenaEFI.efi
goto EXIT
endif

if exist fs4:\AthenaEFI.efi then
fs4:
load AthenaEFI.efi
goto EXIT
endif

if exist fs5:\AthenaEFI.efi then
fs5:
load AthenaEFI.efi
goto EXIT
endif

if exist fs6:\AthenaEFI.efi then
fs6:
load AthenaEFI.efi
goto EXIT
endif

if exist fs7:\AthenaEFI.efi then
fs7:
load AthenaEFI.efi
goto EXIT
endif

if exist fs8:\AthenaEFI.efi then
fs8:
load AthenaEFI.efi
goto EXIT
endif

if exist fs9:\AthenaEFI.efi then
fs9:
load AthenaEFI.efi
goto EXIT
endif

load AthenaEFI.efi
goto EXIT

:EXIT
stall 5000000
exit
