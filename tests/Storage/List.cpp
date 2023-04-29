#include <Expect>
#include <CityBuilder/Storage/List.h>

SUITE(List) {
  TEST(single-remove, "Test removing an element from a length 1 list.") {
    List<int> list { 3 };
    
    list.remove(0);
    
    EXPECT list.isEmpty();
    EXPECT list.count() == 0;
  };
  
  TEST(index-out-of-bounds, "Check accessing an out-of-bounds index.") {
    List<int> list { 3 };
    
    EXPECT_EXCEPTION(IndexOutOfBounds) { list[1]; };
    EXPECT_EXCEPTION(IndexOutOfBounds) { list[-1]; };
  };
  
  TEST(remove, "Test removing an element from a list.") {
    List<int> list { 3, 4, 5 };
    
    list.remove(1);
    
    EXPECT list.count() == 2;
    EXPECT list[0] == 3;
    EXPECT list[1] == 5;
  };
  
  TEST(append, "Test appending an element to a list.") {
    List<int> list { 3, 4, 5 };
    
    list.append(6);
    
    EXPECT list.count() == 4;
    EXPECT list[0] == 3;
    EXPECT list[1] == 4;
    EXPECT list[2] == 5;
    EXPECT list[3] == 6;
  };
  
  TEST(prepend, "Test prepending an element to a list.") {
    List<int> list { 3, 4, 5 };
    
    list.insert(2, 0);
    
    EXPECT list.count() == 4;
    EXPECT list[0] == 2;
    EXPECT list[1] == 3;
    EXPECT list[2] == 4;
    EXPECT list[3] == 5;
  };
}
