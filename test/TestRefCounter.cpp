import <gtest/gtest.h>

import "RefCounter.h"


TEST(RefCounterTest, OneThusand) {

}



int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}