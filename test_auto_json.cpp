//
// Created by DerrickHsu on 2024/3/25.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "cpp_free_mock.h"
#include "auto_json.h"

using namespace std;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::InvokeWithoutArgs;
using ::testing::WithArgs;

// float有精度缺失marshal时不做单测
struct InnerMsg : public AutoJsonHelper {
    int id;
    std::string name;
    double avg_double;
    std::vector<std::string> array_string;
    std::vector<int> array_int;

    void SetJsonMapping() override {
        AUTO_JSON_MAPPING(id, "innermsg_id");
        AUTO_JSON_MAPPING(name, "innermsg_name");
        AUTO_JSON_MAPPING(avg_double, "innermsg_avg_double");
        AUTO_JSON_MAPPING(array_string, "innermsg_array_string");
        AUTO_JSON_MAPPING(array_int, "innermsg_array_int");
    }

    void reset() {
        id = 0;
        name = "";
        avg_double = 0;
        array_string.clear();
        array_int.clear();
    }
};

struct JsonMsg : public AutoJsonHelper {
    int id;
    std::string name;
    double avg_double;
    std::vector<std::string> array_string;
    std::vector<int> array_int;
    std::vector<InnerMsg> array_innermsg;
    std::map<std::string, std::string> map_string_string;
    std::map<std::string, int> map_string_int;
    std::map<std::string, InnerMsg> map_string_innermsg;
    std::map<int, std::string> map_int_string;
    std::map<int, int> map_int_int;
    std::map<int, InnerMsg> map_int_innermsg;
    InnerMsg innermsg;

    void SetJsonMapping() override {
        AUTO_JSON_MAPPING(id, "id");
        AUTO_JSON_MAPPING(name, "name");
        AUTO_JSON_MAPPING(avg_double, "avg_double");
        AUTO_JSON_MAPPING(array_string, "array_string");
        AUTO_JSON_MAPPING(array_int, "array_int");
        AUTO_JSON_MAPPING(array_innermsg, "array_innermsg");
        AUTO_JSON_MAPPING(map_string_string, "map_string_string");
        AUTO_JSON_MAPPING(map_string_int, "map_string_int");
        AUTO_JSON_MAPPING(map_string_innermsg, "map_string_innermsg");
        AUTO_JSON_MAPPING(map_int_string, "map_int_string");
        AUTO_JSON_MAPPING(map_int_int, "map_int_int");
        AUTO_JSON_MAPPING(map_int_innermsg, "map_int_innermsg");
        AUTO_JSON_MAPPING(innermsg, "innermsg");
    }
};

class AutoJsonTest : public ::testing::Test {
public:
    virtual void SetUp() {
    }
    virtual void TearDown() {
        CLEAR_MOCKER();
    }
public:
    const double pai = 3.14159265358979626;
    const double pai_1 = 4.25260376469080737;
    const double pai_2 = 5.36371487570191848;
    const double pai_3 = 6.47482598681202959;
};


/* ====== 测试Case说明 ======
 * -----Marshal-----
 * Case1: 正常格式的marshal
 * Case2: marshal空结构体
 * Case3: 结构体中部分字段无需marshal
 * Case4: 空SetJsonMapping函数
 * Case5: 未继承AutoJsonHelper调用AutoJson::Marshal
 * -----Unmarshal-----
 * Case1: 正常格式的unmarshal
 * Case2: json中字段全部为默认值
 * Case3: json空串
 * Case4: 结构体中部分字段无需unmarshal
 * Case5: 空SetJsonMapping函数
 * Case6: 未继承AutoJsonHelper调用AutoJson::Unmarshal
 * Case7: json串中key对应的value类型非结构体中的类型
 * =========================
 */

// case1: 正常格式的marshal
TEST_F(AutoJsonTest, TestMarshal_case1) {
    InnerMsg inner_1;
    InnerMsg inner_2;
    InnerMsg inner_3;
    JsonMsg json_msg;

    inner_1.id = 1;
    inner_1.name = "inner_1";
    inner_1.avg_double = 3.14159265358979626;
    inner_1.array_string = std::vector<std::string>{"inner_string_1", "inner_string_2", "inner_string_3"};
    inner_1.array_int = std::vector<int>{10001, 10002, 10003, 10004};

    inner_2.id = 2;
    inner_2.name = "inner_2";
    inner_2.avg_double = 4.25260376469080737;
    inner_2.array_string = std::vector<std::string>{"inner_string_4", "inner_string_5", "inner_string_6"};
    inner_2.array_int = std::vector<int>{20001, 20002, 20003, 20004};

    inner_3.id = 3;
    inner_3.name = "inner_3";
    inner_3.avg_double = 5.36371487570191848;
    inner_3.array_string = std::vector<std::string>{"inner_string_7", "inner_string_8", "inner_string_9"};
    inner_3.array_int = std::vector<int>{30001, 30002, 30003, 30004};

    json_msg.id = 1001;
    json_msg.name = "msg";
    json_msg.avg_double = 6.47482598681202959;
    json_msg.array_string = std::vector<std::string>{"msg_string_1", "msg_string_2", "msg_string_3"};
    json_msg.array_int = std::vector<int>{1, 2, 3};
    json_msg.array_innermsg = std::vector<InnerMsg>{inner_1, inner_2, inner_3};
    json_msg.map_string_string = std::map<std::string, std::string>{{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}};
    json_msg.map_string_int = std::map<std::string, int>{{"key_1", 1}, {"key_2", 2}, {"key_3", 3}};
    json_msg.map_string_innermsg = std::map<std::string, InnerMsg>{{"key_1", inner_1}, {"key_2", inner_2}, {"key_3", inner_3}};
    json_msg.map_int_string = std::map<int, std::string>{{1, "value_1"}, {2, "value_2"}, {3, "value_3"}};
    json_msg.map_int_int = std::map<int, int>{{1, 11}, {2, 22}, {3, 33}};
    json_msg.map_int_innermsg = std::map<int, InnerMsg>{{1, inner_1}, {2, inner_2}, {3, inner_3}};
    json_msg.innermsg = inner_1;

    std::string result;
    AutoJson::Marshal(result, json_msg);
    std::string right_json = R"({"array_innermsg":[{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}],"array_int":[1,2,3],"array_string":["msg_string_1","msg_string_2","msg_string_3"],"avg_double":6.474825986812029,"id":1001,"innermsg":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"map_int_innermsg":{"1":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"2":{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},"3":{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}},"map_int_int":{"1":11,"2":22,"3":33},"map_int_string":{"1":"value_1","2":"value_2","3":"value_3"},"map_string_innermsg":{"key_1":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"key_2":{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},"key_3":{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}},"map_string_int":{"key_1":1,"key_2":2,"key_3":3},"map_string_string":{"key_1":"value_1","key_2":"value_2","key_3":"value_3"},"name":"msg"})";

    EXPECT_EQ(result, right_json);
};

// case2: marshal空结构体
TEST_F(AutoJsonTest, TestMarshal_case2) {
    InnerMsg msg;
    msg.reset();
    std::string result;
    AutoJson::Marshal(result, msg);
    std::string right_json = R"({"innermsg_array_int":null,"innermsg_array_string":null,"innermsg_avg_double":0.0,"innermsg_id":0,"innermsg_name":""})";

    EXPECT_EQ(result, right_json);
}

// case3: 结构体中部分字段无需marshal
TEST_F(AutoJsonTest, TestMarshal_case3) {
    struct Case3Struct : public AutoJsonHelper{
        int id;
        std::string name;
        int id_not_marshal;
        std::string name_not_marshal;

        void SetJsonMapping() {
            AUTO_JSON_MAPPING(id, "id_marshal");
            AUTO_JSON_MAPPING(name, "name_marshal");
        }
    };

    Case3Struct case3_marshal;
    case3_marshal.id = 1001;
    case3_marshal.id_not_marshal = 2002;
    case3_marshal.name = "name_m";
    case3_marshal.name_not_marshal = "name_n_m";

    std::string result;
    AutoJson::Marshal(result, case3_marshal);
    std::string right_json = R"({"id_marshal":1001,"name_marshal":"name_m"})";

    EXPECT_EQ(result, right_json);
}

// case4: 空SetJsonMapping函数
TEST_F(AutoJsonTest, TestMarshal_case4) {
    struct Case4Struct : public AutoJsonHelper{
        int id;
        std::string name;

        void SetJsonMapping() {
        }
    };

    Case4Struct case4_marshal;
    case4_marshal.id = 1001;
    case4_marshal.name = "name_m";

    std::string result;
    AutoJson::Marshal(result, case4_marshal);
    EXPECT_EQ(result, "");
}

// case5: 未继承AutoJsonHelper调用AutoJson::Marshal
TEST_F(AutoJsonTest, TestMarshal_case5) {
    struct Case5Struct {
        int id;
        std::string name;
    };

    Case5Struct case5_marshal;
    case5_marshal.id = 1001;
    case5_marshal.name = "name_m";
    std::string result;
    AutoJson::Marshal(result, case5_marshal);
    EXPECT_EQ(result, "");
}

// case1: 正常格式的unmarshal
TEST_F(AutoJsonTest, TestUnmarshal_case1) {
    std::string json_string = R"({"array_innermsg":[{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}],"array_int":[1,2,3],"array_string":["msg_string_1","msg_string_2","msg_string_3"],"avg_double":6.474825986812029,"id":1001,"innermsg":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"map_int_innermsg":{"1":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"2":{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},"3":{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}},"map_int_int":{"1":11,"2":22,"3":33},"map_int_string":{"1":"value_1","2":"value_2","3":"value_3"},"map_string_innermsg":{"key_1":{"innermsg_array_int":[10001,10002,10003,10004],"innermsg_array_string":["inner_string_1","inner_string_2","inner_string_3"],"innermsg_avg_double":3.141592653589796,"innermsg_id":1,"innermsg_name":"inner_1"},"key_2":{"innermsg_array_int":[20001,20002,20003,20004],"innermsg_array_string":["inner_string_4","inner_string_5","inner_string_6"],"innermsg_avg_double":4.252603764690807,"innermsg_id":2,"innermsg_name":"inner_2"},"key_3":{"innermsg_array_int":[30001,30002,30003,30004],"innermsg_array_string":["inner_string_7","inner_string_8","inner_string_9"],"innermsg_avg_double":5.363714875701919,"innermsg_id":3,"innermsg_name":"inner_3"}},"map_string_int":{"key_1":1,"key_2":2,"key_3":3},"map_string_string":{"key_1":"value_1","key_2":"value_2","key_3":"value_3"},"name":"msg"})";
    JsonMsg result;
    AutoJson::Unmarshal(json_string, result);

    EXPECT_EQ(result.id, 1001);
    EXPECT_EQ(result.name, "msg");
    EXPECT_DOUBLE_EQ(result.avg_double, pai_3);
    ASSERT_EQ(result.array_string.size(), 3);
    EXPECT_EQ(result.array_string[0], "msg_string_1");
    EXPECT_EQ(result.array_string[1], "msg_string_2");
    EXPECT_EQ(result.array_string[2], "msg_string_3");
    ASSERT_EQ(result.array_int.size(), 3);
    EXPECT_EQ(result.array_int[0], 1);
    EXPECT_EQ(result.array_int[1], 2);
    EXPECT_EQ(result.array_int[2], 3);

    ASSERT_EQ(result.array_innermsg.size(), 3);
    EXPECT_EQ(result.array_innermsg[0].id, 1);
    EXPECT_EQ(result.array_innermsg[0].name, "inner_1");
    EXPECT_DOUBLE_EQ(result.array_innermsg[0].avg_double, pai);
    ASSERT_EQ(result.array_innermsg[0].array_string.size(), 3);
    EXPECT_EQ(result.array_innermsg[0].array_string[0], "inner_string_1");
    EXPECT_EQ(result.array_innermsg[0].array_string[1], "inner_string_2");
    EXPECT_EQ(result.array_innermsg[0].array_string[2], "inner_string_3");
    ASSERT_EQ(result.array_innermsg[0].array_int.size(), 4);
    EXPECT_EQ(result.array_innermsg[0].array_int[0], 10001);
    EXPECT_EQ(result.array_innermsg[0].array_int[1], 10002);
    EXPECT_EQ(result.array_innermsg[0].array_int[2], 10003);
    EXPECT_EQ(result.array_innermsg[0].array_int[3], 10004);
    EXPECT_EQ(result.array_innermsg[1].id, 2);
    EXPECT_EQ(result.array_innermsg[1].name, "inner_2");
    EXPECT_DOUBLE_EQ(result.array_innermsg[1].avg_double, pai_1);
    ASSERT_EQ(result.array_innermsg[1].array_string.size(), 3);
    EXPECT_EQ(result.array_innermsg[1].array_string[0], "inner_string_4");
    EXPECT_EQ(result.array_innermsg[1].array_string[1], "inner_string_5");
    EXPECT_EQ(result.array_innermsg[1].array_string[2], "inner_string_6");
    ASSERT_EQ(result.array_innermsg[1].array_int.size(), 4);
    EXPECT_EQ(result.array_innermsg[1].array_int[0], 20001);
    EXPECT_EQ(result.array_innermsg[1].array_int[1], 20002);
    EXPECT_EQ(result.array_innermsg[1].array_int[2], 20003);
    EXPECT_EQ(result.array_innermsg[1].array_int[3], 20004);
    EXPECT_EQ(result.array_innermsg[2].id, 3);
    EXPECT_EQ(result.array_innermsg[2].name, "inner_3");
    EXPECT_DOUBLE_EQ(result.array_innermsg[2].avg_double, pai_2);
    ASSERT_EQ(result.array_innermsg[2].array_string.size(), 3);
    EXPECT_EQ(result.array_innermsg[2].array_string[0], "inner_string_7");
    EXPECT_EQ(result.array_innermsg[2].array_string[1], "inner_string_8");
    EXPECT_EQ(result.array_innermsg[2].array_string[2], "inner_string_9");
    ASSERT_EQ(result.array_innermsg[2].array_int.size(), 4);
    EXPECT_EQ(result.array_innermsg[2].array_int[0], 30001);
    EXPECT_EQ(result.array_innermsg[2].array_int[1], 30002);
    EXPECT_EQ(result.array_innermsg[2].array_int[2], 30003);
    EXPECT_EQ(result.array_innermsg[2].array_int[3], 30004);

    EXPECT_EQ(result.map_string_string.size(), 3);
    ASSERT_EQ(result.map_string_string.count("key_1"), 1);
    EXPECT_EQ(result.map_string_string["key_1"], "value_1");
    ASSERT_EQ(result.map_string_string.count("key_2"), 1);
    EXPECT_EQ(result.map_string_string["key_2"], "value_2");
    ASSERT_EQ(result.map_string_string.count("key_3"), 1);
    EXPECT_EQ(result.map_string_string["key_3"], "value_3");

    EXPECT_EQ(result.map_string_int.size(), 3);
    ASSERT_EQ(result.map_string_int.count("key_1"), 1);
    EXPECT_EQ(result.map_string_int["key_1"], 1);
    ASSERT_EQ(result.map_string_int.count("key_2"), 1);
    EXPECT_EQ(result.map_string_int["key_2"], 2);
    ASSERT_EQ(result.map_string_int.count("key_3"), 1);
    EXPECT_EQ(result.map_string_int["key_3"], 3);

    EXPECT_EQ(result.map_string_innermsg.size(), 3);
    ASSERT_EQ(result.map_string_innermsg.count("key_1"), 1);
    EXPECT_EQ(result.map_string_innermsg["key_1"].id, 1);
    EXPECT_EQ(result.map_string_innermsg["key_1"].name, "inner_1");
    EXPECT_DOUBLE_EQ(result.map_string_innermsg["key_1"].avg_double, pai);
    ASSERT_EQ(result.map_string_innermsg["key_1"].array_string.size(), 3);
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_string[0], "inner_string_1");
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_string[1], "inner_string_2");
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_string[2], "inner_string_3");
    ASSERT_EQ(result.map_string_innermsg["key_1"].array_int.size(), 4);
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_int[0], 10001);
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_int[1], 10002);
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_int[2], 10003);
    EXPECT_EQ(result.map_string_innermsg["key_1"].array_int[3], 10004);
    ASSERT_EQ(result.map_string_innermsg.count("key_2"), 1);
    EXPECT_EQ(result.map_string_innermsg["key_2"].id, 2);
    EXPECT_EQ(result.map_string_innermsg["key_2"].name, "inner_2");
    EXPECT_DOUBLE_EQ(result.map_string_innermsg["key_2"].avg_double, pai_1);
    ASSERT_EQ(result.map_string_innermsg["key_2"].array_string.size(), 3);
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_string[0], "inner_string_4");
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_string[1], "inner_string_5");
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_string[2], "inner_string_6");
    ASSERT_EQ(result.map_string_innermsg["key_2"].array_int.size(), 4);
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_int[0], 20001);
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_int[1], 20002);
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_int[2], 20003);
    EXPECT_EQ(result.map_string_innermsg["key_2"].array_int[3], 20004);
    ASSERT_EQ(result.map_string_innermsg.count("key_3"), 1);
    EXPECT_EQ(result.map_string_innermsg["key_3"].id, 3);
    EXPECT_EQ(result.map_string_innermsg["key_3"].name, "inner_3");
    EXPECT_DOUBLE_EQ(result.map_string_innermsg["key_3"].avg_double, pai_2);
    ASSERT_EQ(result.map_string_innermsg["key_3"].array_string.size(), 3);
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_string[0], "inner_string_7");
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_string[1], "inner_string_8");
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_string[2], "inner_string_9");
    ASSERT_EQ(result.map_string_innermsg["key_3"].array_int.size(), 4);
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_int[0], 30001);
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_int[1], 30002);
    EXPECT_EQ(result.map_string_innermsg["key_3"].array_int[2], 30003);

    EXPECT_EQ(result.map_int_string.size(), 3);
    ASSERT_EQ(result.map_int_string.count(1), 1);
    EXPECT_EQ(result.map_int_string[1], "value_1");
    ASSERT_EQ(result.map_int_string.count(2), 1);
    EXPECT_EQ(result.map_int_string[2], "value_2");
    ASSERT_EQ(result.map_int_string.count(3), 1);
    EXPECT_EQ(result.map_int_string[3], "value_3");

    EXPECT_EQ(result.map_int_int.size(), 3);
    ASSERT_EQ(result.map_int_int.count(1), 1);
    EXPECT_EQ(result.map_int_int[1], 11);
    ASSERT_EQ(result.map_int_int.count(2), 1);
    EXPECT_EQ(result.map_int_int[2], 22);
    ASSERT_EQ(result.map_int_int.count(3), 1);
    EXPECT_EQ(result.map_int_int[3], 33);

    EXPECT_EQ(result.map_int_innermsg.size(), 3);
    ASSERT_EQ(result.map_int_innermsg.count(1), 1);
    EXPECT_EQ(result.map_int_innermsg[1].id, 1);
    EXPECT_EQ(result.map_int_innermsg[1].name, "inner_1");
    EXPECT_DOUBLE_EQ(result.map_int_innermsg[1].avg_double, pai);
    ASSERT_EQ(result.map_int_innermsg[1].array_string.size(), 3);
    EXPECT_EQ(result.map_int_innermsg[1].array_string[0], "inner_string_1");
    EXPECT_EQ(result.map_int_innermsg[1].array_string[1], "inner_string_2");
    EXPECT_EQ(result.map_int_innermsg[1].array_string[2], "inner_string_3");
    ASSERT_EQ(result.map_int_innermsg[1].array_int.size(), 4);
    EXPECT_EQ(result.map_int_innermsg[1].array_int[0], 10001);
    EXPECT_EQ(result.map_int_innermsg[1].array_int[1], 10002);
    EXPECT_EQ(result.map_int_innermsg[1].array_int[2], 10003);
    EXPECT_EQ(result.map_int_innermsg[1].array_int[3], 10004);
    ASSERT_EQ(result.map_int_innermsg.count(2), 1);
    EXPECT_EQ(result.map_int_innermsg[2].id, 2);
    EXPECT_EQ(result.map_int_innermsg[2].name, "inner_2");
    EXPECT_DOUBLE_EQ(result.map_int_innermsg[2].avg_double, pai_1);
    ASSERT_EQ(result.map_int_innermsg[2].array_string.size(), 3);
    EXPECT_EQ(result.map_int_innermsg[2].array_string[0], "inner_string_4");
    EXPECT_EQ(result.map_int_innermsg[2].array_string[1], "inner_string_5");
    EXPECT_EQ(result.map_int_innermsg[2].array_string[2], "inner_string_6");
    ASSERT_EQ(result.map_int_innermsg[2].array_int.size(), 4);
    EXPECT_EQ(result.map_int_innermsg[2].array_int[0], 20001);
    EXPECT_EQ(result.map_int_innermsg[2].array_int[1], 20002);
    EXPECT_EQ(result.map_int_innermsg[2].array_int[2], 20003);
    EXPECT_EQ(result.map_int_innermsg[2].array_int[3], 20004);
    ASSERT_EQ(result.map_int_innermsg.count(3), 1);
    EXPECT_EQ(result.map_int_innermsg[3].id, 3);
    EXPECT_EQ(result.map_int_innermsg[3].name, "inner_3");
    EXPECT_DOUBLE_EQ(result.map_int_innermsg[3].avg_double, pai_2);
    ASSERT_EQ(result.map_int_innermsg[3].array_string.size(), 3);
    EXPECT_EQ(result.map_int_innermsg[3].array_string[0], "inner_string_7");
    EXPECT_EQ(result.map_int_innermsg[3].array_string[1], "inner_string_8");
    EXPECT_EQ(result.map_int_innermsg[3].array_string[2], "inner_string_9");
    ASSERT_EQ(result.map_int_innermsg[3].array_int.size(), 4);
    EXPECT_EQ(result.map_int_innermsg[3].array_int[0], 30001);
    EXPECT_EQ(result.map_int_innermsg[3].array_int[1], 30002);
    EXPECT_EQ(result.map_int_innermsg[3].array_int[2], 30003);

    EXPECT_EQ(result.innermsg.id, 1);
    EXPECT_EQ(result.innermsg.name, "inner_1");
    EXPECT_DOUBLE_EQ(result.innermsg.avg_double, pai);
    ASSERT_EQ(result.innermsg.array_string.size(), 3);
    EXPECT_EQ(result.innermsg.array_string[0], "inner_string_1");
    EXPECT_EQ(result.innermsg.array_string[1], "inner_string_2");
    EXPECT_EQ(result.innermsg.array_string[2], "inner_string_3");
    ASSERT_EQ(result.innermsg.array_int.size(), 4);
    EXPECT_EQ(result.innermsg.array_int[0], 10001);
    EXPECT_EQ(result.innermsg.array_int[1], 10002);
    EXPECT_EQ(result.innermsg.array_int[2], 10003);
    EXPECT_EQ(result.innermsg.array_int[3], 10004);
};

// case2: json中字段全部为默认值
TEST_F(AutoJsonTest, TestUnmarshal_case2) {
    std::string json_string = R"({"innermsg_array_int":[],"innermsg_array_string":[],"innermsg_avg_double":0.0,"innermsg_id":0,"innermsg_name":""})";
    InnerMsg result;
    result.id = 1001;
    result.name = "inner_msg";
    result.avg_double = pai;
    result.array_string = std::vector<std::string>{"inner_1", "inner_2"};
    result.array_int = std::vector<int>{1, 2, 3};

    AutoJson::Unmarshal(json_string, result);

    EXPECT_EQ(result.id, 0);
    EXPECT_EQ(result.name, "");
    EXPECT_DOUBLE_EQ(result.avg_double, 0);
    EXPECT_EQ(result.array_string.size(), 0);
    EXPECT_EQ(result.array_int.size(), 0);
}

// case3: json空串
TEST_F(AutoJsonTest, TestUnmarshal_case3) {
    std::string json_string_1 = R"()";
    InnerMsg result_1;
    result_1.id = 1001;
    result_1.name = "inner_msg";
    result_1.avg_double = pai;
    result_1.array_string = std::vector<std::string>{"inner_1", "inner_2"};
    result_1.array_int = std::vector<int>{1, 2, 3};

    AutoJson::Unmarshal(json_string_1, result_1);
    EXPECT_EQ(result_1.id, 1001);
    EXPECT_EQ(result_1.name, "inner_msg");
    EXPECT_DOUBLE_EQ(result_1.avg_double, pai);
    ASSERT_EQ(result_1.array_string.size(), 2);
    EXPECT_EQ(result_1.array_string[0], "inner_1");
    EXPECT_EQ(result_1.array_string[1], "inner_2");
    ASSERT_EQ(result_1.array_int.size(), 3);
    EXPECT_EQ(result_1.array_int[0], 1);
    EXPECT_EQ(result_1.array_int[1], 2);
    EXPECT_EQ(result_1.array_int[2], 3);

    std::string json_string_2 = R"({})";
    InnerMsg result_2;
    result_2.id = 1001;
    result_2.name = "inner_msg";
    result_2.avg_double = pai;
    result_2.array_string = std::vector<std::string>{"inner_1", "inner_2"};
    result_2.array_int = std::vector<int>{1, 2, 3};
    AutoJson::Unmarshal(json_string_2, result_2);
    EXPECT_EQ(result_2.id, 1001);
    EXPECT_EQ(result_2.name, "inner_msg");
    EXPECT_DOUBLE_EQ(result_2.avg_double, pai);
    ASSERT_EQ(result_2.array_string.size(), 2);
    EXPECT_EQ(result_2.array_string[0], "inner_1");
    EXPECT_EQ(result_2.array_string[1], "inner_2");
    ASSERT_EQ(result_2.array_int.size(), 3);
    EXPECT_EQ(result_2.array_int[0], 1);
    EXPECT_EQ(result_2.array_int[1], 2);
    EXPECT_EQ(result_2.array_int[2], 3);

    std::string json_string_3 = R"({""})";
    InnerMsg result_3;
    result_3.id = 1001;
    result_3.name = "inner_msg";
    result_3.avg_double = pai;
    result_3.array_string = std::vector<std::string>{"inner_1", "inner_2"};
    result_3.array_int = std::vector<int>{1, 2, 3};
    AutoJson::Unmarshal(json_string_3, result_3);
    EXPECT_EQ(result_3.id, 1001);
    EXPECT_EQ(result_3.name, "inner_msg");
    EXPECT_DOUBLE_EQ(result_3.avg_double, pai);
    ASSERT_EQ(result_3.array_string.size(), 2);
    EXPECT_EQ(result_3.array_string[0], "inner_1");
    EXPECT_EQ(result_3.array_string[1], "inner_2");
    ASSERT_EQ(result_3.array_int.size(), 3);
    EXPECT_EQ(result_3.array_int[0], 1);
    EXPECT_EQ(result_3.array_int[1], 2);
    EXPECT_EQ(result_3.array_int[2], 3);
}

// case4: 结构体中部分字段无需unmarshal
TEST_F(AutoJsonTest, TestUnmarshal_case4) {
    struct Case4Struct : public AutoJsonHelper{
        int id;
        std::string name;
        int id_not_unmarshal;
        std::string name_not_unmarshal;

        void SetJsonMapping() {
            AUTO_JSON_MAPPING(id, "id");
            AUTO_JSON_MAPPING(name, "name");
        }
    };

    std::string json_string = R"({"id":1001,"name":"name_m","id_not_unmarshal": 2002,"name_not_unmarshal":"name_n_m"})";
    Case4Struct result;
    result.id_not_unmarshal = 3003;
    result.name_not_unmarshal = "name_origin";
    AutoJson::Unmarshal(json_string, result);

    EXPECT_EQ(result.id, 1001);
    EXPECT_EQ(result.name, "name_m");
    EXPECT_EQ(result.id_not_unmarshal, 3003);
    EXPECT_EQ(result.name_not_unmarshal, "name_origin");
}

// case5: 空SetJsonMapping函数
TEST_F(AutoJsonTest, TestUnmarshal_case5) {
    struct Case5Struct : public AutoJsonHelper{
        int id;
        std::string name;

        void SetJsonMapping() {
        }
    };

    std::string json_string = R"({"id":2002,"name":"name_um"})";
    Case5Struct result;
    result.id = 1001;
    result.name = "name_m";
    AutoJson::Unmarshal(json_string, result);

    EXPECT_EQ(result.id, 1001);
    EXPECT_EQ(result.name, "name_m");
}

// case6: 未继承AutoJsonHelper调用AutoJson::Unmarshal
TEST_F(AutoJsonTest, TestUnmarshal_case6) {
    struct Case6Struct {
        int id;
        std::string name;
    };

    std::string json_string = R"({"id":2002,"name":"name_um"})";
    Case6Struct result;
    result.id = 1001;
    result.name = "name_m";
    AutoJson::Unmarshal(json_string, result);

    EXPECT_EQ(result.id, 1001);
    EXPECT_EQ(result.name, "name_m");
}

// case7: json串中key对应的value类型非结构体中的类型
TEST_F(AutoJsonTest, TestUnmarshal_case7) {
    InnerMsg result;
    result.id = 1001;
    result.name = "inner_msg";
    result.avg_double = pai;
    result.array_string = std::vector<std::string>{"inner_1", "inner_2"};
    result.array_int = std::vector<int>{1, 2, 3};

    std::string json_string = R"({"innermsg_array_int":{"key_2":"value_2"},"innermsg_array_string":"no","innermsg_avg_double":[1,2,3],"innermsg_id":"msg","innermsg_name":123})";
    AutoJson::Unmarshal(json_string, result);
    EXPECT_EQ(result.id, 1001);
    EXPECT_EQ(result.name, "inner_msg");
    EXPECT_DOUBLE_EQ(result.avg_double, pai);
    ASSERT_EQ(result.array_string.size(), 2);
    EXPECT_EQ(result.array_string[0], "inner_1");
    EXPECT_EQ(result.array_string[1], "inner_2");
    ASSERT_EQ(result.array_int.size(), 3);
    EXPECT_EQ(result.array_int[0], 1);
    EXPECT_EQ(result.array_int[1], 2);
    EXPECT_EQ(result.array_int[2], 3);
}