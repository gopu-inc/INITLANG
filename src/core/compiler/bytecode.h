// src/core/compiler/bytecode.h
enum class OpCode {
    // Constantes
    OP_CONSTANT, OP_NULL, OP_TRUE, OP_FALSE,
    
    // Variables
    OP_DEFINE_GLOBAL, OP_GET_GLOBAL, OP_SET_GLOBAL,
    OP_GET_LOCAL, OP_SET_LOCAL,
    
    // Opérations
    OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
    OP_NEGATE, OP_NOT,
    
    // Comparaisons
    OP_EQUAL, OP_NOT_EQUAL, OP_GREATER, OP_LESS,
    
    // Contrôle
    OP_JUMP, OP_JUMP_IF_FALSE, OP_LOOP,
    OP_CALL, OP_RETURN,
    
    // Structures
    OP_BUILD_LIST, OP_BUILD_STRUCT,
    
    // Spécial INITLANG
    OP_INIT_GER, OP_INIT_LOG
};

struct Chunk {
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;
};
