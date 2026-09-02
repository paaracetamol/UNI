# Este trabalho foi integralmente realizado pelos alunos
# Daniel Santos (l70070) e Bernardo Gamula (l58440)
# sem recurso a ferramentas de inteligencia artificial.


.data

w:      .word 0
h:      .word 0
img:    .word 0



fname:  .space 100


# file
fd:     .word 0
lbuf:   .space 128
lpos:   .word 0





# temps
a:      .word 0
b:      .word 0
c:      .word 0
d:      .word 0

nl:     .byte '\n'


msgin:   .string "input .myvg: "
msgerr:  .string "erro\n"
msgok:   .string "done\n"
oname:   .string "out.txt"




.text
        j main
        .globl main



sign:
        bgt a0, zero, sp1
        blt  a0, zero, sn1
        ret
sp1:    li   a0, 1
        ret
sn1:    li  a0, -1
        ret




absv:
        bge  a0, zero, ab1
        neg  a0, a0
ab1:    ret



pstr:
        li   a7, 4
        ecall
        ret




# linha -> lbuf, -1=eof
readln:
        li t0, 0   # i
rl1:    li   t1, 127
        bge  t0, t1, rl3  # buf cheio
        la   t2, lbuf
        add a1, t2, t0
        la   t3, fd
        lw   a0, 0(t3)
        li   a2, 1
        li   a7, 63
        ecall
        ble  a0, zero, rl4  # eof

        la   t2, lbuf
        add  t2, t2, t0
        lbu  t4, 0(t2)
        li   t5, '\n'
        beq  t4, t5, rl3    # fim
        addi t0, t0, 1
        j    rl1

rl4:    beq  t0, zero, rl5  # nada
rl3:    la   t2, lbuf
        add  t2, t2, t0
        sb   zero, 0(t2) # \0
        la   t3, lpos
        sw   zero, 0(t3)
        mv   a0, t0
        ret
rl5:    li   a0, -1
        ret




# peek
peekc:
        la   t0, lpos
        lw   t1, 0(t0)
        la   t2, lbuf
        add  t2, t2, t1
        lbu  a0, 0(t2)
        bne a0, zero, pk1
        li   a0, -1
pk1:    ret



# next byte, -1=fim
nextc:
        la   t0, lpos
        lw   t1, 0(t0)
        la   t2, lbuf
        add  t2, t2, t1
        lbu  a0, 0(t2)
        bne  a0, zero, nx1
        li   a0, -1
        ret
nx1:    addi t1, t1, 1
        sw   t1, 0(t0)
        ret





# nao salta \n
skipsp:
        addi sp, sp, -4
        sw   ra, 0(sp)
ss1:    call peekc
        li   t0, ' '
        beq  a0, t0, ss2
        li t0, '\t'
        beq  a0, t0, ss2
        j    ss3
ss2:    call nextc
        j    ss1
ss3:    lw   ra, 0(sp)
        addi sp, sp, 4
        ret



# val*10 = (val<<3)+(val<<1)
pint:
        addi sp, sp, -16
        sw   ra, 12(sp)
        sw   s0,  8(sp)
        sw   s1,  4(sp)
        sw   s2,  0(sp)
        call skipsp
        li   s0, 0
        li   s1, 0

        call peekc
        li   t0, '-'
        bne  a0, t0, pi1
        call nextc
        li   s0, 1
pi1:
pi2:    call peekc
        li   t0, '0'
        blt  a0, t0, pi3
        li   t0, '9'
        bgt  a0, t0, pi3
        call nextc
        li   t0, '0'
        sub  s2, a0, t0
        slli t1, s1, 3
        slli t2, s1, 1
        add  s1, t1, t2
        add  s1, s1, s2
        j    pi2
pi3:    mv   a0, s1
        beq  s0, zero, pi4
        neg  a0, a0
pi4:    lw   ra, 12(sp)
        lw   s0,  8(sp)
        lw   s1,  4(sp)
        lw   s2,  0(sp)
        addi sp, sp, 16
        ret



# point(x=a0, y=a1)
point:
        blt  a0, zero, pte
        la   t0, w
        lw   t0, 0(t0)
        bge  a0, t0, pte
        blt  a1, zero, pte
        la   t1, h
        lw   t1, 0(t1)
        bge  a1, t1, pte
        la   t2, img
        lw   t2, 0(t2)
        slli t3, a1, 2
        add  t2, t2, t3
        lw   t2, 0(t2) # img[y]
        add  t2, t2, a0
        li   t3, '#'
        sb   t3, 0(t2)
pte:    ret






# line(x1=a0, y1=a1, x2=a2, y2=a3) -> bresenham
line:
        addi sp, sp, -48
        sw   ra, 44(sp)
        sw   s0, 40(sp)
        sw   s1, 36(sp)
        sw   s2, 32(sp)
        sw   s3, 28(sp)
        sw   s4, 24(sp) # dx
        sw   s5, 20(sp) # dy
        sw   s6, 16(sp) # sx
        sw   s7, 12(sp) # sy
        sw   s8,  8(sp) # e
        sw   s9,  4(sp) # e2
        mv   s0, a0
        mv   s1, a1
        mv   s2, a2
        mv   s3, a3

        sub  a0, s2, s0
        call absv
        mv   s4, a0
        sub  a0, s3, s1
        call absv
        neg  s5, a0
        sub  a0, s2, s0
        call sign
        mv   s6, a0
        sub  a0, s3, s1
        call sign
        mv   s7, a0
        add  s8, s4, s5

ln1:    mv   a0, s0
        mv   a1, s1
        call point
        bne  s0, s2, ln2
        beq  s1, s3, lne
ln2:    slli s9, s8, 1
        blt  s9, s5, ln3
            beq  s0, s2, lne
            add  s8, s8, s5
            add  s0, s0, s6
ln3:    bgt  s9, s4, ln4
            beq  s1, s3, lne
            add  s8, s8, s4
            add  s1, s1, s7
ln4:    j    ln1

lne:    lw   ra, 44(sp)
        lw   s0, 40(sp)
        lw   s1, 36(sp)
        lw   s2, 32(sp)
        lw   s3, 28(sp)
        lw   s4, 24(sp)
        lw   s5, 20(sp)
        lw   s6, 16(sp)
        lw   s7, 12(sp)
        lw   s8,  8(sp)
        lw   s9,  4(sp)
        addi sp, sp, 48
        ret




# 4 linhas
rect:
        addi sp, sp, -20
        sw   ra, 16(sp)
        sw   s0, 12(sp)
        sw   s1,  8(sp)
        sw   s2,  4(sp)
        sw   s3,  0(sp)
        mv   s0, a0
        mv   s1, a1
        mv   s2, a2
        mv   s3, a3

        mv   a0, s0
        mv   a1, s1
        mv   a2, s2
        mv   a3, s1
        call line
        mv   a0, s2
        mv   a1, s1
        mv   a2, s2
        mv   a3, s3
        call line

        mv   a0, s2
        mv   a1, s3
        mv   a2, s0
        mv   a3, s3
        call line
        mv   a0, s0
        mv   a1, s3
        mv   a2, s0
        mv   a3, s1
        call line

        lw   ra, 16(sp)
        lw   s0, 12(sp)
        lw   s1,  8(sp)
        lw   s2,  4(sp)
        lw   s3,  0(sp)
        addi sp, sp, 20
        ret





# aloc matriz h*w com sbrk, -> ' '
initimg:
        addi sp, sp, -16
        sw   ra, 12(sp)
        sw   s0,  8(sp) # i
        sw   s1,  4(sp) # base ptrs
        sw   s2,  0(sp) # w
        la   t0, w
        lw   s2, 0(t0)
        la   t0, h
        lw   t0, 0(t0)
        slli a0, t0, 2
        li   a7, 9
        ecall
        mv   s1, a0
        la   t0, img
        sw   s1, 0(t0)

        li   s0, 0
        la   t1, h
        lw   t1, 0(t1)
ii1:    bge  s0, t1, ii3
        mv   a0, s2
        li   a7, 9
        ecall
        slli t2, s0, 2
        add  t2, s1, t2
        sw   a0, 0(t2)

        mv   t3, a0
        li   t4, 0
ii2:    bge  t4, s2, ii4
        li   t5, ' '
        add  t6, t3, t4
        sb   t5, 0(t6)
        addi t4, t4, 1
        j    ii2
ii4:    addi s0, s0, 1
        j    ii1
ii3:    lw   ra, 12(sp)
        lw   s0,  8(sp)
        lw   s1,  4(sp)
        lw   s2,  0(sp)
        addi sp, sp, 16
        ret




# ler file (a0=nome) -> parse
readf:
        addi sp, sp, -8
        sw   ra, 4(sp)
        sw   s0, 0(sp)
        li   a1, 0
        li   a7, 1024
        ecall
        mv   s0, a0
        bge  s0, zero, rf1
        la   a0, msgerr
        call pstr
        li   a7, 10
        ecall
rf1:
        la   t0, fd
        sw   s0, 0(t0)

        call readln         # 1a linha = w h
        call pint
        la   t0, w
        sw   a0, 0(t0)
        call pint
        la   t0, h
        sw   a0, 0(t0)
        call initimg


pl:     call readln
        blt  a0, zero, ple  # eof
        call skipsp
        call peekc
        li   t0, -1
        beq  a0, t0, pl     # linha vazia
        li   t0, '#'
        beq  a0, t0, pl     # comentario
        call nextc
        mv   t6, a0
        li   t0, 'p'
        beq  t6, t0, cp
        li   t0, 'l'
        beq  t6, t0, cl
        li   t0, 'r'
        beq  t6, t0, cr
        j    pl

cp:     call pint
        la   t0, a
        sw   a0, 0(t0)
        call pint
        la   t0, b
        sw   a0, 0(t0)
        la   t0, a
        lw   a0, 0(t0)
        la   t0, b
        lw   a1, 0(t0)
        call point
        j    pl



cl:     call pint
        la   t0, a
        sw   a0, 0(t0)
        call pint
        la   t0, b
        sw   a0, 0(t0)
        call pint
        la   t0, c
        sw   a0, 0(t0)
        call pint
        la   t0, d
        sw   a0, 0(t0)
        la   t0, a
        lw   a0, 0(t0)
        la   t0, b
        lw   a1, 0(t0)
        la   t0, c
        lw   a2, 0(t0)
        la   t0, d
        lw   a3, 0(t0)
        call line
        j    pl

cr:     call pint
        la   t0, a
        sw   a0, 0(t0)
        call pint
        la   t0, b
        sw   a0, 0(t0)
        call pint
        la   t0, c
        sw   a0, 0(t0)
        call pint
        la   t0, d
        sw   a0, 0(t0)
        la   t0, a
        lw   a0, 0(t0)
        la   t0, b
        lw   a1, 0(t0)
        la   t0, c
        lw   a2, 0(t0)
        la   t0, d
        lw   a3, 0(t0)
        call rect
        j    pl

ple:    la   t0, fd
        lw   a0, 0(t0)
        li   a7, 57
        ecall
        lw   ra, 4(sp)
        lw   s0, 0(sp)
        addi sp, sp, 8
        ret










# print ascii
prtimg:
        addi sp, sp, -20
        sw   ra, 16(sp)
        sw   s0, 12(sp)
        sw   s1,  8(sp)
        sw   s2,  4(sp)
        sw   s3,  0(sp)
        la   t0, h
        lw   s2, 0(t0)
        la   t0, w
        lw   s3, 0(t0)

        li   s0, 0
po1:    bge  s0, s2, po3
        la   t0, img
        lw   t0, 0(t0)
        slli t1, s0, 2
        add  t0, t0, t1
        lw   t0, 0(t0)
        li   s1, 0
po2:    bge  s1, s3, po4
        add  t1, t0, s1
        lbu  a0, 0(t1)
        li   a7, 11
        ecall
        addi s1, s1, 1
        j    po2
po4:    li   a0, '\n'
        li   a7, 11
        ecall
        addi s0, s0, 1
        j    po1
po3:    lw   ra, 16(sp)
        lw   s0, 12(sp)
        lw   s1,  8(sp)
        lw   s2,  4(sp)
        lw   s3,  0(sp)
        addi sp, sp, 20
        ret




# write a0=nome
save:
        addi sp, sp, -24
        sw   ra, 20(sp)
        sw   s0, 16(sp)
        sw   s1, 12(sp)
        sw   s3,  4(sp)
        sw   s4,  0(sp)
        li   a1, 1
        li   a7, 1024
        ecall
        mv   s0, a0
        la   t0, h
        lw   s3, 0(t0)
        la   t0, w
        lw   s4, 0(t0)

        li   s1, 0
sv1:    bge  s1, s3, sv2
        la   t0, img
        lw   t0, 0(t0)
        slli t1, s1, 2
        add  t0, t0, t1
        lw   t0, 0(t0)
        mv   a0, s0
        mv   a1, t0
        mv   a2, s4
        li   a7, 64
        ecall

        mv   a0, s0
        la   a1, nl
        li   a2, 1
        li   a7, 64
        ecall
        addi s1, s1, 1
        j    sv1
sv2:    mv   a0, s0
        li   a7, 57
        ecall
        lw   ra, 20(sp)
        lw   s0, 16(sp)
        lw   s1, 12(sp)
        lw   s3,  4(sp)
        lw   s4,  0(sp)
        addi sp, sp, 24
        ret








main:
        addi sp, sp, -4
        sw   ra, 0(sp)
        la   a0, msgin
        call pstr

        la   a0, fname
        li   a1, 100
        li   a7, 8
        ecall

        # rm \n de read_string
        la   t0, fname
tn1:    lbu  t1, 0(t0)
        beq  t1, zero, tn3
        li   t2, '\n'
        beq  t1, t2, tn2
        addi t0, t0, 1
        j    tn1
tn2:    sb   zero, 0(t0)
tn3:
        la   a0, fname
        call readf
        call prtimg

        la   a0, oname
        call save
        la   a0, msgok
        call pstr

        lw   ra, 0(sp)
        addi sp, sp, 4
        li   a7, 10
        ecall
