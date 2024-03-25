//
// Created by DerrickHsu on 2024/3/25.
//

#ifndef AUTO_JSON_H
#define AUTO_JSON_H

#include <string>
#include <typeinfo>
#include <type_traits>
#include "json/json.h"

enum AutoJsonMethod {
    Default = 0,
    Marshal = 1,
    Unmarshal = 2,
};

namespace _autojson {
    template <typename T>
    struct MarshalHelper_check
    {
        // Check if the template class T has 'SetJsonMapping' function
        template <typename U>
        static constexpr auto check(U *u) -> decltype(std::declval<U>().SetJsonMapping(), std::true_type());

        // If not, return std::false_type
        static constexpr std::false_type check(...);

        static constexpr bool exist = std::is_same<decltype(check(std::declval<T *>())), std::true_type>::value;
    };

    /**
     * Generic serialize method for class that has 'SetJsonMapping' function(return an empty string on failure)
     * @tparam T Derived class of AutoJsonHelper
     * @param json_string[in,out] JSON result after serializing
     * @param obj[in] Derived class object of AutoJsonHelper
     */
    template <typename T, typename std::enable_if<MarshalHelper_check<T>::exist,int>::type = 0>
    inline void _marshal(std::string &json_string, T &obj) {
        obj.Clear();
        obj.SetMethod(AutoJsonMethod::Marshal);
        obj.SetJsonMapping();
        json_string = obj.GetString();
    }

    /**
     * Generic serialize method for class that DOESNT have 'SetJsonMapping' function(return an empty string)
     * @tparam T Template class
     * @param json_string[in,out] JSON result
     * @param obj[in] Object of template class
     */
    template <typename T, typename std::enable_if<!MarshalHelper_check<T>::exist,int>::type = 0>
    inline void _marshal(std::string &json_string, T &obj) {
        json_string = std::string{};
    }

    /**
     * Generic deserialize method for class that has 'SetJsonMapping' function(return an empty string on failure)
     * @tparam T Derived class of AutoJsonHelper
     * @param json_string[in] The Json needs to be deserialized
     * @param obj[in,out] Object result after deserializing
     */
    template <typename T, typename std::enable_if<MarshalHelper_check<T>::exist,int>::type = 0>
    inline void _unmarshal(std::string &json_string, T &obj) {
        obj.Clear();
        Json::Reader reader;
        Json::Value root;
        obj.SetMethod(AutoJsonMethod::Unmarshal);
        if (reader.parse(json_string, root) && !root.empty() && root.isObject()) {
            obj.SetDocument(root);
            obj.SetJsonMapping();
        }
    }

    /**
     * Generic deserialize method for class that DOESNT have 'SetJsonMapping' function(do nothing)
     * @tparam T Template class
     * @param json_string[in] The Json needs to be deserialized
     * @param obj[in,out] Object result after deserializing
     */
    template <typename T, typename std::enable_if<!MarshalHelper_check<T>::exist,int>::type = 0>
    inline void _unmarshal(std::string &json_string, T &obj) {}
}

namespace AutoJson {
    /**
     * Serialize object to JSON string
     * @param json_string[in,out] JSON result
     * @param obj[in] Object needs to be serialized
     */
    template <typename T>
    inline void Marshal(std::string &json_string, const T &obj) {
        _autojson::_marshal(json_string, const_cast<T&>(obj));
    }

    /**
     * Deserialized JSON string to object
     * @param json_string[in] JSON string needs to be deserialized
     * @param obj[in,out] Object result
     */
    template <typename T>
    inline void Unmarshal(std::string &json_string, const T &obj) {
        _autojson::_unmarshal(json_string, const_cast<T&>(obj));
    }
}

/**
 * Specify the mapping between member variables and JSON fields
 */
#define AUTO_JSON_MAPPING(variable, key)                                \
    if (AutoJsonMethod::Marshal == this->method_) {                    \
        _marshal_into_document(variable, key);                         \
    } else if (AutoJsonMethod::Unmarshal == this->method_) {           \
        _unmarshal_into_obj(variable, key);                            \
    }

class AutoJsonHelper {
public:
    AutoJsonHelper() = default;
    ~AutoJsonHelper() = default;
    virtual void SetJsonMapping() = 0;
    void SetMethod(AutoJsonMethod method) { this->method_ = method; };
    void SetDocument(const Json::Value &doc) { this->document_ = doc; };
    Json::Value GetDocument() {return this->document_;};

    /**
     * Convert JSON document to JSON string
     * @return JSON string
     */
    std::string GetString() {
        Json::FastWriter writer;

        std::string result = writer.write(this->document_);
        // if document is empty, return empty string
        if (this->document_.empty()) {
            return std::string{};
        }
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        return result;
    };

    /**
     * @brief Clear member variables
     */
    void Clear() {
        this->document_.clear();
        this->method_ = AutoJsonMethod::Default;
    };

protected:
    AutoJsonMethod method_ = AutoJsonMethod::Default; //!< Method Type. 0=>Not Init, 1=>Serialize, 2=>Deserialize

    /**
     * Serialize variable into JSON according to the specified keys
     */
    template <typename T>
    void _marshal_into_document(T &var, const std::string &json_key);

    /**
     * Deserialize JSON into variable according to the specified keys
     */
    template <typename T>
    void _unmarshal_into_obj(T &var, const std::string &json_key);

private:

    template <typename T, typename std::enable_if<_autojson::MarshalHelper_check<T>::exist,int>::type = 0>
    inline bool _marshal_for_spl_(T &obj, Json::Value &dc) {
        obj.Clear();
        obj.SetMethod(AutoJsonMethod::Marshal);
        obj.SetJsonMapping();
        dc = obj.GetDocument();
        return true;
    }

    template <typename T, typename std::enable_if<!_autojson::MarshalHelper_check<T>::exist,int>::type = 0>
    inline bool _marshal_for_spl_(T &obj, Json::Value &dc) {
        this->_marshal_into_document_(obj, dc);
        return true;
    }

    /**
     * Serialize core function
     * @tparam T Basic data types other than map, vector, etc., from the STL.
     * @param var[in] Value needs to be serialized
     * @param dc[in,out] The Document that Value is serialized into
     */
    template <typename T>
    void _marshal_into_document_(const T &var, Json::Value &dc);

    // The Overload versions of _marshal_into_document_ for STL types
    template <typename T>
    void _marshal_into_document_(const std::map<std::string, T> &var, Json::Value &dc);

    template <typename T>
    void _marshal_into_document_(const std::map<long, T> &var, Json::Value &dc);

    template <typename T>
    void _marshal_into_document_(const std::map<int, T> &var, Json::Value &dc);

    template <typename T>
    void _marshal_into_document_(const std::vector<T> &var, Json::Value &dc);

    /**
     * @brief Check if the template type T is a custom data structure for serializing
     */
    template <typename T, typename std::enable_if<_autojson::MarshalHelper_check<T>::exist,int>::type = 0>
    inline bool _unmarshal_for_spl_(T &obj, const Json::Value &dc) {
        obj.Clear();
        obj.SetMethod(AutoJsonMethod::Unmarshal);
        if (!dc.empty() && dc.isObject()) {
            obj.SetDocument(dc);
            obj.SetJsonMapping();
        } else {
            return false;
        }
        return true;
    }

    /**
     * @brief Check if the template type T is not a custom data structure for deserializing
     */
    template <typename T, typename std::enable_if<!_autojson::MarshalHelper_check<T>::exist,int>::type = 0>
    inline bool _unmarshal_for_spl_(T &obj, const Json::Value &dc) {
        this->_unmarshal_into_obj_(obj, dc);
        return true;
    }

    // Deserialize core function for basic data types
    void _unmarshal_into_obj_(int &var, const Json::Value &dc);
    void _unmarshal_into_obj_(long &var, const Json::Value &dc);
    void _unmarshal_into_obj_(bool &var, const Json::Value &dc);
    void _unmarshal_into_obj_(float &var, const Json::Value &dc);
    void _unmarshal_into_obj_(double &var, const Json::Value &dc);
    void _unmarshal_into_obj_(std::string &var, const Json::Value &dc);

    // The Overload versions of _unmarshal_into_obj_ for STL types
    // If deserialization fails, its key should not exist in var
    template <typename T>
    void _unmarshal_into_obj_(std::map<std::string, T> &var, const Json::Value &dc);

    template <typename T>
    void _unmarshal_into_obj_(std::map<long, T> &var, const Json::Value &dc);

    template <typename T>
    void _unmarshal_into_obj_(std::map<int, T> &var, const Json::Value &dc);

    template <typename T>
    void _unmarshal_into_obj_(std::vector<T> &var, const Json::Value &dc);

private:
    Json::Value document_;
};

template <typename T>
inline void AutoJsonHelper::_marshal_into_document(T &var, const std::string &json_key) {
    _marshal_for_spl_(var, this->document_[json_key]);
}

template <typename T>
inline void AutoJsonHelper::_marshal_into_document_(const T &var, Json::Value &dc) {
    dc = var;
}

template <>
inline void AutoJsonHelper::_marshal_into_document_<long>(const long &var, Json::Value &dc) {
    dc = Json::Int64(var);
}

template<typename T>
inline void AutoJsonHelper::_marshal_into_document_(const std::map<std::string, T> &var, Json::Value &dc) {
    for (const auto &it_var : var) {
        _marshal_for_spl_(const_cast<T&>(it_var.second), dc[it_var.first]);
    }
}

template<typename T>
inline void AutoJsonHelper::_marshal_into_document_(const std::map<long, T> &var, Json::Value &dc) {
    for (const auto &it_var : var) {
        _marshal_for_spl_(const_cast<T&>(it_var.second), dc[std::to_string(it_var.first)]);
    }
}

template<typename T>
inline void AutoJsonHelper::_marshal_into_document_(const std::map<int, T> &var, Json::Value &dc) {
    for (const auto &it_var : var) {
        _marshal_for_spl_(const_cast<T&>(it_var.second), dc[std::to_string(it_var.first)]);
    }
}

template<typename T>
inline void AutoJsonHelper::_marshal_into_document_(const std::vector<T> &var, Json::Value &dc) {
    for (int i = 0; i < var.size(); ++i) {
        _marshal_for_spl_(const_cast<T&>(var[i]), dc[i]);
    }
}

template <typename T>
inline void AutoJsonHelper::_unmarshal_into_obj(T &var, const std::string &json_key) {
    if (this->document_.isMember(json_key)) {
        _unmarshal_for_spl_(var, this->document_[json_key]);
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(int &var, const Json::Value &dc) {
    if (dc.isIntegral()) {
        var = dc.asInt();
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(long &var, const Json::Value &dc) {
    if (dc.isIntegral()) {
        var = dc.asInt64();
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(bool &var, const Json::Value &dc) {
    if (dc.isBool()) {
        var = dc.asBool();
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(float &var, const Json::Value &dc) {
    if (dc.isDouble()) {
        var = dc.asFloat();
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(double &var, const Json::Value &dc) {
    if (dc.isDouble()) {
        var = dc.asDouble();
    }
}

inline void AutoJsonHelper::_unmarshal_into_obj_(std::string &var, const Json::Value &dc) {
    if (dc.isString()) {
        var = dc.asString();
    }
}

template <typename T>
inline void AutoJsonHelper::_unmarshal_into_obj_(std::map<std::string, T> &var, const Json::Value &dc) {
    if (dc.isObject()) {
        var.clear();
        auto mems = dc.getMemberNames();
        for (auto &mem : mems) {
            T item;
            if (_unmarshal_for_spl_(item, dc[mem])) {
                var.template emplace(mem, item);
            }
        }
    }
}

template <typename T>
inline void AutoJsonHelper::_unmarshal_into_obj_(std::map<long, T> &var, const Json::Value &dc) {
    if (dc.isObject()) {
        var.clear();
        auto mems = dc.getMemberNames();
        for (auto &mem : mems) {
            T item;
            if (_unmarshal_for_spl_(item, dc[mem])) {
                var.template emplace(atol(mem.c_str()), item);
            }
        }
    }
}

template <typename T>
inline void AutoJsonHelper::_unmarshal_into_obj_(std::vector<T> &var, const Json::Value &dc) {
    if (dc.isArray()) {
        var.clear();
        for (int i = 0; i < dc.size(); ++i) {
            T item;
            if (_unmarshal_for_spl_(item, dc[i])) {
                var.template emplace_back(item);
            } else {
                var = std::vector<T>{};
                return;
            }
        }
    }
}

template <typename T>
inline void AutoJsonHelper::_unmarshal_into_obj_(std::map<int, T> &var, const Json::Value &dc) {
    if (dc.isObject()) {
        var.clear();
        auto mems = dc.getMemberNames();
        for (auto &mem : mems) {
            T item;
            if (_unmarshal_for_spl_(item, dc[mem])) {
                var.template emplace(atoi(mem.c_str()), item);
            }
        }
    }
}

#endif //AUTO_JSON_H
