#pragma once

//其实可以用USES_CONVERSION加T2W,W2T来代替，但是用的是栈的内存，现在这个方法用堆内存转换，除此之外实现是一样的

CStringA CStrT2CStrA(const CString &CStrT);
CStringW CStrT2CStrW(const CString &CStrT);
CString CStrA2CStrT(const CStringA &CStrA);
CString CStrW2CStrT(const CStringW &CStrW);
CStringA CStrW2CStrA(const CStringW &CStrW);
CStringW CStrA2CStrW(const CStringA &CStrA);