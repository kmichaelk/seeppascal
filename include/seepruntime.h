//
// Created by Michael on 11.05.2024.
//

#ifndef SEEP_RUNTIME_H
#define SEEP_RUNTIME_H

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cassert>

namespace Seep {
    class Runtime;
    enum class Type : unsigned char;
    enum class TypeGroup : unsigned char;
    class MemRec;
    struct Context;
    class Debugger;
    class ParserError;
}

class Seep::Runtime {
public:
    using ArgV = std::vector<MemRec*>;
    using Procedure = std::function<void(Runtime& rt, MemRec& out, const ArgV& args)>;
    // using FnHandler = void(*)(Runtime* rt, void* out, const ArgV& args);
    // using FnHandler = void(*)(Runtime* rt, void* out, ...);
private:
    struct Internals;
    Internals* internals;

    const std::string source;

    Debugger* debugger;
public:
    std::istream& in;
    std::ostream& out;

    explicit Runtime(std::string source, std::ostream& out = std::cout, std::istream& in = std::cin);

    ~Runtime();

    Runtime(const Runtime& other) = delete;
    Runtime& operator=(const Runtime& other) = delete;

    void bind_procedure(const std::string& name, const std::vector<TypeGroup>& arg_types, Type ret_type, Procedure pfn);

    void attach_debugger(Debugger* _debugger);

    void warm_up();
    void execute();

    void panic(const std::string& cause);
};

enum class Seep::Type : unsigned char {
    Void,
    Reference,

    Boolean,        // 1B ~ bool

    Char,           // 1B ~ char

    ShortInt,       // 1B ~ char
    Byte,           // 1B ~ unsigned char
    Integer,        // 2B ~ short
    Word,           // 2B ~ unsigned short
    LongInt,        // 4B ~ int

    Real,           // 6B
    Single,         // 4B ~ float
    Double,         // 8B ~ double
    Extended,       // 10B

    String
};

enum class Seep::TypeGroup : unsigned char {
    Any,
    Reference,
    Variadic,

    Numeric,
    Real,

    String
};

namespace Seep {
    bool BelongsToGroup(Type type, TypeGroup group) noexcept;
}

class Seep::MemRec {
    Type _type = Type::Void;
public:
    MemRec() = default;
    explicit MemRec(Type type);
    explicit MemRec(Type type, const void* val);

    union {
        int _int;
        double _float;
        std::string* _pstr;
    } value;

    MemRec(const MemRec& src) : _type(src._type), value(src.value) {
        if (_type == Type::String) {
            value._pstr = new std::string(*value._pstr);
        } // todo
    };
    MemRec(MemRec&& src) noexcept;
    ~MemRec();

    [[nodiscard]] Type type() const { return _type; };

    void reassign(const void* val);

    [[nodiscard]] int cmp(const MemRec& other) const noexcept;

    MemRec operator+(const MemRec& other) const;
    MemRec operator-(const MemRec& other) const;
    MemRec operator*(const MemRec& other) const;
    MemRec operator/(const MemRec& other) const;
    MemRec operator%(const MemRec& other) const;

    MemRec operator-() const;

    MemRec& operator=(const MemRec& other) = delete;
    MemRec& operator=(MemRec&& other) noexcept;

    bool operator==(const MemRec& other) const noexcept;
    bool operator!=(const MemRec& other) const noexcept;

    bool operator< (const MemRec& other) const noexcept;
    bool operator<=(const MemRec& other) const noexcept;
    bool operator>=(const MemRec& other) const noexcept;
    bool operator> (const MemRec& other) const noexcept;
};

struct Seep::Context {
    struct State;
    State* state;
    explicit Context(State* state);

    [[nodiscard]] Runtime& get_runtime() const;

    [[nodiscard]] const std::string& get_program_name() const;

    [[nodiscard]] std::unordered_map<std::string, MemRec>::iterator vars_begin() const;
    [[nodiscard]] std::unordered_map<std::string, MemRec>::iterator vars_end() const;
};

class Seep::Debugger {
public:

    virtual ~Debugger() = default;

    virtual void onAttach(Context& ctx) = 0;
    virtual void onFinish(Context& ctx) = 0;
};

class Seep::ParserError : public std::runtime_error {
public:
    explicit ParserError(const std::string& message)
        : std::runtime_error(message)
    {}
};

#endif // SEEP_RUNTIME_H
