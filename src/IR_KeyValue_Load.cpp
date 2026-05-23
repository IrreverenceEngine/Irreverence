#include <IR_Common.hpp>
#include <IR_KeyValue.hpp>
#include <IR_File.hpp>

#include <cstring>

namespace IR {

    enum TokenType {
        TOKEN_NONE,
        TOKEN_STRING,
        TOKEN_NUMBER,
        TOKEN_LSQUARE,
        TOKEN_RSQUARE,
        TOKEN_LBRACE,
        TOKEN_RBRACE
    };

    struct Token {
        TokenType type;
        const char* start;
        UInt64 length;
    };

    static bool tokenize(const char* data, UInt64 size, std::vector<Token>& tokens);
    static bool parse(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv);

    KeyValue* KeyValue::Load(const char* path)
    {
        File file(path, "rb");

        if (!file.IsOpen()) {
            return nullptr;
        }

        UInt64 size;

        char* contents = file.ReadAll(&size);
        IR_DEFER({ delete[] contents; });

        return LoadMemory(contents, size);
    }

    constexpr const char* TokenTypeToString(TokenType type)
    {
        switch (type) {
            case TOKEN_NONE: return "NONE";
            case TOKEN_STRING: return "STRING";
            case TOKEN_NUMBER: return "NUMBER";
            case TOKEN_LSQUARE: return "LSQUARE";
            case TOKEN_RSQUARE: return "RSQUARE";
            case TOKEN_LBRACE: return "LBRACE";
            case TOKEN_RBRACE: return "RBRACE";
            default: return "UNKNOWN";
        }
    }

    KeyValue* KeyValue::LoadMemory(const char* data, UInt64 size)
    {
        std::vector<Token> tokens;

        if (!tokenize(data, size, tokens)) {
            return nullptr;
        }


        UInt64 index = 0;
        KeyValue* kv = new KeyValue(nullptr, nullptr, KeyValue::Type::OBJECT);

        if (!parse(tokens, index, kv)) {
            delete kv;
            return nullptr;
        }

        return kv;
    }

    static bool tokenize(const char* data, UInt64 size, std::vector<Token>& tokens)
    {
        if (size < 1) {
            return false;
        }

        UInt64 pos = 0;
        bool comment = false;
        Token curToken;
        curToken.length = 0;
        curToken.start = nullptr;
        curToken.type = TOKEN_STRING;

        while (pos < size) {
            char c = data[pos++];

            if (c == '\n' || c == '\r') {
                comment = false;
                continue;
            }

            if (comment) {
                continue;
            }

            switch (c) {
                case '{': {
                    if (curToken.length > 0) {
                        curToken.type = TOKEN_STRING;
                        tokens.push_back(curToken);
                        curToken = {};
                    }

                    tokens.push_back({ TOKEN_LBRACE });
                    break;
                }

                case '}': {
                    if (curToken.length > 0) {
                        curToken.type = TOKEN_STRING;
                        tokens.push_back(curToken);
                        curToken = {};
                    }

                    tokens.push_back({ TOKEN_RBRACE });
                    break;
                }

                case '[': {
                    if (curToken.length > 0) {
                        curToken.type = TOKEN_STRING;
                        tokens.push_back(curToken);
                        curToken = {};
                    }

                    tokens.push_back({ TOKEN_LSQUARE });
                    break;
                }

                case ']': {
                    if (curToken.length > 0) {
                        curToken.type = TOKEN_STRING;
                        tokens.push_back(curToken);
                        curToken = {};
                    }

                    tokens.push_back({ TOKEN_RSQUARE });
                    break;
                }

                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7':
                case '8': case '9': case '.': case '-': {
                    if (curToken.length > 0) {
                        curToken.length++;
                        break;
                    }

                    Token token;
                    token.length = 1;
                    token.type = TOKEN_NUMBER;
                    token.start = data + pos - 1;

                    bool over = false;
                    bool dot = c == '.' ? true : false;

                    while (pos < size && !over) {
                        char c2 = data[pos];

                        if (c2 >= '0' && c2 <= '9') {
                            token.length++;
                        } else if (c2 == '.' && !dot) {
                            dot = true;
                            token.length++;
                        } else {
                            over = true;
                            --pos;
                        }

                        ++pos;
                    }

                    tokens.push_back(token);
                    break;
                }

                case '\"': {
                    bool over = false;

                    Token token;
                    token.start = data + pos;
                    token.length = 1;
                    token.type = TOKEN_STRING;

                    while (pos < size && !over) {
                        char c2 = data[pos];

                        switch (c2) {
                            case '\"': {
                                over = true;
                                token.length -= 1;
                                break;
                            }

                            default: {
                                token.length++;
                                break;
                            }
                        }

                        ++pos;
                    }

                    tokens.push_back(token);
                    break;
                }

                case '/': {
                    if (data[pos] != '/') {
                        curToken.length++;
                        break;
                    }

                    ++pos;

                    comment = true;
                    break;
                }

                case ' ': case '\t': case '\n': case '\r': {
                    if (curToken.length < 1) {
                        break;
                    }

                    curToken.type = TOKEN_STRING;

                    tokens.push_back(curToken);

                    curToken = {};

                    break;
                }

                default: {
                    if (curToken.length < 1) {
                        curToken.start = data + pos - 1;
                        curToken.length = 1;
                        break;
                    }

                    curToken.length++;

                    break;
                }
            }
        }

        return true;
    }

    static bool parse_object(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv, bool bObjectToArray = false);
    static bool parse_array(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv, bool bObjectToArray = false);

    static bool parse(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv)
    {
        bool initial = kv->GetKey().empty();

        if (index >= tokens.size()) {
            return false;
        }

        const Token& token = tokens[index];

        if (token.type == TOKEN_STRING) {
            return parse_object(tokens, index, kv);
        }

        return parse_array(tokens, index, kv);
    }

    constexpr UInt64 MAX_BUFFER_SIZE = 4096;

    static bool parse_object(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv, bool bObjectToArray)
    {
        bool initial = kv->GetKey().empty();

        static char key_buffer[MAX_BUFFER_SIZE];
        static char value_buffer[MAX_BUFFER_SIZE];

        while (index < tokens.size()) {
            const Token& key = tokens[index++];

            std::memset(key_buffer, 0, sizeof(key_buffer));
            std::memset(value_buffer, 0, sizeof(value_buffer));

            // NOTE: Just a hacky fix for reading quake3 shader files...
            // will remove and change once we fork TrenchBroom
            if (key.type == TOKEN_LBRACE && kv->ChildCount() == 0 && !bObjectToArray) {
                kv->SetType(KeyValue::Type::ARRAY);

                bool success = parse_array(tokens, --index, kv, true);

                const Token& next = tokens[index - 1];

                return true;
            }

            if (key.type == TOKEN_RBRACE) {
                return true;
            }

            if (key.type != TOKEN_STRING) {
                IR_MSG(ERROR, "Expected string key at position %llu in KeyValue data, found type %d.", index - 1, key.type);
                return false;
            }

            for (UInt64 i = 0; i < key.length; i++) {
                key_buffer[i] = key.start[i];
            }

            const Token& value = tokens[index++];

            switch (value.type) {
                case TOKEN_STRING: {
                    for (UInt64 i = 0; i < value.length; i++) {
                        value_buffer[i] = value.start[i];
                    }

                    kv->AddChildString(key_buffer, value_buffer);
                    break;
                }

                case TOKEN_NUMBER: {
                    for (UInt64 i = 0; i < value.length; i++) {
                        value_buffer[i] = value.start[i];
                    }

                    Float64 num = std::strtod(value_buffer, nullptr);

                    kv->AddChildNumber(key_buffer, num);
                    break;
                }

                case TOKEN_LBRACE: {
                    KeyValue* next = kv->AddChildObject(key_buffer);

                    if (!parse_object(tokens, index, next)) {
                        IR_MSG(ERROR, "Failed to parse object at position %llu in KeyValue data.", index - 1);
                        return false;
                    }

                    break;
                }

                case TOKEN_LSQUARE: {
                    KeyValue* next = kv->AddChildArray(key_buffer);

                    if (!parse_array(tokens, index, next)) {
                        IR_MSG(ERROR, "Failed to parse array at position %llu in KeyValue data.", index - 1);
                        return false;
                    }

                    break;
                }

                case TOKEN_NONE:
                case TOKEN_RSQUARE:
                case TOKEN_RBRACE: {
                    IR_MSG(ERROR, "Unexpected token type %d at position %llu in KeyValue data.", value.type, index - 1);
                    return false;
                }
            }
        }

        return true;
    }

    static bool parse_array(const std::vector<Token>& tokens, UInt64& index, KeyValue* kv, bool bObjectToArray)
    {
        static char value_buffer[MAX_BUFFER_SIZE];

        while (index < tokens.size()) {
            const Token& token = tokens[index++];

            std::memset(value_buffer, 0, sizeof(value_buffer));

            if (token.type == TOKEN_RSQUARE) {
                return true;
            }

            if (token.type == TOKEN_RBRACE) {
                if (!bObjectToArray) {
                    IR_MSG(ERROR, "Unexpected '}' at position %llu in KeyValue data.", index - 1);
                    return false;
                }

                return true;
            }

            switch (token.type) {
                case TOKEN_STRING: {
                    for (UInt64 i = 0; i < token.length; i++) {
                        value_buffer[i] = token.start[i];
                    }

                    kv->AddChildString(value_buffer);
                    break;
                }

                case TOKEN_NUMBER: {
                    for (UInt64 i = 0; i < token.length; i++) {
                        value_buffer[i] = token.start[i];
                    }

                    Float64 num = std::strtod(value_buffer, nullptr);

                    kv->AddChildNumber(num);
                    break;
                }

                case TOKEN_LBRACE: {
                    KeyValue* next = kv->AddChildObject();

                    if (!parse_object(tokens, index, next)) {
                        IR_MSG(ERROR, "Failed to parse object at position %llu in KeyValue data.", index - 1);
                        return false;
                    }

                    break;
                }

                case TOKEN_LSQUARE: {
                    KeyValue* next = kv->AddChildArray();

                    if (!parse_array(tokens, index, next)) {
                        IR_MSG(ERROR, "Failed to parse array at position %llu in KeyValue data.", index - 1);
                        return false;
                    }

                    break;
                }

                case TOKEN_NONE:
                case TOKEN_RBRACE:
                case TOKEN_RSQUARE: {
                    IR_MSG(ERROR, "Unexpected token type %d at position %llu in KeyValue data.", token.type, index - 1);
                    return false;
                }
            }
        }

        return true;
    }
}
