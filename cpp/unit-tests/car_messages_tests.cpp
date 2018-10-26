#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include "../../teensy/CarMessages.h"

using namespace std;


TEST(SimpleMessage, transfer) {
    SimpleMessage in, out;
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


TEST(TraceDynamics, transfer) {
    TraceDynamics in, out;
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
