# School Information Management System
## About
This is a school information management system with basic CRUD functionalities for students, courses, and classes, utilizing a three-tier architecture with socket communication between client and server, thread pool management for handling multiple client requests, and implementing reliable data transmission using the UDP protocol.
## Features
1. Core Features:
- Student Information Management: Record and manage basic student information.
- Course Information Management: Record and manage basic course information.
- Class Information Management: Record and manage basic class information.
- CRUD Operations: Support for creating, reading, updating, and deleting (CRUD) records for students, courses, and classes, including advanced query capabilities with support for complex search combinations.
- Relationship Management:
    - A student can belong to only one class.
    - A class can have multiple students.
    - A student can enroll in multiple courses.
    - A course can be taken by multiple students.
- Handling Duplicates: Support for managing duplicate student names, class names, and course names.
2. Three-Tier Architecture: The system uses a three-tier architecture, with socket-based communication between the client and server. The server is responsible for database operations.
3. Thread Pool Management: Efficient handling of multiple client requests using a thread pool.
4. Reliable Data Transmission: Implementation of reliable data transmission using TCP protocol
## Structure 
![image](https://github.com/user-attachments/assets/7fef0f1f-eb44-4e43-96f1-fc567ddc359a)

![image](https://github.com/user-attachments/assets/993b4de6-4f6b-4a9c-ba59-1549a05cdc60)

## Use
Tested only on windows 10 and windows 11.
1. Start Server
2. Start Client and log in
 ![image](https://github.com/user-attachments/assets/26454acc-6c0b-43a0-ae4a-e382cdf26d6c)
3. Perform CRUD operations
   - Please note: The student ID is a number, while all other data fields are strings.
   - example:
   ![image](https://github.com/user-attachments/assets/be69f858-3343-45d7-9f0e-18e1f835fcb2)




