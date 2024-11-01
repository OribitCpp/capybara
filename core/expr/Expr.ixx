export module Expr;

import  <llvm/ADT/APInt.h>;
import <memory>;

//   -1 =  minus expr + constant expr


export template<typename DerivedType, typename BaseType>
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


export enum ExprKind {
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


export class Expr {
public:
    Expr(unsigned int bitNumber,uint64_t value):m_value(bitNumber,value){ s_total++; }
    Expr(const llvm::APInt& value):m_value(value){}
    virtual ~Expr(){s_total--;}

    virtual ExprKind getKind(){return ExprKind::INVALID;}

    static unsigned int total() { return s_total; }
public:
    std::shared_ptr<Expr> leftExpr;
    std::shared_ptr<Expr> rightExpr;
protected:
    llvm::APInt m_value;
private:
    static unsigned int s_total;
};

unsigned int Expr::s_total = 0;
