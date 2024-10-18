#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <map>
#include <mutex>

using namespace web;                      
using namespace web::http;                
//using namespace web::http::experimental;  // HTTP client.
using namespace web::http::experimental::listener; // HTTP server.

std::map<int, json::value> todoList;     // Todo ����Ʈ ����
std::mutex mtx;                           // ����ȭ�� ���� ���ؽ�
int currentId = 1;                        // ID ����

// GET ��û ó��
void handle_get(http_request request) {
    json::value responseJson = json::value::object(); // ��ü�� ����
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& item : todoList) {
        responseJson[std::to_wstring(item.first)] = item.second; // ID�� ���ڿ��� Ű ���
    }

    request.reply(status_codes::OK, responseJson);
}

// POST ��û ó��
void handle_post(http_request request) {
    request.extract_json().then([&](json::value requestJson) {
        std::lock_guard<std::mutex> lock(mtx);
        int id = currentId++;
        todoList[id] = requestJson; // ���ο� Todo �߰�

        // Create a JSON object for the response
        json::value responseJson;
        responseJson[U("message")] = json::value::string(U("Created: ") + std::to_wstring(id));

        request.reply(status_codes::Created, responseJson);
        }).wait();
}

// PUT ��û ó��
void handle_put(http_request request) {
    int id = std::stoi(request.relative_uri().path().substr(1)); // URL���� ID ����

    request.extract_json().then([&](json::value requestJson) {
        std::lock_guard<std::mutex> lock(mtx);
        if (todoList.find(id) != todoList.end()) {
            todoList[id] = requestJson; // Todo ����
            request.reply(status_codes::OK, json::value::string(U("Updated")));
        }
        else {
            request.reply(status_codes::NotFound, json::value::string(U("Not Found")));
        }
        }).wait();
}

// DELETE ��û ó��
void handle_delete(http_request request) {
    int id = std::stoi(request.relative_uri().path().substr(1)); // URL���� ID ����
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
    std::getline(std::cin, line); // Enter Ű�� ���� ������ ���
    return 0;
}