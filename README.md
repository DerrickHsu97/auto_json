# AUTO JSON
**[[English](README.md)] [[中文](README_zh.md)]**

基于 [Jsoncpp](https://github.com/open-source-parsers/jsoncpp) 实现的C++序列化&反序列化库。进行简单的**类继承**和**变量-字段映射**，
取代之前繁琐的序列化/反序列化代码逻辑编写。

## 依赖
* [Jsoncpp](https://github.com/open-source-parsers/jsoncpp) 
* C++ 11
* [GoogleTest](https://github.com/google/googletest?tab=readme-ov-file#welcome-to-googletest-googles-c-test-framework) (如果需要测试)

## 使用方法

### 准备工作
1. 根据官方文档安装Jsoncpp库(已安装则忽略)
2. 下载auto_json库
```shell
git clone  https://github.com/DerrickHsu97/auto_json.git
```
3. 将auto_json.h加入到您的工程中，并引用
```c++
#include "auto_json.h"
```

### 使用AUTO_JSON
<a id="demo"></a>
1. 使您的类/结构体继承`AutoJsonHelper`类，并重写`SetJsonMapping()`方法(SetJsonMapping()用与指定变量与字段的关联关系)
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
2. 进行序列化/反序列化
```c++
// 序列化'obj'输出到'json'
AutoJson::Marshal(json, obj);

// 反序列化'json'输出到'obj'
AutoJson::Unmarshal(json, obj);
```

## 单元测试
支持使用 [GoogleTest](https://github.com/google/googletest?tab=readme-ov-file#welcome-to-googletest-googles-c-test-framework) 框架进行单元测试，
直接将`test_auto_json.cpp`文件放到您的单元测试文件目录下即可。GoogleTest详细使用方法参考[GoogleTest用户手册](https://google.github.io/googletest/) 。

## DEMO
沿用[使用AUTO_JSON](#demo)章节中的结构体`Demo`进行示例展示

### 序列化

#### 代码
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

#### 结果
```json
{"demo_id":20240325,"name":"auto_json"}
```

### 反序列化

#### 代码
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

#### 结果
```text
id:20240325
name:auto_json
```