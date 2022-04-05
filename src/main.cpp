#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <cstddef>

template<class T>
class Pointer {
private:
  T* m_ptr;
  void incref() { if (m_ptr != nullptr) m_ptr->m_ref--; }
  void decref() { if (m_ptr != nullptr && --(m_ptr->m_ref) == 0) { delete m_ptr; m_ptr = nullptr; } }
  template<class U>
  friend class Pointer;
public:
  Pointer() : m_ptr(nullptr) {}
  Pointer(T* ptr) : m_ptr(ptr) { incref(); }
  Pointer(const Pointer& other) : m_ptr(other.m_ptr) { incref(); }
  template<class U>
  Pointer(const Pointer<U>& other) : m_ptr(other.m_ptr) { incref(); }
  Pointer(Pointer&& other) : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
  template<class U>
  Pointer(Pointer<U>&& other) : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
  Pointer& operator=(const Pointer& other) { Pointer(other).swap(*this); return *this; }
  template<class U>
  Pointer& operator=(const Pointer<U>& other) { Pointer(other).swap(*this); return *this; }
  Pointer& operator=(Pointer&& other) { Pointer(std::move(other)).swap(*this); return *this; }
  template<class U>
  Pointer& operator=(Pointer<U>&& other) { Pointer(std::move(other)).swap(*this); return *this; }
  ~Pointer() { decref(); }
  operator bool() const { return m_ptr != nullptr; }
  T& operator*() const { return *m_ptr; }
  T* operator->() const { return m_ptr; }
  T* get() const { return m_ptr; }
  void swap(Pointer& other) { std::swap(m_ptr, other.m_ptr); }
};

namespace std {

  template<class T>
  void swap(Pointer<T>& left, Pointer<T>& right) noexcept {
    left.swap(right);
  }
  
}

class Object {
public:
  Object() = default;
  Object(const Object&) = delete;
  Object(Object&&) = delete;
  Object& operator=(const Object&) = delete;
  Object& operator=(Object&&) = delete;
  virtual ~Object() = default;
  virtual Pointer<Object> add(const Object*) const { throw std::runtime_error("'add' N/I"); }
  virtual Pointer<Object> sub(const Object*) const { throw std::runtime_error("'sub' N/I"); }
  virtual Pointer<Object> mul(const Object*) const { throw std::runtime_error("'mul' N/I"); }
  virtual bool is_true() const { return true; }
  virtual void print(std::ostream&) const { throw std::runtime_error("'print' N/I"); }
  virtual int id() const = 0;
private:
  unsigned int m_ref = 0;
  template <class T> friend class Pointer;
};

#define CODE_ID 0
#define INTEGER_ID 1
#define FUNCTION_ID 2

enum {
  INVOKE, // args_count
  INVOKE_SELF, // args_count
  TAIL, // args_count
  TAIL_SELF, // args_count
  RETURN_VALUE,
  RETURN,
  JUMP_IF, // offset
  JUMP, // offset
  PUSH_LOCAL, // ilocal
  PUSH_CONST, // iconst
  PUSH_CAPTURE, // icapture
  POP_LOCAL, // ilocal
  ADD,
  SUB,
  MUL,
  PRINT,
  MAKE_FUNCTION // args_count, captures_count
};

class Code : public Object {
public:
  using Unit = unsigned int;
  std::vector<Unit> m_units;
  Code() = default;
  ~Code() = default;
  const Unit* data() const { return m_units.data(); }
  void append(Unit unit) { m_units.push_back(unit); }
  size_t size() const { return m_units.size(); }
  void set_unit(size_t index, Unit unit) { m_units[index] = unit; }
  void print(std::ostream& os) const override {
    os << "Code:\n";
    std::string prefix("\t");
    const Unit* cur = m_units.data();
    const Unit* begin = cur;
    const Unit* end = cur + m_units.size();
    while (cur != end) {
      os << (cur - begin);
      switch (*cur++) {
        case INVOKE: {
          size_t args_count = *cur++;
          os << prefix << "INVOKE: args_count = " << args_count << std::endl;
          break;
        }
        case INVOKE_SELF: {
          size_t args_count = *cur++;
          os << prefix << "INVOKE_SELF: args_count = " << args_count << std::endl;
          break;
        }
        case TAIL: {
          size_t args_count = *cur++;
          os << prefix << "TAIL: args_count = " << args_count << std::endl;
          break;
        }
        case TAIL_SELF: {
          size_t args_count = *cur++;
          os << prefix << "TAIL_SELF: args_count = " << args_count << std::endl;
          break;
        }
        case RETURN_VALUE: {
          os << prefix << "RETURN_VALUE\n";
          break;
        }
        case RETURN: {
          os << prefix << "RETURN\n";
          break;
        }
        case JUMP_IF: {
          size_t offset = *cur++;
          os << prefix << "JUMP_IF: offset = " << offset << std::endl;
          break;
        }
        case JUMP: {
          size_t offset = *cur++;
          os << prefix << "JUMP: offset = " << offset << std::endl;
          break;
        }
        case PUSH_LOCAL: {
          int ilocal = *cur++;
          os << prefix << "PUSH_LOCAL: ilocal = " << ilocal << '\n';
          break;
        }
        case PUSH_CONST: {
          int iconst = *cur++;
          os << prefix << "PUSH_CONST: iconst = " << iconst << '\n';
          break;
        }
        case PUSH_CAPTURE: {
          int icapture = *cur++;
          os << prefix << "PUSH_CAPTURE: icapture = " << icapture << '\n';
          break;
        }
        case POP_LOCAL: {
          size_t ilocal = *cur++;
          os << prefix << "POP_LOCAL: ilocal = " << ilocal << std::endl;
          break;
        }
        case ADD: {
          os << prefix << "ADD\n";
          break;
        }
        case SUB: {
          os << prefix << "SUB\n";
          break;
        }
        case MUL: {
          os << prefix << "MUL\n";
          break;
        }
        case PRINT: {
          os << prefix << "PRINT\n";
          break;
        }
        case MAKE_FUNCTION: {
          size_t args_count = *cur++;
          size_t captures_count = *cur++;
          os << prefix << "MAKE_FUNCTION: args_count = " << args_count << ", captures_count = " << captures_count << std::endl;
          break;
        }
        default:
          break;
      }
    }
  }
  int id() const override { return CODE_ID; }
};

class Integer : public Object {
public:
  int m_data;
  Integer(int data) : Object(), m_data(data) {}
  ~Integer() = default;
  Pointer<Object> add(const Object* obj) const override {
    if (obj->id() != INTEGER_ID) throw std::runtime_error("'integer.add' N/I for given type");
    const Integer* other = static_cast<const Integer*>(obj);
    return Pointer<Object>(new Integer(this->m_data + other->m_data));
  }
  Pointer<Object> sub(const Object* obj) const override {
    if (obj->id() != INTEGER_ID) throw std::runtime_error("'integer.sub' N/I for given type");
    const Integer* other = static_cast<const Integer*>(obj);
    return Pointer<Object>(new Integer(this->m_data - other->m_data));
  }
  Pointer<Object> mul(const Object* obj) const override {
    if (obj->id() != INTEGER_ID) throw std::runtime_error("'integer.mul' N/I for given type");
    const Integer* other = static_cast<const Integer*>(obj);
    return Pointer<Object>(new Integer(this->m_data * other->m_data));
  }
  bool is_true() const override { return m_data != 0; }
  void print(std::ostream& os) const override { os << "Integer: " << m_data << std::endl; }
  int id() const override { return INTEGER_ID; }
};

class Function : public Object {
public:
  std::vector< Pointer<Object> > m_captures;
  Pointer<Code> m_code;
  size_t m_args_count;
  Function(std::vector< Pointer<Object> >&& captures, Pointer<Code>&& code, size_t args_count) : Object(), m_captures(std::move(captures)), m_code(std::move(code)), m_args_count(args_count) {}
  ~Function() = default;
  void print(std::ostream&) const override { throw std::runtime_error("'function.print' N/I"); }
  int id() const override { return FUNCTION_ID; }
};

#define MAX_DEPTH 1000

class Evaler {
private:
  std::vector< Pointer<Object> > m_consts;
  struct Frame {
    std::vector< Pointer<Object> > m_locals;
    Pointer<Function> m_function;
    const Code::Unit* m_current;
    Frame* m_back;
  } *m_frame;
  size_t m_depth = 0;
public:
  Evaler() = default;
  ~Evaler() {
    while (m_frame != nullptr) {
      Frame* back = m_frame->m_back;
      delete m_frame;
      m_frame = back;
    }
  }
  void init(Pointer<Function>&& function, std::vector< Pointer<Object> >&& consts) {
    m_consts = std::move(consts);
    m_frame = new Frame();
    m_frame->m_function = std::move(function);
    m_frame->m_current = m_frame->m_function->m_code->data();
    m_frame->m_back = nullptr;
  }
  void invoke() {
    if (m_depth == MAX_DEPTH) throw std::runtime_error("INVOKE: stack overflow");
    size_t args_count = *m_frame->m_current++;
    auto& locals = m_frame->m_locals;
    if (args_count +  1 > locals.size()) throw std::runtime_error("INVOKE: args_count + 1 > locals.size");
    std::vector< Pointer<Object> > new_locals{};
    for (size_t i = 0; i < args_count; ++i) {
      new_locals.push_back(std::move(locals.back()));
      locals.pop_back();
    }
    Pointer<Object> object = std::move(locals.back()); locals.pop_back();
    if (object->id() != FUNCTION_ID) throw std::runtime_error("INVOKE: wrong function type");
    Pointer<Function> function{static_cast<Function*>(object.get())};
    if (args_count != function->m_args_count) throw std::runtime_error("INVOKE: args_count != function.args_count");
    Frame* frame = new Frame();
    frame->m_locals = std::move(new_locals);
    frame->m_function = std::move(function);
    frame->m_current = frame->m_function->m_code->data();
    frame->m_back = m_frame;
    m_frame = frame;
    m_depth++;
  }
  void invoke_self() {
    if (m_depth == MAX_DEPTH) throw std::runtime_error("INVOKE_SELF: stack overflow");
    size_t args_count = *m_frame->m_current++;
    if (args_count != m_frame->m_function->m_args_count) throw std::runtime_error("INVOKE_SELF: args_count != function.args_count");
    auto& locals = m_frame->m_locals;
    if (args_count > locals.size()) throw std::runtime_error("INVOKE_SELF: args_count > locals.size");
    std::vector< Pointer<Object> > new_locals{};
    for (size_t i = 0; i < args_count; ++i) {
      new_locals.push_back(std::move(locals.back()));
      locals.pop_back();
    }
    Frame* frame = new Frame();
    frame->m_locals = std::move(new_locals);
    frame->m_function = m_frame->m_function;
    frame->m_current = frame->m_function->m_code->data();
    frame->m_back = m_frame;
    m_frame = frame;
    m_depth++;
  }
  void tail() {
    size_t args_count = *m_frame->m_current++;
    auto& locals = m_frame->m_locals;
    if (args_count +  1 > locals.size()) throw std::runtime_error("TAIL: args_count + 1 > locals.size");
    std::vector< Pointer<Object> > new_locals{};
    for (size_t i = 0; i < args_count; ++i) {
      new_locals.push_back(std::move(locals.back()));
      locals.pop_back();
    }
    Pointer<Object> object = std::move(locals.back()); locals.pop_back();
    if (object->id() != FUNCTION_ID) throw std::runtime_error("TAIL: wrong function type");
    Pointer<Function> function{static_cast<Function*>(object.get())};
    if (args_count != function->m_args_count) throw std::runtime_error("TAIL: args_count != function.args_count");
    m_frame->m_locals = std::move(new_locals);
    m_frame->m_function = std::move(function);
    m_frame->m_current = m_frame->m_function->m_code->data();
  }
  void tail_self() {
    size_t args_count = *m_frame->m_current++;
    if (args_count != m_frame->m_function->m_args_count) throw std::runtime_error("TAIL_SELF: args_count != function.args_count");
    auto& locals = m_frame->m_locals;
    if (args_count > locals.size()) throw std::runtime_error("TAIL_SELF: args_count > locals.size");
    std::vector< Pointer<Object> > new_locals{};
    for (size_t i = 0; i < args_count; ++i) {
      new_locals.push_back(std::move(locals.back()));
      locals.pop_back();
    }
    m_frame->m_locals = std::move(new_locals);
    m_frame->m_current = m_frame->m_function->m_code->data();
  }
  void return_value() {
    auto& locals = m_frame->m_locals;
    if (locals.empty()) throw std::runtime_error("RETURN_VALUE: locals.empty");
    auto result = std::move(locals.back());
    Frame* back = m_frame->m_back;
    delete m_frame;
    m_frame = back;
    m_frame->m_locals.push_back(std::move(result));
    --m_depth;
  }
  void ret() {
    Frame* back = m_frame->m_back;
    delete m_frame;
    m_frame = back;
  }
  void jump_if() {
    size_t offset = *m_frame->m_current++;
    Code* code = m_frame->m_function->m_code.get();
    if (offset >= code->size()) throw std::runtime_error("JUMP_IF: offset >= code.size");
    auto& locals = m_frame->m_locals;
    if (locals.empty()) throw std::runtime_error("JUMP_IF: locals.empty");
    auto obj = std::move(locals.back()); locals.pop_back();
    if (obj->is_true()) {
      m_frame->m_current = code->data() + offset;
    }
  }
  void jump() {
    size_t offset = *m_frame->m_current++;
    Code* code = m_frame->m_function->m_code.get();
    if (offset >= code->size()) throw std::runtime_error("JUMP: offset >= code.size");
    m_frame->m_current = code->data() + offset;
  }
  void push_local() {
    size_t ilocal = *m_frame->m_current++;
    auto& locals = m_frame->m_locals;
    if (ilocal >= locals.size()) throw std::runtime_error("PUSH_LOCAL: ilocal >= locals.size");
    locals.push_back(locals[ilocal]);
  }
  void push_const() {
    size_t iconst = *m_frame->m_current++;
    if (iconst >= m_consts.size()) throw std::runtime_error("PUSH_CONST: iconst >= consts.size");
    m_frame->m_locals.push_back(m_consts[iconst]);
  }
  void push_capture() {
    size_t icapture = *m_frame->m_current++;
    auto& captures = m_frame->m_function->m_captures;
    if (icapture >= captures.size()) throw std::runtime_error("PUSH_CAPTURE: icapture >= captures.size");
    m_frame->m_locals.push_back(captures[icapture]);
  }
  void pop_local() {
    size_t ilocal = *m_frame->m_current++;
    auto& locals = m_frame->m_locals;
    if (ilocal == locals.size() - 1) return;
    if (ilocal >= locals.size()) throw std::runtime_error("POP_LOCAL: ilocal >= locals.size");
    auto obj = std::move(locals.back()); locals.pop_back();
    if (ilocal >= locals.size()) std::runtime_error("POP_LOCAL: ilocal >= locals.size");
    locals[ilocal] = std::move(obj);
  }
  void add() {
    auto& locals = m_frame->m_locals;
    if (2 > locals.size()) throw std::runtime_error("ADD: 2 > locals.size");
    auto right = std::move(locals.back()); locals.pop_back();
    auto left = std::move(locals.back()); locals.pop_back();
    locals.push_back(left->add(right.get()));
  }
  void sub() {
    auto& locals = m_frame->m_locals;
    if (2 > locals.size()) throw std::runtime_error("SUB: 2 > locals.size");
    auto right = std::move(locals.back()); locals.pop_back();
    auto left = std::move(locals.back()); locals.pop_back();
    locals.push_back(left->sub(right.get()));
  }
  void mul() {
    auto& locals = m_frame->m_locals;
    if (2 > locals.size()) throw std::runtime_error("MUL: 2 > locals.size");
    auto right = std::move(locals.back()); locals.pop_back();
    auto left = std::move(locals.back()); locals.pop_back();
    locals.push_back(left->mul(right.get()));
  }
  void print() {
    auto& locals = m_frame->m_locals;
    if (locals.empty()) throw std::runtime_error("PRINT: locals.empty");
    auto result = std::move(locals.back()); locals.pop_back();
    result->print(std::cout);
  }
  void make_function() {
    size_t args_count = *m_frame->m_current++;
    size_t captures_count = *m_frame->m_current++;
    auto& locals = m_frame->m_locals;
    if (captures_count + 1 > locals.size()) throw std::runtime_error("MAKE_FUNCTION: captures_count + 1 >= locals.size");
    std::vector< Pointer<Object> > captures{};
    for (size_t i = 0; i < captures_count; ++i) {
      auto capture = std::move(locals.back()); locals.pop_back();
      captures.push_back(std::move(capture));
    }
    auto code = std::move(locals.back()); locals.pop_back();
    if (code->id() != CODE_ID) throw std::runtime_error("MAKE_FUNCTION: code has wrong type");
    auto function = Pointer<Function>{new Function(std::move(captures), Pointer<Code>{static_cast<Code*>(code.get())}, args_count)};
    locals.push_back(std::move(function));
  }
  void eval() {
    while (m_frame != nullptr) {
      switch (*m_frame->m_current++) {
        case INVOKE: invoke(); break;
        case INVOKE_SELF: invoke_self(); break;
        case TAIL: tail(); break;
        case TAIL_SELF: tail_self(); break;
        case RETURN_VALUE: return_value(); break;
        case RETURN: ret(); break;
        case JUMP_IF: jump_if(); break;
        case JUMP: jump(); break;
        case PUSH_LOCAL: push_local(); break;
        case PUSH_CONST: push_const(); break;
        case PUSH_CAPTURE: push_capture(); break;
        case POP_LOCAL: pop_local(); break;
        case ADD: add(); break;
        case SUB: sub(); break;
        case MUL: mul(); break;
        case PRINT: print(); break;
        case MAKE_FUNCTION: make_function(); break;
        default: throw std::runtime_error("Evaler error: unknown opcode");
      }
    }
  }
};

namespace AST {

#define DECLVISIT(T) virtual void visit(const T&) = 0;
#define DEFACCEPT void accept(Visitor& visitor) const override { visitor.visit(*this); }

  struct Literal;
  struct Name;
  struct BinOp;
  struct Value;
  struct SelfValue;
  struct IfThenElse;
  struct Lambda;
  struct Assign;
  struct Print;

  class Visitor {
  public:
    Visitor() = default;
    Visitor(const Visitor&) = delete;
    Visitor(Visitor&&) = delete;
    Visitor& operator=(const Visitor&) = delete;
    Visitor& operator=(Visitor&&) = delete;
    virtual ~Visitor() = default;
    DECLVISIT(Literal)
    DECLVISIT(Name)
    DECLVISIT(BinOp)
    DECLVISIT(Value)
    DECLVISIT(SelfValue)
    DECLVISIT(IfThenElse)
    DECLVISIT(Lambda)
    DECLVISIT(Assign)
    DECLVISIT(Print)
  };

  struct Node {
    Node() = default;
    Node(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = delete;
    virtual ~Node() = default;
    virtual void accept(Visitor&) const = 0;
  };

  struct Expr : Node {};

  struct Literal : Expr {
    Literal(Pointer<Object>&& object) : Expr(), m_object(std::move(object)) {}
    ~Literal() = default;
    Pointer<Object> m_object;
    DEFACCEPT
  };

  struct Name : Expr {
    std::string m_data;
    Name(const std::string& data) : Expr(), m_data(data) {}
    Name(std::string&& data) : Expr(), m_data(std::move(data)) {}
    ~Name() =  default;
    DEFACCEPT
  };

  struct BinOp : Expr {
    enum Kind { ADD, SUB, MUL };
    BinOp(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right, Kind kind) : Expr(), m_left(std::move(left)), m_right(std::move(right)), m_kind(kind) {}
    ~BinOp() = default;
    std::unique_ptr<Expr> m_left, m_right;
    Kind m_kind;
    DEFACCEPT
  };

  struct Value : Expr {
    Value(std::unique_ptr<Expr>&& func, std::vector< std::unique_ptr<Expr> >&& argvec) : Expr(), m_argvec(std::move(argvec)), m_func(std::move(func)) {}
    ~Value() = default;
    std::vector< std::unique_ptr<Expr> > m_argvec;
    std::unique_ptr<Expr> m_func;
    DEFACCEPT
  };

  struct SelfValue : Expr {
    SelfValue(std::vector< std::unique_ptr<Expr> >&& argvec) : Expr(), m_argvec(std::move(argvec)) {}
    ~SelfValue() = default;
    std::vector< std::unique_ptr<Expr> > m_argvec;
    DEFACCEPT
  };

  struct IfThenElse : Expr {
    IfThenElse(std::unique_ptr<Expr>&& cond, std::unique_ptr<Expr>&& then, std::unique_ptr<Expr>&& otherwise) :
      Expr(), m_cond(std::move(cond)), m_then(std::move(then)), m_otherwise(std::move(otherwise)) {}
    ~IfThenElse() = default;
    std::unique_ptr<Expr> m_cond, m_then, m_otherwise;
    DEFACCEPT
  };

  struct Lambda : Expr {
    Lambda(std::vector<std::string>&& params, std::unique_ptr<Expr>&& body) : Expr(), m_params(std::move(params)), m_body(std::move(body)) {}
    ~Lambda() = default;
    std::vector<std::string> m_params;
    std::unique_ptr<Expr> m_body;
    DEFACCEPT
  };

  struct Stmt : Node {};

  struct Assign : Stmt {
    Assign(std::string&& name, std::unique_ptr<Expr>&& expr) : Stmt(), m_name(std::move(name)), m_expr(std::move(expr)) {}
    ~Assign() = default;
    std::string m_name;
    std::unique_ptr<Expr> m_expr;
    DEFACCEPT
  };

  struct Print : Stmt {
    Print(std::unique_ptr<Expr>&& expr) : Stmt(), m_expr(std::move(expr)) {}
    ~Print() = default;
    std::unique_ptr<Expr> m_expr;
    DEFACCEPT
  };
  
}

class Compiler : public AST::Visitor {
private:
  using Table = std::unordered_map<std::string, size_t>;
  Table m_locals_table;
  std::vector<std::string> m_captures;
  std::vector< Pointer<Object> > m_consts;
  Pointer<Code> m_code;
  bool m_tail;
public:
  Compiler() : AST::Visitor(), m_locals_table(), m_captures(), m_consts(), m_code(Pointer<Code>{new Code()}) {}
  ~Compiler() = default;
  void visit(const AST::Literal& literal) override {
    size_t iconst = m_consts.size();
    m_consts.push_back(literal.m_object);
    m_code->append(PUSH_CONST);
    m_code->append(iconst);
    if (m_tail) m_code->append(RETURN_VALUE);
  }
  void visit(const AST::Name& name) override {
    auto iter = m_locals_table.find(name.m_data);
    if (iter != m_locals_table.end()) {
      size_t ilocal = iter->second;
      m_code->append(PUSH_LOCAL);
      m_code->append(ilocal);
    } else {
      size_t icapture = 0;
      for (; icapture != m_captures.size(); ++icapture) {
        if (m_captures[icapture] == name.m_data) break;
      }
      if (icapture == m_captures.size()) {
        m_captures.push_back(name.m_data);
      }
      m_code->append(PUSH_CAPTURE);
      m_code->append(icapture);
    }
    if (m_tail) m_code->append(RETURN_VALUE);
  }
  void visit(const AST::BinOp& binop) override {
    bool tail = false;
    std::swap(m_tail, tail);
    binop.m_left->accept(*this);
    binop.m_right->accept(*this);
    std::swap(m_tail, tail);
    switch (binop.m_kind) {
      case AST::BinOp::ADD:
        m_code->append(ADD);
        break;
      case AST::BinOp::SUB:
        m_code->append(SUB);
        break;
      case AST::BinOp::MUL:
        m_code->append(MUL);
        break;
      default:
        break;
    }
    if (m_tail) m_code->append(RETURN_VALUE);
  }
  void visit(const AST::Value& value) override {
    bool tail = false;
    std::swap(m_tail, tail);
    value.m_func->accept(*this);
    for (auto iter = value.m_argvec.rbegin(); iter != value.m_argvec.rend(); ++iter) (*iter)->accept(*this);
    std::swap(m_tail, tail);
    if (m_tail) m_code->append(TAIL);
    else m_code->append(INVOKE);
    m_code->append(value.m_argvec.size());
  }
  void visit(const AST::SelfValue& selfvalue) override {
    bool tail = false;
    std::swap(m_tail, tail);
    for (auto iter = selfvalue.m_argvec.rbegin(); iter != selfvalue.m_argvec.rend(); ++iter) (*iter)->accept(*this);
    std::swap(m_tail, tail);
    if (m_tail) m_code->append(TAIL_SELF);
    else m_code->append(INVOKE_SELF);
    m_code->append(selfvalue.m_argvec.size());
  }
  void visit(const AST::IfThenElse& ite) {
    if (m_tail) {
      /*  <cond>  */
      m_tail = false;
      ite.m_cond->accept(*this);
      m_tail = true;
      /*  JUMP_IF then_offset  */
      m_code->append(JUMP_IF);
      size_t then_offset_index = m_code->size();
      m_code->append(0); /* mock for then_offset */
      /*  <otherwise>  */
      ite.m_otherwise->accept(*this);
      /*  <then>  */
      size_t then_offset = m_code->size();
      ite.m_then->accept(*this);
      /*  set actuals offset  */
      m_code->set_unit(then_offset_index, then_offset);
    } else {
      /*  <cond>  */
      ite.m_cond->accept(*this);
      /*  JUMP_IF then_offset  */
      m_code->append(JUMP_IF);
      size_t then_offset_index = m_code->size();
      m_code->append(0); /* mock for then_offset */
      /*  <otherwise>  */
      ite.m_otherwise->accept(*this);
      /*  JUMP end_offset  */
      m_code->append(JUMP);
      size_t end_offset_index = m_code->size();
      m_code->append(0); /* mock for end_offset */
      /*  <then>  */
      size_t then_offset = m_code->size(); ite.m_then->accept(*this);
      /*  <...>  */
      size_t end_offset = m_code->size();
      /*  set actuals offsets  */
      m_code->set_unit(then_offset_index, then_offset);
      m_code->set_unit(end_offset_index, end_offset);
    }
  }
  void visit(const AST::Lambda& lambda) override {
    Table locals_table{};
    size_t args_count = 0;
    for (auto& param : lambda.m_params) {
      if (locals_table.find(param) != locals_table.end()) throw std::runtime_error("Compiler error: multuple params");
      locals_table[param] = args_count++;
    }
    Pointer<Code> code{new Code()};
    bool tail = true;
    std::vector<std::string> captures{};
    std::swap(m_locals_table, locals_table);
    std::swap(m_code, code);
    std::swap(m_captures, captures);
    std::swap(tail, m_tail);
    lambda.m_body->accept(*this);
    std::swap(m_locals_table, locals_table);
    std::swap(m_code, code);
    std::swap(m_captures, captures);
    std::swap(m_tail, tail);
    size_t icode = m_consts.size();
    m_consts.push_back(std::move(code));
    m_code->append(PUSH_CONST);
    m_code->append(icode);
    tail = false; // collect captures on stack
    std::swap(m_tail, tail);
    for (auto iter = captures.rbegin(); iter != captures.rend(); ++iter) visit(AST::Name(*iter));
    std::swap(m_tail, tail);
    m_code->append(MAKE_FUNCTION);
    m_code->append(args_count);
    m_code->append(captures.size());
    if (m_tail) m_code->append(RETURN_VALUE);
  }
  void visit(const AST::Assign& assign) override {
    m_tail = false;
    assign.m_expr->accept(*this);
    auto iter = m_locals_table.find(assign.m_name);
    if (iter == m_locals_table.end()) {
      size_t ilocal = m_locals_table.size();
      m_locals_table[assign.m_name] = ilocal;
      /* Maybe I should remove next two lines */
      m_code->append(POP_LOCAL);
      m_code->append(ilocal);
    } else {
      m_code->append(POP_LOCAL);
      m_code->append(iter->second);
    }
  }
  void visit(const AST::Print& print) override {
    m_tail = false;
    print.m_expr->accept(*this);
    m_code->append(PRINT);
  }
#ifdef DEBUG
  void show() {
    std::cout << "====CONSTS====\n";
    for (auto& obj : m_consts) obj->print(std::cout);
    std::cout << "=====CODE=====\n";
    m_code->print(std::cout);
    std::cout << "===CAPTURES===\n";
    for (auto& capture : m_captures) std::cout << capture << std::endl;
  }
#endif
  void compile(const std::vector< std::unique_ptr<AST::Stmt> >& stmts) {
    for (auto& stmt : stmts) stmt->accept(*this);
    if (!m_captures.empty()) throw std::runtime_error("Compiler error: undefined captures");
    m_code->append(RETURN);
  }
  void init_evaler(Evaler& evaler) {
    Pointer<Function> function{new Function(std::vector< Pointer<Object> >(), std::move(m_code), 0)};
    evaler.init(std::move(function), std::move(m_consts));
  }
};

class Lexer {
private:
  const char* m_current;
protected:
  int m_token;
  std::string_view m_value;
public:
  Lexer(const char* source) : m_current(source), m_token(-1), m_value() {}
  ~Lexer() = default;
  int next_token() {
    while (std::isspace(*m_current)) ++m_current;
    switch (*m_current) {
      case '\0': return m_token = 0;
      case '+':
      case '-':
      case '*':
      case '(':
      case ')':
      case ',':
      case '=':
      case ':':
      case ';':
        return m_token = *m_current++;
      default:
        break;
    }
    const char* begin = m_current;
    if (std::isdigit(*m_current)) {
      while (std::isdigit(*m_current)) ++m_current;
      m_value = std::string_view(begin, m_current - begin);
      return m_token = '0';
    }
    if (std::isalpha(*m_current)) {
      while (std::isalpha(*m_current)) ++m_current;
      m_value = std::string_view(begin, m_current - begin);
      if (m_value == "function") return m_token = 'f';
      if (m_value == "print") return m_token = 'p';
      if (m_value == "if") return m_token = 'i';
      if (m_value == "then") return m_token = 't';
      if (m_value == "else") return m_token = 'e';
      if (m_value == "self") return m_token = 's';
      return m_token = 'a';
    }
    throw std::runtime_error("Lexer error: unknown token");
  }
};

class Parser : public Lexer {
public:
  Parser(const char* source) : Lexer(source) {}
  ~Parser() = default;
  std::unique_ptr<AST::Expr> parse_prim();
  std::unique_ptr<AST::Expr> parse_term();
  std::unique_ptr<AST::Expr> parse_expr();
  std::unique_ptr<AST::Stmt> parse_stmt();
  std::vector< std::unique_ptr<AST::Stmt> > parse_stmts();
};

std::unique_ptr<AST::Expr> Parser::parse_expr() {
  if (m_token == 'f') {
    next_token();
    std::vector<std::string> params{};
    while (m_token == 'a') {
      params.push_back(std::string{m_value});
      if (next_token() != ',') break;
      next_token();
    }
    if (m_token != ':') throw std::runtime_error("Parser error: ':' expected");
    next_token();
    auto body = parse_expr();
    return std::make_unique<AST::Lambda>(std::move(params), std::move(body));
  }
  if (m_token == 'i') {
    next_token();
    auto cond = parse_expr();
    if (m_token != 't') throw std::runtime_error("Parser error: 'then' expected");
    next_token();
    auto then = parse_expr();
    if (m_token != 'e') throw std::runtime_error("Parser error: 'else' expected");
    next_token();
    auto otherwise = parse_expr();
    return std::make_unique<AST::IfThenElse>(std::move(cond), std::move(then), std::move(otherwise));
  }
  auto left = parse_term();
  while (m_token == '+' || m_token == '-') {
    bool add = m_token == '+';
    next_token();
    auto right = parse_term();
    if (add) left = std::make_unique<AST::BinOp>(std::move(left), std::move(right), AST::BinOp::ADD);
    else left = std::make_unique<AST::BinOp>(std::move(left), std::move(right), AST::BinOp::SUB);
  }
  return left;
}

std::unique_ptr<AST::Expr> Parser::parse_term() {
  auto left = parse_prim();
  while (m_token == '*') {
    next_token();
    auto right = parse_prim();
    left = std::make_unique<AST::BinOp>(std::move(left), std::move(right), AST::BinOp::MUL);
  }
  return left;
}

std::unique_ptr<AST::Expr> Parser::parse_prim() {
  switch (m_token) {
    case '0': {
      int data = 0;
      for (char c : m_value) data = 10 * data + c - '0';
      Pointer<Object> integer{new Integer(data)};
      next_token();
      return std::make_unique<AST::Literal>(std::move(integer));
    }
    case 'a': {
      auto name = std::make_unique<AST::Name>(std::string{m_value});
      if (next_token() != '(') return name;
      std::unique_ptr<AST::Expr> func = std::move(name);
      while (m_token == '(') {
        std::vector< std::unique_ptr<AST::Expr> > argvec{};
        if (next_token() != ')') {
          for (;;) {
            auto arg = parse_expr();
            argvec.push_back(std::move(arg));
            if (m_token != ',') break;
            next_token();
          }
          if (m_token != ')') throw std::runtime_error("Parser error: expected ')' in argvec");
        }
        next_token();
        func = std::make_unique<AST::Value>(std::move(func), std::move(argvec));
      }
      return func;
    }
    case 's': {
      if (next_token() != '(') throw std::runtime_error("Parser error: expected '(' after 'self'");
      std::vector< std::unique_ptr<AST::Expr> > argvec{};
      if (next_token() != ')') {
        for (;;) {
          auto arg = parse_expr();
          argvec.push_back(std::move(arg));
          if (m_token != ',') break;
          next_token();
        }
        if (m_token != ')') throw std::runtime_error("Parser error: expected ')' in argvec");
      }
      next_token();
      //TODO: delete repeated code, maybe rewrite parser
      std::unique_ptr<AST::Expr> expr = std::make_unique<AST::SelfValue>(std::move(argvec));
      while (m_token == '(') {
        argvec = std::vector< std::unique_ptr<AST::Expr> >();
        if (next_token() != ')') {
          for (;;) {
            auto arg = parse_expr();
            argvec.push_back(std::move(arg));
            if (m_token != ',') break;
            next_token();
          }
          if (m_token != ')') throw std::runtime_error("Parser error: expected ')' in argvec");
        }
        next_token();
        expr = std::make_unique<AST::Value>(std::move(expr), std::move(argvec));
      }
      return expr;
    }
    case '(': {
      next_token();
      auto expr = parse_expr();
      if (m_token != ')') throw std::runtime_error("Parser error: expected ')'");
      if (next_token() != '(') return expr;
      //TODO
      while (m_token == '(') {
        std::vector< std::unique_ptr<AST::Expr> > argvec{};
        if (next_token() != ')') {
          for (;;) {
            auto arg = parse_expr();
            argvec.push_back(std::move(arg));
            if (m_token != ',') break;
            next_token();
          }
          if (m_token != ')') throw std::runtime_error("Parser error: expected ')' in argvec");
        }
        next_token();
       expr = std::make_unique<AST::Value>(std::move(expr), std::move(argvec));
      }
      return expr;
    }
    default:
      throw std::runtime_error("Parser error: wrong primary expression");
  }
}

std::unique_ptr<AST::Stmt> Parser::parse_stmt() {
  std::unique_ptr<AST::Stmt> stmt{};
  switch(m_token) {
    case 'a': {
      std::string name{m_value};
      if (next_token() != '=') throw std::runtime_error("Parser error: expected '='");
      next_token();
      auto expr = parse_expr();
      stmt = std::make_unique<AST::Assign>(std::move(name), std::move(expr));
      break;
    }
    case 'p': {
      next_token();
      auto expr = parse_expr();
      stmt = std::make_unique<AST::Print>(std::move(expr));
      break;
    }
    default:
      throw std::runtime_error("Parser error: wrong stmt");
  }
  if (m_token != ';') throw std::runtime_error("Parser error: ';' expected");
  next_token();
  return stmt;
}

std::vector< std::unique_ptr<AST::Stmt> > Parser::parse_stmts() {
  std::vector< std::unique_ptr<AST::Stmt> > stmts{};
  while (m_token != 0) {
    auto stmt = parse_stmt();
    stmts.push_back(std::move(stmt));
  }
  return stmts;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: prog [options] filename\n";
    return 0;
  }
  std::ifstream ifs{argv[argc - 1]};
  if (!ifs) {
    std::cerr << "Error: file '" << argv[argc - 1] << "' can not be opened\n";
    return 1;
  }
  bool dis = false;
  for (int i = 1; i < argc - 1; ++i) {
    if (!std::strcmp(argv[i], "-d")) {
      dis = true;
    } else {
      std::cout << "Unknown option '" << argv[i] << "'\n";
      return 0;
    }
  }
  std::string source{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
  ifs.close();
  Parser parser{source.data()};
  try {
    parser.next_token();
    auto stmts = parser.parse_stmts();
    Compiler compiler{};
    Evaler evaler{};
    compiler.compile(stmts);
    if (dis) {
      compiler.show();
    } else {
      compiler.init_evaler(evaler);
      evaler.eval();
    }
  } catch (std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
  } catch (std::bad_alloc& err) {
    std::cerr << err.what() << std::endl;
  }
  return 0;
}