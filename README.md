# Mid-Term-Project
# Tên đề tài: Hệ thống tưới cây tự động và giám sát qua MQTT. 
# Nội dung: 
- ESP32 đọc dữ liệu từ cảm biến độ ẩm đất và gửi thông tin qua giao thức MQTT đến Node-RED. Node-RED xử lý dữ liệu, hiển thị trên dashboard, và điều khiển bơm nước tự động hoặc thủ công từ giao diện web.
# Sơ đồ khối: 
![diagram](https://github.com/user-attachments/assets/a39e6c23-bf11-47eb-8667-75c768a6c86c) 

# Linh kiện: 
- ESP32 
- Cảm biến độ ẩm đất 
- Relay 5V 
- Máy bơm mini 
- Breadboard 
# Đặt vấn đề: 
Cuộc sống ngày càng hiện đại đi kèm với nhiều vấn đề về môi trường sống,chất lượng không khí nên càng ngày càng nhiều người có mong muốn trồng cây xanh để cải thiện không gian.Tuy nhiên, việc chăm sóc cây trồng, đặc biệt là tưới nước đúng thời điểm và đủ lượng, thường gặp nhiều khó khăn vì: 
- Nhịp sống bận rộn: Nhiều người không có đủ thời gian để kiểm tra tình trạng đất và tưới cây đều đặn. 
- Thiếu hiểu biết về nhu cầu nước của cây: Điều này dẫn đến tình trạng tưới quá nhiều hoặc quá ít, làm ảnh hưởng đến sự phát triển của cây. 
- Khoảng cách địa lý: Với những người thường xuyên vắng nhà hoặc quản lý các khu vườn ở xa, việc theo dõi và tưới cây trở nên không thực tế.
# Cảm hứng đề tài: 
- Xuất phát từ việc thích trồng và chăm sóc cây cảnh của bản, mong muốn tạo ra một giải pháp tự động giúp người chăm sóc cây trồng dễ dàng hơn.
- Hệ thống này không chỉ giúp người dùng giải quyết các vấn đề trong việc chăm sóc cây mà còn mở ra khả năng mở rộng áp dụng trong nông nghiệp thông minh, phù hợp với xu hướng phát triển bền vững và hiện đại hóa trong sản xuất nông nghiệp.

# Demo: 
# Giao diện Node-RED: 

![giao_dien](https://github.com/user-attachments/assets/ca49c4ea-e00d-43a8-9f5c-a289ca4b482e) 
# Các chế độ điều khiển , thông số hiển thị ,trạng thái: 
- Trên dashboard thể hiện các thông số về độ ẩm, đồ thị, trạng thái của máy bơm và chế độ điều khiển người dùng.
- Có 2 chế độ AUTO và MANUAL : AUTO khi hệ thống tự động kiểm soát bơm , MANUAL khi người dùng tự điều khiển bơm. 

![on_auto](https://github.com/user-attachments/assets/004fda27-26ee-4780-84a4-b8734f4cda67)
- Chế độ AUTO khi bơm mở 

![off_auto](https://github.com/user-attachments/assets/560bf582-5a01-45b8-8dec-e022729b1446)
- Chế độ AUTO khi bơm đóng  

![on_manual](https://github.com/user-attachments/assets/e6e1edc3-77f8-47ab-b8fb-01c2863416b1)
- Chế độ MANUAL khi bơm mở 

![off_manual](https://github.com/user-attachments/assets/dc2c800b-c9ec-4dfb-aec5-ab6462a9ae3e)

- Chế độ MANUAL khi bơm đóng  

# Hiển thị thông tin qua Serial và  HiveMQ qua MQTT: 

- Các thông số độ ẩm và trạng thái được thể hiện qua các hình dưới: 

![hivemq](https://github.com/user-attachments/assets/b7036e30-a34d-4b84-b8b3-fe4df3e7dd17)
- Thông tin gửi và nhận trên HiveMQ


![chon_che_do_serial](https://github.com/user-attachments/assets/8f4a97a0-f7ee-4efa-97ec-7fb71604bd76)
- Thông tin về chọn chế độ trên VSCode 
  
![serial_mqtt](https://github.com/user-attachments/assets/52cfd9e2-11bf-4553-91b0-ac18754f39d7)
- Thông tin về thông số, trạng thái trên VSCode
  

# Ảnh setup linh kiện và video demo máy bơm khi mở:

![462558426_556668457233836_727470844798820272_n](https://github.com/user-attachments/assets/503c456f-a1ab-4ea5-bcd0-95ef61020ebe)
- Ảnh lắp linh kiện phần cứng 




- Demo Video: 
https://github.com/user-attachments/assets/f84d7eb9-4497-421b-9772-d5ed053baab5




