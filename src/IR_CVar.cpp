#include "IR_CTypes.hpp"
#include <IR_CVar.hpp>

#include <map>
#include <cstring>

namespace IR {

    static std::map<std::string, CVar*> s_CVarMap;

    CVar::CVar(const char* name, UInt32 flags, const bool& v)
    : m_Name(name), m_Flags(flags), m_Type(CVar::Type::BOOL)
    { m_Value.as_bool = v; s_CVarMap[name] = this; }

    CVar::CVar(const char* name, UInt32 flags, const Int64& v)
    : m_Name(name), m_Flags(flags), m_Type(CVar::Type::INT64)
    { m_Value.as_int64 = v; s_CVarMap[name] = this; }

    CVar::CVar(const char* name, UInt32 flags, const Float64& v)
    : m_Name(name), m_Flags(flags), m_Type(CVar::Type::FLOAT64)
    { m_Value.as_float64 = v; s_CVarMap[name] = this; }

    CVar::CVar(const char* name, UInt32 flags, const std::string& v)
    : m_Name(name), m_Flags(flags), m_Type(CVar::Type::STRING)
    {
        m_Value.as_string = new char[v.size() + 1];
        m_Value.as_string[v.size()] = '\0';
        memcpy(m_Value.as_string, v.data(), v.size());

        s_CVarMap[name] = this;
    }

    CVar::CVar(const char* name, UInt32 flags, const char* v)
    : m_Name(name), m_Flags(flags), m_Type(CVar::Type::STRING)
    {
        UInt64 len = strlen(v);
        m_Value.as_string = new char[len + 1];
        m_Value.as_string[len] = '\0';
        memcpy(m_Value.as_string, v, len);

        s_CVarMap[name] = this;
    }

    CVar::~CVar()
    {
        if (m_Type == CVar::Type::STRING) {
            delete[] m_Value.as_string;
        }

        auto it = s_CVarMap.find(m_Name);
        if (it != s_CVarMap.end()) {
            s_CVarMap.erase(it);
        }
    }

    void CVar::SetBool(const bool& v)
    {
        if (m_Type != CVar::Type::BOOL) {
            return;
        }

        m_Value.as_bool = v;
    }

    void CVar::SetInt64(const Int64& v)
    {
		if (m_Type == CVar::Type::BOOL) {
			return SetBool((bool)v);
		}

		if (m_Type == CVar::Type::FLOAT64) {
			return SetFloat64((Float64)v);
		}

        if (m_Type != CVar::Type::INT64) {
            return;
        }

        m_Value.as_int64 = v;
    }

    void CVar::SetFloat64(const Float64& v)
    {
		if (m_Type == CVar::Type::BOOL) {
			return SetBool((bool)v);
		}

		if (m_Type == CVar::Type::INT64) {
			return SetInt64((Int64)v);
		}

        if (m_Type != CVar::Type::FLOAT64) {
            return;
        }
        m_Value.as_float64 = v;
    }

    void CVar::SetString(const std::string& v)
    {
        if (m_Type != CVar::Type::STRING) {
            return;
        }

        if (m_Value.as_string) {
            delete[] m_Value.as_string;

            m_Value.as_string = new char[v.size() + 1];
            m_Value.as_string[v.size()] = '\0';
            memcpy(m_Value.as_string, v.data(), v.size());
        }
    }

    void CVar::SetString(const char* v)
    {
        if (m_Type != CVar::Type::STRING) {
            return;
        }

        if (m_Value.as_string) {
            delete[] m_Value.as_string;

            UInt64 len = strlen(v);
            m_Value.as_string = new char[len + 1];
            m_Value.as_string[len] = '\0';
            memcpy(m_Value.as_string, v, len);
        }
    }

    CVar* CVar::Get(const char* name)
    {
        auto it = s_CVarMap.find(name);
        if (it != s_CVarMap.end()) {
            return it->second;
        }

        return nullptr;
    }

	void CVar::Iterate(const std::function<void(CVar*)>& callback)
	{
		for (auto& [_, cvar] : s_CVarMap) {
			if (!cvar) {
				continue;
			}

			callback(cvar);
		}
	}
}
