# AUTO JSON
**[[English](README.md)] [[中文](README_CN.md)]**

A C++ serialization and deserialization library based on [Jsoncpp](https://github.com/open-source-parsers/jsoncpp), replace the previously cumbersome serialization/deserialization code with class inheritance and variable-field mapping.

## Dependencies
* [Jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* C++ 11
* [GoogleTest](https://github.com/google/googletest?tab=readme-ov-file#welcome-to-googletest-googles-c-test-framework) (if your project need unit test)

## Quick Start

### Prepare
1. Install the Jsoncpp library according to the official documentation (ignore if already installed).
2. Download Auto_Json library.
```shell
git clone  https://github.com/DerrickHsu97/auto_json.git
```
3. Include `auto_json.h` in your project.
```c++
#include "auto_json.h"
```

### Using AUTO_JSON
<a id="demo"></a>
1. Make your data class/struct inherit from the `AutoJsonHelper` class and override the `SetJsonMapping()` method (used to specify the association between variables and fields).
```c++
struct Demo {
    int id;
    std::string name;
    
    void SetJsonMapping() override {
        AUTO_JSON_MAPPING(id, "demo_id");
        AUTO_JSON_MAPPING(name, "name");
    }
};
```
2. Serialize/Deserialize.
```c++
// serialize 'obj' into 'json'
AutoJson::Marshal(json, obj);

// deserialize 'json' into 'obj'
AutoJson::Unmarshal(json, obj);
```

## Unit Test (if need)
Support unit testing with [GoogleTest](https://github.com/google/googletest?tab=readme-ov-file#welcome-to-googletest-googles-c-test-framework) framework, simply place the `test_auto_json.cpp` file in your unit test directory. For detailed instructions on using GoogleTest, please refer to [GoogleTest User Guide](https://google.github.io/googletest/).

## DEMO
Using the structure `Demo` from the [Using AUTO_JSON](#demo) section as an example.

### Serialize

#### Code
```c++
#include <iostream>
#include "auto_json.h"
#include "string"

int main() {
    Demo demo_obj;
    demo_obj.id = 20240325;
    demo_obj.name = "auto_json";
    
    std::string rst;
    AutoJson::Marshal(rst, demo_obj);
    
    std::cout << rst << std::endl;
}
```

#### Output
```json
{"demo_id":20240325,"name":"auto_json"}
```

### Deserialize

#### Code
```c++
#include <iostream>
#include "auto_json.h"
#include "string"

int main() {
    std::string json = R"({"demo_id":20240325,"name":"auto_json"})";
    Demo rst_obj;
    AutoJson::Unmarshal(json, rst_obj);
    
    std::cout << "id:" << rst_obj.id << std::endl;
    std::cout << "name:" << rst_obj.name << std::endl;
}
```

#### Output
```text
id:20240325
name:auto_json
```