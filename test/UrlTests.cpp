#include <requests/Url.hpp>

#include <gtest/gtest.h>
#include <unordered_map>

class UrlTest : public testing::Test
{
protected:    
    UrlTest()
        : url1_("https://localhost:5000/file/123?name=hello&good=true")
    {
        std::unordered_map<std::string, std::string> queries =
        {
            { "greet", "hello" }
        };
        
        url1_.addQueries(queries);        
    }
    
    requests::Url url1_;
};

TEST_F(UrlTest, Schema)
{
    EXPECT_EQ("https", url1_.schema());
}

TEST_F(UrlTest, Host)
{
    EXPECT_EQ("localhost", url1_.host());    
}

TEST_F(UrlTest, Port)
{
    EXPECT_EQ("5000", url1_.port());    
}

TEST_F(UrlTest, Path)
{
    EXPECT_EQ("/file/123", url1_.path());            
}        

TEST_F(UrlTest, HasQueries)
{
    EXPECT_EQ(true, url1_.hasQueries());        
}        

TEST_F(UrlTest, Queries)
{
    EXPECT_EQ("name=hello&good=true&greet=hello", url1_.queries());    
}        

TEST_F(UrlTest, PathAndQueries)
{
    EXPECT_EQ("/file/123?name=hello&good=true&greet=hello", url1_.pathAndQueries());        
}        
    
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
