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
    ResetArgs("-don");
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", false));
    BOOST_CHECK(GetBoolArg("-don", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-dono"));
    BOOST_CHECK(!GetBoolArg("-dono", false));
    BOOST_CHECK(GetBoolArg("-dono", true));

    ResetArgs("-don=0");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", false));
    BOOST_CHECK(!GetBoolArg("-don", true));

    ResetArgs("-don=1");
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", false));
    BOOST_CHECK(GetBoolArg("-don", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nodon");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", false));
    BOOST_CHECK(!GetBoolArg("-don", true));

    ResetArgs("-nodon=1");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", false));
    BOOST_CHECK(!GetBoolArg("-don", true));

    ResetArgs("-don -nodon");  // -don should win
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", false));
    BOOST_CHECK(GetBoolArg("-don", true));

    ResetArgs("-don=1 -nodon=1");  // -don should win
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", false));
    BOOST_CHECK(GetBoolArg("-don", true));

    ResetArgs("-don=0 -nodon=0");  // -don should win
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", false));
    BOOST_CHECK(!GetBoolArg("-don", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--don=1");
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", false));
    BOOST_CHECK(GetBoolArg("-don", true));

    ResetArgs("--nodon=1");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", false));
    BOOST_CHECK(!GetBoolArg("-don", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-don", "eleven"), "eleven");

    ResetArgs("-don -bar");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-don", "eleven"), "");

    ResetArgs("-don=");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-don", "eleven"), "");

    ResetArgs("-don=11");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-don", "eleven"), "11");

    ResetArgs("-don=eleven");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-don", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-don", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-don", 0), 0);

    ResetArgs("-don -bar");
    BOOST_CHECK_EQUAL(GetArg("-don", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-don=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-don", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-don=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-don", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--don");
    BOOST_CHECK_EQUAL(GetBoolArg("-don"), true);

    ResetArgs("--don=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-don", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nodon");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", true));
    BOOST_CHECK(!GetBoolArg("-don", false));

    ResetArgs("-nodon=1");
    BOOST_CHECK(!GetBoolArg("-don"));
    BOOST_CHECK(!GetBoolArg("-don", true));
    BOOST_CHECK(!GetBoolArg("-don", false));

    ResetArgs("-nodon=0");
    BOOST_CHECK(GetBoolArg("-don"));
    BOOST_CHECK(GetBoolArg("-don", true));
    BOOST_CHECK(GetBoolArg("-don", false));

    ResetArgs("-don --nodon");
    BOOST_CHECK(GetBoolArg("-don"));

    ResetArgs("-nodon -don"); // don always wins:
    BOOST_CHECK(GetBoolArg("-don"));
}

BOOST_AUTO_TEST_SUITE_END()
