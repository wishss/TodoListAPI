# TodoListAPI
### 간단한 Todo List API
+ 기능: 할 일 목록을 추가, 조회, 수정, 삭제하는 API

## 개발 환경 구축
+ 환경 : Windows 10
+ IDE : Visual Stdio 2019
+ C++

### vcpkg 설치
    git clone https://github.com/microsoft/vcpkg.git cd vcpkg
    .\bootstrap-vcpkg.bat
    
### cpprestsdk 설치
    .\vcpkg install cpprestsdk

### Visual Studio에서 vcpkg 통합
    .\vcpkg integrate install

## REST API 요청/응답 명세
### 시스템 상태(CPU/Memory/Disk) 조회
    #### [ GET /todos ] ####
    요청:
    GET http://localhost:8080/todos

    응답:
    {
        "1": {
            "title": "Buy groceries",
            "completed": false
        },
        "2": {
            "title": "Read a book",
            "completed": true
        }
    }

    #### [ POST /todos ] ####
    요청:
    POST http://localhost:8080/todos
    Content-Type: application/json

    {
        "title": "Go for a walk",
        "completed": false
    }

    응답:
    201 Created
    Created: 3

    #### [ PUT /todos/1 ] ####
    요청:
    PUT http://localhost:8080/todos/1
    Content-Type: application/json

    {
        "title": "Buy groceries",
        "completed": true
    }

    응답:
    200 OK
    Updated

    #### [ DELETE /todos/2 ] ####
    요청:
    DELETE http://localhost:8080/todos/2

    응답:
    200 OK
    Deleted
    
## 사용한 라이브러리
+ cpprestsdk
