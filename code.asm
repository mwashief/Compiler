.model small
.stack 100h
.data
t1 dw 1 dup(?)
t2 dw 1 dup(?)
t3 dw 1 dup(?)
t4 dw 1 dup(?)
t5 dw 1 dup(?)
t6 dw 1 dup(?)
t7 dw 1 dup(?)
t8 dw 1 dup(?)
t9 dw 1 dup(?)
t10 dw 1 dup(?)
t11 dw 1 dup(?)
t12 dw 1 dup(?)
t13 dw 1 dup(?)
.code
main proc
mov ax, @data
mov ds, ax
mov t11, 5
mov bx, t11
mov t12, bx
mov t1, bx
call fact
mov t13, dx
mov bx, t13
mov t10, bx
mov ax, t10
call println
mov ah, 4ch
int 21h
main endp
println proc
push ax
push bx
push cx
push dx
or ax, ax
jge @end_if1
push ax
mov dl, '-'
mov ah, 2
int 21h
pop ax
neg ax
@end_if1:
xor cx, cx
mov bx, 10D
@repeat1:
xor dx,dx
div bx
push dx
inc cx
or ax, ax
jne @repeat1
mov ah, 2
@print_loop:
pop dx
or dl, 30h
int 21h
loop @print_loop
mov dl, 0ah
int 21h
mov dl, 0dh
int 21h
pop dx
pop cx
pop bx
pop ax
ret
println endp
fact proc
mov t2, 1
mov t3, 0
mov bx, t1
cmp bx, t2
jg L1
mov t3, 1
L1:
cmp t3, 0
je L2
mov t4, 1
mov dx, t4
ret
L2:
push t1
mov t5, 1
mov bx, t1
mov cx, t5
sub bx, cx
mov t6, bx
mov t7, bx
mov t1, bx
call fact
mov t8, dx
pop t1
mov ax, t1
mov bx, t8
imul bx
mov t9, ax
mov dx, t9
ret
fact endp
end main
