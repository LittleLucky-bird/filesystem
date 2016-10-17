

#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

enum AttrType {
    MyINT,
    FLOAT,
    STRING
};

enum AggeType {
    NONE,
    SUM,
    AVG,
    MIN,
    MAX
};

enum CompOp {
    EQ_OP,
    LT_OP,
    GT_OP,
    LE_OP,
    GE_OP,
    NE_OP,
    LIKE_OP,
    NULL_OP,
    NO_OP
};


#endif /* COMMON_COMMON_H_ */
