#gcc -m32 -Wall -O1 -g -c -s -fvisibility=hidden cod4_extfunc.c
#gcc -m32 -Wall  -fvisibility=hidden -mfpmath=sse -march=atom -c cod4_extfunc.c
#gcc -m32 -Wall  -fvisibility=hidden -march=atom -c cod4_extfunc.c

#gcc -m32 -Wall -O1 -fvisibility=hidden -fno-omit-frame-pointer -c cod4_extfunc.c
#gcc -m32 -Wall -O1 -s -fvisibility=hidden -fno-omit-frame-pointer -mtune=prescott -c cod4_extfunc.c

gcc -m32 -Wall -O1 -g -fno-omit-frame-pointer -mtune=prescott -c *.c
#gcc -m32 -Wall -O1 -s -fvisibility=hidden -fno-omit-frame-pointer -march=core2 -c cod4_extfunc.c


#gcc -m32 -Wall -O1 -s -fvisibility=hidden -fno-omit-frame-pointer -march=core2 -mcx16 -msahf -mpopcnt -msse4.2 --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=256 -c cod4_extfunc.c


#gcc -m32 -Wall -O1 -g -fvisibility=hidden -fno-omit-frame-pointer -march=corei7 -c cod4_extfunc.c
#gcc -m32 -Wall -O1 -g -fvisibility=hidden -fno-omit-frame-pointer -mfpmath=sse -march=atom -c cod4_extfunc.c
#gcc -m32 -Wall -O1 -fvisibility=hidden -fno-omit-frame-pointer -march=amdfam10 -c cod4_extfunc.c

#gcc -m32 -s -shared -fvisibility=hidden -Wl,-soname,cod4x17a.so -o cod4x17a.so *.o
nasm -f elf qcommon_hooks.asm
nasm -f elf cmd_hooks.asm
nasm -f elf cvar_hooks.asm
nasm -f elf filesystem_hooks.asm

gcc -m32 -Wl,-ldl,-lm,-Tlinkerscript.ld -o cod4x17a_dedrun *.o 
#gcc -m32 -shared -fvisibility=hidden -Wl,-soname,cod4x17a.so -o cod4x17a.so *.o

rm *.o

./version_make_progress.sh