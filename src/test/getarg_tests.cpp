#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-NTC");
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", false));
    BOOST_CHECK(GetBoolArg("-NTC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-NTCo"));
    BOOST_CHECK(!GetBoolArg("-NTCo", false));
    BOOST_CHECK(GetBoolArg("-NTCo", true));

    ResetArgs("-NTC=0");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", false));
    BOOST_CHECK(!GetBoolArg("-NTC", true));

    ResetArgs("-NTC=1");
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", false));
    BOOST_CHECK(GetBoolArg("-NTC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noNTC");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", false));
    BOOST_CHECK(!GetBoolArg("-NTC", true));

    ResetArgs("-noNTC=1");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", false));
    BOOST_CHECK(!GetBoolArg("-NTC", true));

    ResetArgs("-NTC -noNTC");  // -NTC should win
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", false));
    BOOST_CHECK(GetBoolArg("-NTC", true));

    ResetArgs("-NTC=1 -noNTC=1");  // -NTC should win
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", false));
    BOOST_CHECK(GetBoolArg("-NTC", true));

    ResetArgs("-NTC=0 -noNTC=0");  // -NTC should win
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", false));
    BOOST_CHECK(!GetBoolArg("-NTC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--NTC=1");
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", false));
    BOOST_CHECK(GetBoolArg("-NTC", true));

    ResetArgs("--noNTC=1");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", false));
    BOOST_CHECK(!GetBoolArg("-NTC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NTC", "eleven"), "eleven");

    ResetArgs("-NTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NTC", "eleven"), "");

    ResetArgs("-NTC=");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NTC", "eleven"), "");

    ResetArgs("-NTC=11");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-NTC", "eleven"), "11");

    ResetArgs("-NTC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-NTC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NTC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-NTC", 0), 0);

    ResetArgs("-NTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-NTC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-NTC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-NTC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-NTC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-NTC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--NTC");
    BOOST_CHECK_EQUAL(GetBoolArg("-NTC"), true);

    ResetArgs("--NTC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-NTC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noNTC");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", true));
    BOOST_CHECK(!GetBoolArg("-NTC", false));

    ResetArgs("-noNTC=1");
    BOOST_CHECK(!GetBoolArg("-NTC"));
    BOOST_CHECK(!GetBoolArg("-NTC", true));
    BOOST_CHECK(!GetBoolArg("-NTC", false));

    ResetArgs("-noNTC=0");
    BOOST_CHECK(GetBoolArg("-NTC"));
    BOOST_CHECK(GetBoolArg("-NTC", true));
    BOOST_CHECK(GetBoolArg("-NTC", false));

    ResetArgs("-NTC --noNTC");
    BOOST_CHECK(GetBoolArg("-NTC"));

    ResetArgs("-noNTC -NTC"); // NTC always wins:
    BOOST_CHECK(GetBoolArg("-NTC"));
}

BOOST_AUTO_TEST_SUITE_END()
