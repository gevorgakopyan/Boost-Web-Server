
#include "boost/filesystem.hpp"
#include <boost/asio/buffers_iterator.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request_api_handler.h"
class APIHandlerFixture : public ::testing::Test
{
public:
    APIHandlerFixture() {
        root = "../test_crud";
        base_uri = "/api";

        boost::filesystem::path root_path(root);
        boost::filesystem::remove_all(root_path);
        if (!boost::filesystem::exists(root_path))
        {
            boost::filesystem::create_directory(root_path);
        }
    }
protected:
    std::string root;
    std::string base_uri;
    std::map<std::string, std::vector<int>> path_counts;
    user_profile profile = {0, "", "", true};
};
// Bad request if the method is not GET, PUT, POST, DELETE
TEST_F(APIHandlerFixture, BadRequest) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/Shoes");
    request.method(bhttp::verb::patch);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::bad_request, status);
}

TEST_F(APIHandlerFixture, InvalidNoEntity) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist
    boost::filesystem::remove_all(dir_path);

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/");
    request.method(bhttp::verb::post);
    boost::beast::ostream(request.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);
    EXPECT_EQ(path_counts.size(), 0);
}

TEST_F(APIHandlerFixture, InvalidBadURI) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/skdjfnksdj/");
    request.method(bhttp::verb::post);
    boost::beast::ostream(request.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);
    EXPECT_EQ(path_counts.size(), 0);
}

TEST_F(APIHandlerFixture, PostInvalidDirectory) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/Shoes/Books");
    request.method(bhttp::verb::post);
    boost::beast::ostream(request.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);
    EXPECT_EQ(path_counts.size(), 0);
}

TEST_F(APIHandlerFixture, PostRequest) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/Shoes");
    request.method(bhttp::verb::post);
    boost::beast::ostream(request.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);
    std::string body1 = boost::beast::buffers_to_string(response.body().data());

    bhttp::request<bhttp::dynamic_body> request2;
    request2.target("/api/Shoes");
    request2.method(bhttp::verb::post);
    boost::beast::ostream(request2.body()) << "\'{\"name\":\"Jeff\", \"age\":32, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response2;
    request_api_handler api_handler2(base_uri, root, request2.target().to_string(), path_counts, profile);
    bhttp::status status2 = api_handler2.handle_request(request2, response2);
    std::string body2 = boost::beast::buffers_to_string(response2.body().data());

    bhttp::request<bhttp::dynamic_body> request3;
    request3.target("/api/Books");
    request3.method(bhttp::verb::post);
    boost::beast::ostream(request3.body()) << "\'{\"name\":\"Jessie\", \"age\":45, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response3;
    request_api_handler api_handler3(base_uri, root, request3.target().to_string(), path_counts, profile);
    bhttp::status status3 = api_handler3.handle_request(request3, response3);
    std::string body3 = boost::beast::buffers_to_string(response3.body().data());

    EXPECT_EQ(bhttp::status::created, status1);
    EXPECT_EQ(bhttp::status::created, status2);
    EXPECT_EQ(bhttp::status::created, status3);

    std::stringstream buffer;
    std::stringstream buffer2;
    std::stringstream buffer3;
    std::ifstream t(root + "/Shoes/1");
    std::ifstream t2(root + "/Shoes/2");
    std::ifstream t3(root + "/Books/1");
    buffer << t.rdbuf();
    buffer2 << t2.rdbuf();
    buffer3 << t3.rdbuf();
    EXPECT_EQ(buffer.str(), "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'");
    EXPECT_EQ(buffer2.str(), "\'{\"name\":\"Jeff\", \"age\":32, \"car\":null}\'");
    EXPECT_EQ(buffer3.str(), "\'{\"name\":\"Jessie\", \"age\":45, \"car\":null}\'");

    std::string rep1 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Shoes with ID 1</h1></body>"
        "</html>";
    std::string rep2 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Shoes with ID 2</h1></body>"
        "</html>";
    std::string rep3 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Books with ID 1</h1></body>"
        "</html>";
    EXPECT_EQ(body1, rep1);
    EXPECT_EQ(body2, rep2);
    EXPECT_EQ(body3, rep3);

    boost::filesystem::remove_all(root + "/Shoes");
    boost::filesystem::remove_all(root + "/Books");
}

TEST_F(APIHandlerFixture, PostWithDeleteds) {
    std::vector<int> shoes_ids = { 3 };
    std::vector<int> books_ids = { 1,2,4 };
    path_counts["Shoes"] = shoes_ids;
    path_counts["Books"] = books_ids;

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/Shoes");
    request.method(bhttp::verb::post);
    boost::beast::ostream(request.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);
    std::string body1 = boost::beast::buffers_to_string(response.body().data());

    bhttp::request<bhttp::dynamic_body> request2;
    request2.target("/api/Books");
    request2.method(bhttp::verb::post);
    boost::beast::ostream(request2.body()) << "\'{\"name\":\"Jeff\", \"age\":32, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response2;
    request_api_handler api_handler2(base_uri, root, request2.target().to_string(), path_counts, profile);
    bhttp::status status2 = api_handler2.handle_request(request2, response2);
    std::string body2 = boost::beast::buffers_to_string(response2.body().data());

    bhttp::request<bhttp::dynamic_body> request3;
    request3.target("/api/Books");
    request3.method(bhttp::verb::post);
    boost::beast::ostream(request3.body()) << "\'{\"name\":\"Jessie\", \"age\":45, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response3;
    request_api_handler api_handler3(base_uri, root, request3.target().to_string(), path_counts, profile);
    bhttp::status status3 = api_handler3.handle_request(request3, response3);
    std::string body3 = boost::beast::buffers_to_string(response3.body().data());

    EXPECT_EQ(bhttp::status::created, status1);
    EXPECT_EQ(bhttp::status::created, status2);
    EXPECT_EQ(bhttp::status::created, status3);

    std::stringstream buffer;
    std::stringstream buffer2;
    std::stringstream buffer3;
    std::ifstream t(root + "/Shoes/1");
    std::ifstream t2(root + "/Books/3");
    std::ifstream t3(root + "/Books/5");
    buffer << t.rdbuf();
    buffer2 << t2.rdbuf();
    buffer3 << t3.rdbuf();
    EXPECT_EQ(buffer.str(), "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'");
    EXPECT_EQ(buffer2.str(), "\'{\"name\":\"Jeff\", \"age\":32, \"car\":null}\'");
    EXPECT_EQ(buffer3.str(), "\'{\"name\":\"Jessie\", \"age\":45, \"car\":null}\'");

    std::string rep1 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Shoes with ID 1</h1></body>"
        "</html>";
    std::string rep2 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Books with ID 3</h1></body>"
        "</html>";
    std::string rep3 = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created file at " + root + "/Books with ID 5</h1></body>"
        "</html>";
    EXPECT_EQ(body1, rep1);
    EXPECT_EQ(body2, rep2);
    EXPECT_EQ(body3, rep3);

    boost::filesystem::remove_all(root + "/Shoes");
    boost::filesystem::remove_all(root + "/Books");
}

TEST_F(APIHandlerFixture, PostWithExistingFiles) {
    boost::filesystem::create_directory(root + "/Shoes");
    std::ostringstream oss;
    oss << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    std::string body = oss.str();
    oss.clear();
    std::ofstream file(root + "/Shoes/2");
    file << body;
    file.close();

    std::ostringstream oss2;
    oss2 << "\'{\"name\":\"Jeff\", \"age\":32, \"car\":null}\'";
    body = oss2.str();
    oss2.clear();
    std::ofstream file2(root + "/Shoes/1");
    file2 << body;
    file2.close();

    std::ostringstream oss3;
    oss3 << "\'{\"name\":\"Jessie\", \"age\":45, \"car\":null}\'";
    body = oss3.str();
    oss3.clear();
    std::ofstream file3(root + "/Shoes/3");
    file3 << body;
    file3.close();

    request_api_handler api_handler(base_uri, root, "/api/Shoes", path_counts, profile);

    std::vector<int> ids = { 1,2,3 };
    EXPECT_THAT(path_counts["Shoes"], testing::UnorderedElementsAreArray(ids));

    boost::filesystem::remove_all(root + "/Shoes");
}

TEST_F(APIHandlerFixture, DeleteInvalidPath) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/invalid");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);
}

TEST_F(APIHandlerFixture, DeleteNotInPathCounts) {
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/invalid/1");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);
}
TEST_F(APIHandlerFixture, DeleteOnDirectory) {
    bhttp::request<bhttp::dynamic_body> request;
    boost::filesystem::path dir_path(root + "/test");
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::path final_path(root + "/test" + "/dir");
    boost::filesystem::create_directory(final_path);

    request.target("/api/test/dir");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);

    // cleanup 
    boost::filesystem::remove_all(dir_path);
}

TEST_F(APIHandlerFixture, DeleteIDNotInt) {
    bhttp::request<bhttp::dynamic_body> request;
    boost::filesystem::path dir_path(root + "/unit_test");
    boost::filesystem::create_directory(dir_path);
    // cleanup first in case files exist 
    boost::filesystem::remove_all(dir_path);

    // Create file: code from https://stackoverflow.com/questions/30029343/how-do-i-create-a-file-with-boost-filesystem-without-opening-it 
    boost::filesystem::ofstream(root + "/unit_test/abc");
    request.target("/api/unit_test/abc");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);

    // cleanup 
    boost::filesystem::remove_all(dir_path);
}

TEST_F(APIHandlerFixture, DeleteIDNotInPathCounts) {
    bhttp::request<bhttp::dynamic_body> request;
    boost::filesystem::path dir_path(root + "/unit_test");
    boost::filesystem::create_directory(dir_path);
    // cleanup first in case files exist 
    boost::filesystem::remove_all(dir_path);

    // Create file: code from https://stackoverflow.com/questions/30029343/how-do-i-create-a-file-with-boost-filesystem-without-opening-it 
    boost::filesystem::ofstream(root + "/unit_test/1");
    request.target("/api/unit_test/1");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::bad_request, status1);

    // cleanup 
    boost::filesystem::remove_all(dir_path);
}

TEST_F(APIHandlerFixture, DeleteValidRequest) {
    bhttp::request<bhttp::dynamic_body> request;

    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist 
    boost::filesystem::remove_all(dir_path);

    boost::filesystem::create_directory(dir_path);

    // Create file: code from https://stackoverflow.com/questions/30029343/how-do-i-create-a-file-with-boost-filesystem-without-opening-it 
    boost::filesystem::ofstream(root + "/unit_test/1");
    request.target("/api/unit_test/1");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::ok, status1);
    EXPECT_EQ(path_counts["unit_test"].size(), 0);

    // cleanup 
    boost::filesystem::remove_all(dir_path);
}

// If we have the map for unit_test to a vector of {1, 2, 3}
// and we delete 2 and post again,
// The POST should have an ID of 2. 
TEST_F(APIHandlerFixture, DeleteThenPost) {
    bhttp::request<bhttp::dynamic_body> request;
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist 
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);

    std::vector<int> ids = { 1, 3 };
    path_counts["unit_test"] = ids;
    // Create file: code from https://stackoverflow.com/questions/30029343/how-do-i-create-a-file-with-boost-filesystem-without-opening-it 
    boost::filesystem::ofstream(root + "/unit_test/2");
    request.target("/api/unit_test/2");
    request.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status1 = api_handler.handle_request(request, response);

    EXPECT_EQ(bhttp::status::ok, status1);
    EXPECT_EQ(path_counts["unit_test"].size(), 2);

    // POST 
    bhttp::request<bhttp::dynamic_body> request2;
    request2.target("/api/unit_test");
    request2.method(bhttp::verb::post);
    boost::beast::ostream(request2.body()) << "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::response<bhttp::dynamic_body> response2;
    request_api_handler api_handler2(base_uri, root, request.target().to_string(), path_counts, profile);
    bhttp::status status2 = api_handler2.handle_request(request2, response2);
    EXPECT_EQ(bhttp::status::created, status2);

    // Check that 2 is in path_counts 
    std::map<std::string, std::vector<int>>::iterator map_it;
    map_it = path_counts.find("unit_test");

    std::vector<int>::iterator vector_it;
    vector_it = find(map_it->second.begin(), map_it->second.end(), 2);

    EXPECT_NE(vector_it, map_it->second.end());

    // cleanup 
    boost::filesystem::remove_all(dir_path);
}

TEST_F(APIHandlerFixture, GETValidFile) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty file
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test/1");
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test/1", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::ok, status);
}

TEST_F(APIHandlerFixture, GETInvalidFile) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty file
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test/2"); // FILE doesn't exist
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test/2", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::bad_request, status);
}

TEST_F(APIHandlerFixture, GETAfterPOST) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty file
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);

    // POST
    std::string body = "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::request<bhttp::dynamic_body> request2;
    request2.target("/api/unit_test");
    request2.method(bhttp::verb::post);
    boost::beast::ostream(request2.body()) << body;
    bhttp::response<bhttp::dynamic_body> response2;
    request_api_handler api_handler2(base_uri, root, request2.target().to_string(), path_counts, profile);
    bhttp::status status2 = api_handler2.handle_request(request2, response2);
    EXPECT_EQ(bhttp::status::created, status2);

    // GET
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test/1");
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test/1", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::ok, status);
    EXPECT_EQ(body, boost::beast::buffers_to_string(response.body().data()));
}

TEST_F(APIHandlerFixture, LISTValidDir) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty files
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");
    boost::filesystem::ofstream(root + "/unit_test/2");
    boost::filesystem::ofstream(root + "/unit_test/3");

    std::vector<int> list = { 1,2,3 };

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test");
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::ok, status);

    std::vector<int> res_list;
    std::string res_data = boost::beast::buffers_to_string(response.body().data());
    std::string res_data_cleaned = res_data.substr(1, res_data.size() - 2);
    std::istringstream iss(res_data_cleaned);
    std::string token;
    while (std::getline(iss, token, ',')) {
        res_list.push_back(std::stoi(token));
    }
    EXPECT_THAT(list, testing::UnorderedElementsAreArray(res_list));
}

TEST_F(APIHandlerFixture, LISTInvalidDir) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty file
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");

    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test2"); // DIR doesn't exist
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test2", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::bad_request, status);
}

TEST_F(APIHandlerFixture, LISTAfterDELETE) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty files
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");
    boost::filesystem::ofstream(root + "/unit_test/2");
    boost::filesystem::ofstream(root + "/unit_test/3");

    std::vector<int> list = { 1,2,3 };

    // LIST before DELETE
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test");
    request.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::ok, status);

    std::vector<int> res_list;
    std::string res_data = boost::beast::buffers_to_string(response.body().data());
    std::string res_data_cleaned = res_data.substr(1, res_data.size() - 2);
    std::istringstream iss(res_data_cleaned);
    std::string token;
    while (std::getline(iss, token, ',')) {
        res_list.push_back(std::stoi(token));
    }
    EXPECT_THAT(list, testing::UnorderedElementsAreArray(res_list));

    // DELETE
    bhttp::request<bhttp::dynamic_body> request2;
    request2.target("/api/unit_test/1");
    request2.method(bhttp::verb::delete_);
    bhttp::response<bhttp::dynamic_body> response2;
    request_api_handler api_handler2(base_uri, root, request2.target().to_string(), path_counts, profile);
    bhttp::status status2 = api_handler2.handle_request(request2, response2);
    EXPECT_EQ(bhttp::status::ok, status2);

    std::vector<int> list2 = { 2,3 }; // 1 was removed, should no longer be outputted by LIST
    // LIST after DELETE
    bhttp::request<bhttp::dynamic_body> request3;
    request3.target("/api/unit_test");
    request3.method(bhttp::verb::get);
    bhttp::response<bhttp::dynamic_body> response3;
    request_api_handler api_handler3(base_uri, root, "/api/unit_test", path_counts, profile);
    bhttp::status status3 = api_handler3.handle_request(request3, response3);
    EXPECT_EQ(bhttp::status::ok, status);

    res_list = {};
    res_data = boost::beast::buffers_to_string(response3.body().data());
    res_data_cleaned = res_data.substr(1, res_data.size() - 2);
    std::istringstream iss2(res_data_cleaned);
    while (std::getline(iss2, token, ',')) {
        res_list.push_back(std::stoi(token));
    }
    EXPECT_THAT(list2, testing::UnorderedElementsAreArray(res_list));
}

TEST_F(APIHandlerFixture, PUTValidFile) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty files
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");

    std::string body = "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test/1");
    request.method(bhttp::verb::put);
    boost::beast::ostream(request.body()) << body;
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test/1", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::ok, status);
    EXPECT_EQ(body, boost::beast::buffers_to_string(response.body().data()));
}

TEST_F(APIHandlerFixture, PUTInvalidFile) {
    boost::filesystem::path dir_path(root + "/unit_test");
    // cleanup first in case files exist AND create empty files
    boost::filesystem::remove_all(dir_path);
    boost::filesystem::create_directory(dir_path);
    boost::filesystem::ofstream(root + "/unit_test/1");

    std::string body = "\'{\"name\":\"John\", \"age\":30, \"car\":null}\'";
    bhttp::request<bhttp::dynamic_body> request;
    request.target("/api/unit_test/2"); // FILE doesn't exist
    request.method(bhttp::verb::put);
    boost::beast::ostream(request.body()) << body;
    bhttp::response<bhttp::dynamic_body> response;
    request_api_handler api_handler(base_uri, root, "/api/unit_test/2", path_counts, profile);
    bhttp::status status = api_handler.handle_request(request, response);
    EXPECT_EQ(bhttp::status::bad_request, status);
}