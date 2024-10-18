#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <map>
#include <mutex>

using namespace web;                      
using namespace web::http;                
//using namespace web::http::experimental;  // HTTP client.
using namespace web::http::experimental::listener; // HTTP server.

std::map<int, json::value> todoList;     // Todo 리스트 저장
std::mutex mtx;                           // 동기화를 위한 뮤텍스
int currentId = 1;                        // ID 관리

// GET 요청 처리
void handle_get(http_request request) {
    json::value responseJson = json::value::object(); // 객체로 변경
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& item : todoList) {
        responseJson[std::to_wstring(item.first)] = item.second; // ID를 문자열로 키 사용
    }

    request.reply(status_codes::OK, responseJson);
}

// POST 요청 처리
void handle_post(http_request request) {
    request.extract_json().then([&](json::value requestJson) {
        std::lock_guard<std::mutex> lock(mtx);
        int id = currentId++;
        todoList[id] = requestJson; // 새로운 Todo 추가

        // Create a JSON object for the response
        json::value responseJson;
        responseJson[U("message")] = json::value::string(U("Created: ") + std::to_wstring(id));

        request.reply(status_codes::Created, responseJson);
        }).wait();
}

// PUT 요청 처리
void handle_put(http_request request) {
    int id = std::stoi(request.relative_uri().path().substr(1)); // URL에서 ID 추출

    request.extract_json().then([&](json::value requestJson) {
        std::lock_guard<std::mutex> lock(mtx);
        if (todoList.find(id) != todoList.end()) {
            todoList[id] = requestJson; // Todo 수정
            request.reply(status_codes::OK, json::value::string(U("Updated")));
        }
        else {
            request.reply(status_codes::NotFound, json::value::string(U("Not Found")));
        }
        }).wait();
}

// DELETE 요청 처리
void handle_delete(http_request request) {
    int id = std::stoi(request.relative_uri().path().substr(1)); // URL에서 ID 추출
    std::lock_guard<std::mutex> lock(mtx);

    if (todoList.erase(id)) {
        request.reply(status_codes::OK, json::value::string(U("Deleted")));
    }
    else {
        request.reply(status_codes::NotFound, json::value::string(U("Not Found")));
    }
}

int main() {
    uri_builder uri(U("http://localhost:8080/todos"));
    auto addr = uri.to_uri().to_string();
    http_listener listener(addr);

    listener.support(methods::GET, handle_get);
    listener.support(methods::POST, handle_post);
    listener.support(methods::PUT, handle_put);
    listener.support(methods::DEL, handle_delete);

    listener
        .open()
        .then([&listener]() { ucout << "Starting to listen at: " << listener.uri().to_string() << std::endl; })
        .wait();

    std::string line;
    std::getline(std::cin, line); // Enter 키를 누를 때까지 대기
    return 0;
}