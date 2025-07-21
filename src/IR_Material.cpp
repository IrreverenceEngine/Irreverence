#include <IR_Material.hpp>

#include <string>
#include <vector>
#include <fstream>

namespace IR::Renderer {

    struct MToken {
        enum class Type {
            STRING,
            OPEN_CURLY,
            CLOSED_CURLY,
        };

        Type type;
        std::string data;
        UInt32 line;
    };

    bool Material::Init(const char* path)
    {
        std::ifstream file(("assets/materials/" + std::string(path)).c_str());
        if (!file.is_open()) {
            IR_MSG(ERROR, "Failed to open Material File (.shader) \"%s\"", path);
            return false;
        }

		std::string fdata((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        char* data = fdata.data();

        std::vector<MToken> tokens;

        UInt32 lineCount = 0;
        while (char c = *data++) {
            if (c == ' ') {
                continue;
            }

            if (c == '\n') {
                lineCount++;
                continue;
            }

            switch (c) {
                case '/': {
                    char n = *data;

                    if (n == '/') {
                        do {
                            c = *data++;
                        } while (c != '\n');
                        lineCount++;
                        continue;
                    }

                    break;
                }
                case '{': { tokens.push_back({ MToken::Type::OPEN_CURLY, "", lineCount }); continue; }
                case '}': { tokens.push_back({ MToken::Type::CLOSED_CURLY, "", lineCount }); continue; }
            }

            if (!iscntrl(c)) {
                const char* begin = data - 1;

                while ((c = *data)) {
                    if (iscntrl(c) || c == '{' || c == '}' || c == ' ') {
                        break;
                    }

                    data++;
                }

                const char* end = data;
                size_t size = end - begin;

                tokens.push_back({ MToken::Type::STRING, std::string(begin, end), lineCount });
            }
        }

        for (const auto& token : tokens) {
            if (token.type == MToken::Type::STRING) {
                IR_MSG(INFO, "[%d] %s", token.line, token.data.c_str());
            } else if (token.type == MToken::Type::OPEN_CURLY) {
                IR_MSG(INFO, "[%d] {", token.line);
            } else if (token.type == MToken::Type::CLOSED_CURLY) {
                IR_MSG(INFO, "[%d] }", token.line);
            }
        }

        return true;
    }
}