#include <stdio.h>
#include <x86intrin.h>

int main ()
{
    // 10000 раз считаем факториал 8
    long long t1 = __rdtsc ();
    system ("./my_proc");
    long long t2 = __rdtsc ();
    system ("./elf");
    long long t3 = __rdtsc ();
    system ("./elf_from_asm");
    long long t4 = __rdtsc ();
    system ("./llvm");
    long long t5 = __rdtsc ();

    long long time_my_proc = t2 - t1;
    long long time_elf = t3 - t2;
    long long time_elf_from_asm = t4 - t3;
    long long time_llvm = t5 - t4;

    printf ("my processor - %lld\n"
            "my elf - %lld\n"
            "elf from  my asm - %lld\n"
            "elf from llvm - %lld\n",
            time_my_proc, time_elf, time_elf_from_asm, time_llvm);
}