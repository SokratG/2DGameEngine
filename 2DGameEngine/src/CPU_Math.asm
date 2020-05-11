.data
result REAL4 0.0
_temp REAL4 0.0
base2 REAL4 2.0

.code
PUBLIC sqrt__cpu
PUBLIC sin__cpu
PUBLIC cos__cpu
PUBLIC tan__cpu
PUBLIC atan__cpu
PUBLIC atan2__cpu
PUBLIC log2__cpu
PUBLIC log10__cpu
PUBLIC ln__cpu

sqrt__cpu PROC
     MOVD result, XMM0
     FLD result
     FSQRT
     FST result
     MOVD XMM0, result
     RET
sqrt__cpu ENDP


sin__cpu PROC
     MOVD result, XMM0
     FLD result
     FSIN
     FST result
     MOVD XMM0, result
     RET
sin__cpu ENDP

cos__cpu PROC
     MOVD result, XMM0
     FLD result
     FCOS
     FST result
     MOVD XMM0, result
     RET
cos__cpu ENDP

tan__cpu PROC
     MOVD result, XMM0
     FLD result
     FPTAN
     FXCH ST(1)
     FST result
     MOVD XMM0, result
     RET
tan__cpu ENDP


atan__cpu PROC
     MOVD result, XMM0
     MOVD _temp, XMM1
     FLD _temp
     FLD result
     FDIV
     FLD _temp
     FXCH ST(1)
     FPATAN
     FST result
     MOVD XMM0, result
     RET
atan__cpu ENDP



atan2__cpu PROC
     MOVD result, XMM0
     MOVD _temp, XMM1
     FLD result
     FLD _temp
     FPATAN
     FST result
     MOVD XMM0, result
     RET
atan2__cpu ENDP


log2__cpu PROC
     MOVD result, XMM0
     FLD base2
     FLD result
     FYL2X
     FDIV base2
     FST result
     MOVD XMM0, result
     RET
log2__cpu ENDP


log10__cpu PROC
     MOVD result, XMM0
     fldlg2
     FLD result
     FYL2X
     FST result
     MOVD XMM0, result
     RET
log10__cpu ENDP


ln__cpu PROC
     MOVD result, XMM0
     fldln2 
     FLD result
     FYL2X
     FST result
     MOVD XMM0, result
     RET
ln__cpu ENDP


END