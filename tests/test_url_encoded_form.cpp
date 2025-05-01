#include <hot_teacup/header_view.h>
#include <hot_teacup/url_encoded_form.h>
#include <hot_teacup/url_encoded_form_view.h>
#include <gtest/gtest.h>

TEST(UrlEncodedForm, toString)
{
    const auto expectedUrlEncodedFormData = "param1=foo&param2=bar ";

    auto form = http::UrlEncodedForm{{
            {"param1", "foo"},
            {"param2", "bar "},
    }};

    EXPECT_EQ(form.toString(), expectedUrlEncodedFormData);
}

TEST(UrlEncodedForm, WithEmptyParamToString)
{
    const auto expectedUrlEncodedFormData = "param1=&param2=bar &param3=";
    auto form = http::UrlEncodedForm{{
            {"param1", ""},
            {"param2", "bar "},
            {"param3", ""},
    }};
    EXPECT_EQ(form.toString(), expectedUrlEncodedFormData);
}

TEST(UrlEncodedForm, UrlEncodedFromString)
{
    const auto formData = "param1=foo&param2=bar&param2=baz&flag&param4=";

    const auto formResult = http::urlEncodedFormFromString(formData);
    ASSERT_TRUE(formResult.has_value());

    const auto& form = formResult.value();
    ASSERT_EQ(form.paramsCount(), 4);
    EXPECT_EQ(form.paramCount("param1"), 1);
    EXPECT_EQ(form.param("param1"), "foo");
    EXPECT_EQ(form.paramCount("param2"), 2);
    EXPECT_TRUE(form.param("param2", 0) == "bar");
    EXPECT_EQ(form.param("param2", 1), "baz");
    EXPECT_EQ(form.paramCount("param4"), 1);
    EXPECT_EQ(form.param("param4"), "");
}

TEST(UrlEncodedForm, FormFromUrlEncodedFormView)
{
    const auto formData = "param1=foo&param2=bar&flag&param4=";

    const auto formResult = http::urlEncodedFormFromString(formData);
    ASSERT_TRUE(formResult.has_value());

    const auto form = http::UrlEncodedForm(formResult.value());
    ASSERT_EQ(form.paramsCount(), 3);
    EXPECT_EQ(form.paramCount("param1"), 1);
    EXPECT_EQ(form.param("param1"), "foo");
    EXPECT_EQ(form.paramCount("param2"), 1);
    EXPECT_EQ(form.param("param2"), "bar");
    EXPECT_EQ(form.paramCount("param4"), 1);
    EXPECT_EQ(form.param("param4"), "");
}

TEST(UrlEncodedForm, UrlEncodedWithoutNameFromString)
{
    {
        const auto formData = "=";
        const auto formResult = http::urlEncodedFormFromString(formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto formData = "";
        const auto formResult = http::urlEncodedFormFromString(formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto formData = "  \t =foo";
        const auto formResult = http::urlEncodedFormFromString(formData);
        ASSERT_FALSE(formResult.has_value());
    }
    {
        const auto formData = "  \t =foo&param2=bar";
        const auto formResult = http::urlEncodedFormFromString(formData);
        ASSERT_TRUE(formResult.has_value());

        const auto& form = formResult.value();
        ASSERT_EQ(form.paramsCount(), 1);
        EXPECT_EQ(form.paramCount("param2"), 1);
        EXPECT_EQ(form.param("param2"), "bar");
    }
}