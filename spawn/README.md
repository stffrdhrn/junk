# Spawn and Capture test

This is a test copied from glibc and updated to run independent of the glibc
test driver.  The test was failing on the OpenRISC port when writes to stderr
increased to 20,000 bytes.

Splitting the test out like this makes it easier to examine.
