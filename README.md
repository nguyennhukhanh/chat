# Chat Together

## TỔNG QUÁT

**Tiêu Đề:** Chat Together
<br>
**Email:** kwalker.nnk@gmail.com
<br>
**Facebook:** https://www.facebook.com/KWalkerNNK/

## PHÍM TẮT

- `:)` → [Đang cảm thấy vui vẻ!]
- `:(` → [Đang cảm thấy buồn!]
- `:^^` → [Đang cảm phấn khởi!]
- `:time` → Thời gian hiện tại
- `:+1hr` → Thời gian sau 1 tiếng nữa
- `:exit` → Rời đoạn chat


## HƯỚNG DẪN BIÊN DỊCH

* Để xây dựng các tệp thực thi cho cả client và server, hãy chạy lệnh sau từ terminal bên trong thư mục dự án:
    ```
    $ make
    ```

* Để xóa các tệp thực thi, bạn có thể chạy:
    ```
    $ make clean
    ```


## HƯỚNG DẪN CHẠY

* Đầu tiên chạy tệp server như sau:
  ```
    $ ./server [port_number]
  ```
  Ví dụ:
  ```
    $ ./server 1410
  ```

<br>

* Để chạy tệp client, ta làm như sau:
  ```
    $ ./client [port_number] [username] [password]
  ```
  Ví dụ:
  ```
    $ ./client 1410 khanh 123456
  ```


## LƯU Ý

- Tiêu chuẩn C: C99
- Trình biên dịch: GCC
- Địa chỉ IP: 127.0.0.1 (Được mã hóa cứng)
- Mật khẩu: 123456 (Được mã hóa cứng)
- Độ dài tối đa của tin nhắn mà Client có thể nhập: 1024 ký tự
- Thử nghiệm trên:
<br> - Ubuntu 22.04.1 LTS
