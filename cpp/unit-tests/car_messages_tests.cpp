#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include "../../teensy/CarMessages.h"

using namespace std;

TEST(TraceDynamics, transfer) {
    TraceDynamics in, out;
    in.label = "hello";
    in.number = 42;
    StringOutTransfer t;
    in.transfer(t);
    cout << t.str() << endl;

    StringInTransfer t2(t.str().c_str());
    out.transfer(t2);
    EXPECT_EQ(in.number, out.number);
    EXPECT_EQ(in.label, out.label);

    cout << "Label was: " << out.label << endl;
    cout << "number was: " << out.number << endl;
}

TEST(TraceDynamics, not_enough_fields) {
    bool caught = false;
    try {
        StringInTransfer t("42");
        TraceDynamics in;
        in.transfer(t);
    } catch (NativeString error) {
        cout << "caught expected error: " << error << endl;
        caught = true;
    }
    EXPECT_TRUE(caught);
}

TEST(TraceDynamics, too_many_fields) {
    bool caught = false;
    try {
        TraceDynamics td,td2;
        StringOutTransfer t_out;
        td.transfer(t_out);
        cout << "transferred: " << t_out.str() << endl;
        NativeString too_many_fields = t_out.str() + ",extra";
        StringInTransfer t_in(too_many_fields.c_str());
        td2.transfer(t_in);
    } catch (NativeString error) {
        cout << "caught expected error: " << error << endl;
        caught = true;
    }
    EXPECT_TRUE(caught);
}
