//
// Created by Michael on 12.05.2024.
//

#include "seepruntime.h"
#include <cassert>
#include <cstring>
#include <functional>

using namespace Seep;

MemRec::MemRec(Type type, const void *val)
    : _type(type)
{
    if (type == Type::String) {
        value._pstr = new std::string();
    }

    if (type == Type::Void || type == Type::Reference) {
        return;
    }
    
    reassign(val);
}

MemRec::MemRec(MemRec &&src) noexcept {
    std::swap(_type, src._type);
    std::swap(value, src.value);
}

MemRec::~MemRec() {
    if (_type == Type::String) {
        delete value._pstr;
    }
}

// Binary Arithmetic //

template<typename BinaryOp>
[[nodiscard]] MemRec binop(const MemRec& lhs, const MemRec& rhs, BinaryOp op, bool to_real = false) {
    // if (lhs.type() != rhs.type()) {
    //     throw std::runtime_error("Incompatible types");
    // }

    if (BelongsToGroup(lhs.type(), TypeGroup::Numeric) && Seep::BelongsToGroup(rhs.type(), TypeGroup::Numeric)) {
        if (to_real) {
            MemRec res(Type::Double);
            res.value._float = op(lhs.value._int, rhs.value._int);
            return res;
        }
        MemRec res(Type::Integer);
        res.value._int = op(lhs.value._int, rhs.value._int);
        return res;
    }

   MemRec res(Type::Double);
   if (BelongsToGroup(lhs.type(), TypeGroup::Real) && Seep::BelongsToGroup(rhs.type(), TypeGroup::Real)) {
       res.value._float = op(lhs.value._float, rhs.value._float);
   }
   else if (BelongsToGroup(lhs.type(), TypeGroup::Numeric) && Seep::BelongsToGroup(rhs.type(), TypeGroup::Real)) {
       res.value._float = op(lhs.value._int, rhs.value._float);
   }
   else if (BelongsToGroup(lhs.type(), TypeGroup::Real) && Seep::BelongsToGroup(rhs.type(), TypeGroup::Numeric)) {
       res.value._float = op(lhs.value._float, rhs.value._int);
   }
   else {
       throw std::runtime_error("TODO");
   }
   return res;
}

double divide_real(int lhs, int rhs) {
    return static_cast<double>(lhs) / rhs;
}

MemRec MemRec::operator+(const MemRec& other) const {
    if (_type == Type::String) {
        MemRec res(_type);
        res.value._pstr = new std::string(*value._pstr + *other.value._pstr);
        return res;
    }
    return binop(*this, other, std::plus<>{});
}

MemRec MemRec::operator-(const MemRec& other) const {
    return binop(*this, other, std::minus<>{});
}

MemRec MemRec::operator*(const MemRec& other) const {
    if (_type == Type::String) {
        throw std::runtime_error("This operation is applicable to numeric values only");
    }
    return binop(*this, other, std::multiplies<>{});
}

MemRec MemRec::operator/(const MemRec& other) const {
    if (_type == Type::String) {
        throw std::runtime_error("This operation is applicable to numeric values only");
    }
    if (other.value._float == 0) {
        throw std::runtime_error("Division by zero");
    }
    return binop(*this, other, divide_real, true);
}

MemRec MemRec::operator%(const MemRec& other) const {
    if (_type == Type::String) {
        throw std::runtime_error("This operation is applicable to integers only");
    }

    MemRec res(_type);

    switch (_type) {
        case Type::Integer: {
            res.value._int = value._int % other.value._int;
            break;
        }
    }

    return res;
}

MemRec MemRec::operator-() const {
    if (_type == Type::String) {
        throw std::runtime_error("This operation is applicable only to numeric values");
    }

    return binop(MemRec(_type, nullptr), *this, std::minus<>{});
}


// Compare //

int MemRec::cmp(const MemRec& other) const noexcept {
    return std::memcmp(&value, &other.value, sizeof(value));
}

template<typename C>
[[nodiscard]] bool xcmp(const MemRec& lhs, const MemRec& rhs, C cfn) noexcept {
    if (lhs.type() != rhs.type()) {
        return false;
    }

    if (lhs.type() == Type::String) {
        return cfn(*lhs.value._pstr, *rhs.value._pstr);
    }

    return cfn(lhs.cmp(rhs), 0);
}

MemRec& MemRec::operator=(MemRec&& other) noexcept {
    std::swap(_type, other._type);
    std::swap(value, other.value);
    return *this;
}

bool MemRec::operator==(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::equal_to{});
}
bool MemRec::operator!=(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::not_equal_to{});
}

bool MemRec::operator<(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::less{});
}
bool MemRec::operator<=(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::less_equal{});
}
bool MemRec::operator>=(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::greater_equal{});
}
bool MemRec::operator>(const MemRec& other) const noexcept {
    return xcmp(*this, other, std::greater{});
}



// Init //

#define MEM_ASSIGN(target, value)                              \
    target = value                                             \
                ? *static_cast<const decltype(target)*>(value) \
                : decltype(target) {}

void MemRec::reassign(const void* val) {
    switch (_type) {
        case Type::Boolean: {
            MEM_ASSIGN(value._bool, val);
            break;
        }
        case Type::Integer: {
            MEM_ASSIGN(value._int, val);
            break;
        }
        case Type::Double:
        case Type::Real: {
            MEM_ASSIGN(value._float, val);
            break;
        }
        case Type::String: {
            if (!val) {
                *value._pstr = "";
            } else {
                *value._pstr = (*static_cast<const std::string*>(val));
            }
            break;
        }
        case Type::Void:
        case Type::Reference: {
            throw std::runtime_error("Can't create memory record of a not stored type");
        }
        default: {
            assert(!"Unhandled type");
        }
    }
}
