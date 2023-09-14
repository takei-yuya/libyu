#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "yu/json.hpp"

int main() {
  class Klass {
   public:
    Klass() : str(), num(), map(), vec() {}
   private:
    std::string str;
    int num;
    std::map<std::string, float> map;
    std::vector<bool> vec;
   public:
    void stringifyJson(yu::json::Stringifier& stringifier) const {
      JSON_MEMBER_STRINGIFIER(stringifier)
        << JSON_GETTER(str) << JSON_GETTER(num)
        << JSON_GETTER(map) << JSON_GETTER(vec)
        << JSON_STRINGIFY;
    }
    void parseJson(yu::json::Parser& parser) {
      JSON_MEMBER_PARSER(parser)
        << JSON_SETTER(str) << JSON_SETTER(num)
        << JSON_SETTER(map) << JSON_SETTER(vec)
        << JSON_PARSE;
    }
  };
  std::string json = R"(
    {
      "str": "Hello World.", "num": 42,
      "map": { "a": 3.14 }, "vec": [ true, false ]
    }
  )";
  Klass obj = yu::json::from_json<Klass>(json);
  std::cout << yu::json::to_json(obj) << std::endl;
}

