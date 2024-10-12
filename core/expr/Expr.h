#ifndef  EXPR_H
#define EXPR_H


template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
    static int AnyFunction(BaseType* base)
    {
        return 1;
    }

    static  char AnyFunction(void* t2)
    {
        return 0;
    }

    enum
    {
        Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
    };
};


enum OperandWidth
{
	INVALID = 0,
	BOOL = 1,
	INT8 =8,
	INT16 =16,
	INT32 = 32,
	INT64 =64,
	FL80 = 80,
	INT128 = 128,
	INT256=256,
	INT512=512,
	MAX = INT512
};

enum OperationKind {
    INVALID = -1,
    CONSTANT = 0,

    NOTOPTIMIZED,

    READ,
    SELECT,
    CONCAT,
    EXTRACT,

    ZEXT,
    SEXT,


    // ARITHMETIC
    ADD,
    SUB,
    MUL,
    UDIV,
    SDIV,
    UREM,
    SREM,

    // BIT
    AND,
    NOT,
    OR,
    XOR,
    SHL,
    LSHR,
    ASHR,


    // COMPARE
    EQ,
    NE,  ///< NOT USED IN CANONICAL FORM
    ULT,
    ULE,
    UGT, ///< NOT USED IN CANONICAL FORM
    UGE, ///< NOT USED IN CANONICAL FORM
    SLT,
    SLE,
    SGT, ///< NOT USED IN CANONICAL FORM
    SGE, ///< NOT USED IN CANONICAL FORM

    LAST_KIND = SGE,

    CAST_KIND_FIRST = ZEXT,
    CAST_KIND_LAST = SEXT,
    BINARY_KIND_FIRST = ADD,
    BINARY_KIND_LAST = SGE,
    CMP_KIND_FIRST = EQ,
    CMP_KIND_LAST = SGE
};


class Expr {
public:
    Expr();
    virtual ~Expr();

    virtual OperandWidth getWidth() = 0;
    virtual OperationKind getKind() = 0;

    unsigned int hash() { return m_hashValue; }

    virtual bool isConstantOne() { return false; }
    virtual bool isConstantZero() { return false; }
    virtual bool isTrue() { return false; }
    virtual bool isFalse() { return false; }
protected:
    unsigned int m_hashValue = 0;

private:
    static unsigned int s_total;
};


#endif // ! EXPR_H
