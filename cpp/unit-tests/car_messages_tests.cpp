#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include "../string-utils.h"
#include "../../teensy/CarMessages.h"

using namespace std;


TEST(SimpleMessage, transfer) {
    SimpleMessage in, out;
    in.string_value = "hello";
    in.int_value = 42;
    in.float_value = 3.1415;
    in.char_value = 'p';
    StringOutTransfer t;
    in.transfer(t);
    cout << t.str() << endl;

    float tolerance = 1E-6;

    StringInTransfer t2(t.str().c_str());
    out.transfer(t2);
    EXPECT_EQ(in.string_value, out.string_value);
    EXPECT_EQ(in.int_value, out.int_value);
    EXPECT_EQ(in.char_value, out.char_value);
    EXPECT_NEAR(3.14, out.float_value, tolerance); // fixed digits to 2
}


TEST(Dynamics2, defaults_zero) {
    Dynamics2 d;
    EXPECT_EQ(d.odo_fl_a_us, 0);
    EXPECT_EQ(d.go, false);
}

TEST(Dynamics2, transfer) {
    Dynamics2 in, out;
    StringOutTransfer t;
    in.transfer(t);
    cout << t.str() << endl;

    StringInTransfer t2(t.str().c_str());
    out.transfer(t2);
}

TEST(SimpleMessage, not_enough_fields) {
    StringInTransfer t("42");
    SimpleMessage in;
    in.transfer(t);
    EXPECT_EQ(false, t.ok);
    EXPECT_TRUE(t.error_message.length() > 0);
    cout << "expected error: " << t.error_message << endl;
}

TEST(SimpleMessage, too_many_fields) {
    SimpleMessage td,td2;
    StringOutTransfer t_out;
    td.transfer(t_out);
    cout << "transferred: " << t_out.str() << endl;
    NativeString too_many_fields = t_out.str() + ",extra";
    StringInTransfer t_in(too_many_fields.c_str());
    td2.transfer(t_in);
    EXPECT_EQ(false, t_in.ok);
    cout << "expected error: " << t_in.error_message << endl;
}
