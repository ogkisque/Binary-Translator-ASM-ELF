cd front_end
./front
cd ..

cd middle_end
./middle
cd ..

cd back_end
./back
cd ..

nasm -f elf64 asm.asm
ld asm.o -o elf_from_asm

chmod u+x elf

