#include "Map.hpp"
#include "Set.hpp"
#include <iostream>

int main() {
  Set set = {1, 2, 3};
  for (auto&& e : set)
    std::cout << e << '\n';
  auto ret = set.insert(4);
  std::cout << std::boolalpha << *ret.first << ' ' << ret.second << std::endl;
  MultiSet<std::string> mset = {"Hola"};
  auto mret = mset.insert("Adios");
  mset.insert("Hola");
  mset.insert("Union");
  for (auto it = mset.rbegin(); it != mset.rend(); it++)
    std::cout << *it << ' ';
  std::cout << std::endl;
  Map<std::string, int> map;
  map["Uno"]++;
  map["Dos"] = 2;
  map["Tres"] = 3;
  map["Cuatro"] = 4;
  for (auto&& [key, value] : map)
    std::cout << key << " : " << value << '\n';
}
