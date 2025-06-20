#pragma once

#define __IR_TKNCAT(_a, _b) _a ## _b
#define IR_TKNCAT(_a, _b) __IR_TKNCAT(_a, _b)
#define IR_UNIQUE(_a) __IR_TKNCAT(_a, __COUNT__)
#define IR_UNREACHABLE __builtin_unreachable()
#define IR_UNIMPLEMENTED { do { IR_MSG(FATAL, "NOT IMPLEMENTED - %s:%d in func: %s", __FILE__, __LINE__, __func__); IR_UNREACHABLE; } while(0); }
#define IR_ARRSTRIDE(_arr) sizeof((_arr)[0])
#define IR_ARRLEN(_arr) (sizeof(_arr) / IR_ARRSTRIDE(_arr))
#define IR_RETURN(_val) { return _val; }