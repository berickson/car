#include <gtest/gtest.h>
#include "../string-utils.h"

TEST(split, three_fields) {
    string s = "a,bb,c";
    auto v = split(s);
    EXPECT_EQ(v[0],"a");
    EXPECT_EQ(v[1],"bb");
    EXPECT_EQ(v[2],"c");
    EXPECT_EQ(v.size(),3);
}

TEST(split, not_all) {
    string s = "a,bb,c";
    auto v = split(s,',',false);
    EXPECT_EQ(v[0],"a");
    EXPECT_EQ(v[1],"bb,c");
    EXPECT_EQ(v.size(),2);
}
