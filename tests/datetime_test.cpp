#include "datetime.hpp"
#include "log.hpp"

int main() {
    DateTime dt1(2024, 12, 23);
    printx("dt1:", dt1.ToString());

    DateTime dt2 = DateTime("2024-12-07 00:17:59");
    printx("dt1 < dt2: ", (dt1 < dt2));

    DateTime dt3(2024, 12, 23);
    printx("dt1 <= dt3: ", (dt1 <= dt3));

    auto r1 = TimeRange::FromDdHhMmSs(10);
    dt3 += r1;
    printx("+10d: ", dt3.ToString());

    auto r2 = TimeRange::FromHhMmSs(2);
    dt3 += r2;
    printx("+2h: ", dt3.ToString());

    auto r3 = TimeRange::FromMmSs(70);
    dt3 += r3;
    printx("+70min: ", dt3.ToString());

    return 0;
}