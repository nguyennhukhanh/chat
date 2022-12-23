#include "client.h"

int main(int argc, char *argv[])
{
    if (argc <= 3)
    {
        printRed("Loi: Nhap so cong de ket noi, ten nguoi dung va mat khau! "
                 "(Vi du: ./client 1410 username xxxxxx)\n");
        return 1;
    }
    if (argc > 4)
    {
        printRed("Loi: Chi nhan 3 tham so: So cong, ten nguoi dung va mat khau! "
                 "(Vi du: ./client 1410 username xxxxxx)\n");
        return 1;
    }

    port_number = atoi(argv[1]);
    if (port_number < 1024)
    {
        printRed("Loi: So cong khong hop le\n");
        return 1;
    }

    if (strlen(argv[2]) > 20)
    {
        printRed("Loi: Ten nguoi dung khong the vuot qua 20 ky tu\n");
        return 1;
    }

    if (strlen(argv[3]) > 20)
    {
        printRed("Loi: Mat khau khong duoc vuot qua 20 ky tu\n");
        return 1;
    }
    strcpy(username, argv[2]);
    strcpy(password, argv[3]);
    char ch = username[0];
    int i = 0;

    while (ch != '\0')
    {
        if (isalnum(ch))
        {
            i++;
        }
        else
        {
            printRed("Loi: Chi cho phep ten nguoi dung co chu va so\n");
            return 1;
        }
        ch = username[i];
    }

    if (strcmp(password, server_password) != 0)
    {
        printRed("Loi dang nhap: Mat khau khong chinh xac!\n");
        return 1;
    }

    // Tạo Socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Khong the tao Socket");
    }

    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);

    // Kết nối tới máy chủ từ xa
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Loi ket noi");
        return 1;
    }

    status = 1;
    printMagenta("~~~ Ban da vao Chat Room ~~~\n");

    pthread_t receiving_thread;
    int new_socket = socket_desc;
    int *new_sock;
    new_sock = malloc(1);
    *new_sock = new_socket;

    pthread_create(&receiving_thread, NULL, receiver_handler, (void *)new_sock);

    strcpy(auth_token, username);
    strcat(auth_token, "\n");

    // Gửi mã thông báo xác thực
    if (send(socket_desc, auth_token, strlen(auth_token), 0) < 0)
    {
        puts("Dang nhap that bai");
        return 1;
    }

    while (status)
    {
        char str[message_limit];
        fgets(str, message_limit + 2, stdin);
        if (!status)
        {
            return 0;
        }
        if (str[0] == '\0' || str[0] == '\n')
        {
            printRed("Loi: Ban khong the gui tin nhan trong!\n");
            fflush(stdin);
        }
        else
        {
            fflush(stdin);
            if (strlen(str) > message_limit)
            {
                printRed("Loi: Tin nhan cua ban khong the vuot qua 1024 ky tu!\n");
            }
            else
            {
                if (send(socket_desc, str, strlen(str), 0) < 0)
                {
                    puts("Gui that bai!");
                    return 1;
                }
            }
        }
    }
    return 0;
}

void *receiver_handler(void *socket)
{
    // Lấy mô tả của socket
    int sock = *(int *)socket;
    int read_size;
    char server_message[2000];
    char processed_server_message[2000];

    // Nhận tin nhắn từ client
    // Hàm RECV được sử dụng để đọc dữ liệu của Socket.
    while ((read_size = recv(sock, server_message, 2000, 0)) > 0)
    {
        int ie = 0;
        for (int i = 0; i <= strlen(server_message); i++)
        {
            if (server_message[i] == '\n')
            {
                server_message[i] = '\0';
            }
            processed_server_message[i] = server_message[i];
            ie = i;
        }
        printBlue(processed_server_message);
        printf("\n");
        processed_server_message[ie] = '\n';
    }

    // Nếu thành công, recv() trả về độ dài của tin nhắn hoặc datagram trong byte. Giá trị 0 cho biết kết nối được đóng.
    if (read_size == 0)
    {
        printYellow("May chu bi ngat ket noi\n");
        exit(0);
    }
    else if (read_size == -1)
    {
        perror("May chu van chua bi dong, Loi recv!");
    }

    status = 0;

    // Giải phóng con trỏ Socket
    free(socket);

    return 0;
}

void printRed(char *string)
{
    printf("\033[1;31m");
    printf("%s", string);
    printf("\033[0m");
}

void printMagenta(char *string)
{
    printf("\033[1;35m");
    printf("%s", string);
    printf("\033[0m");
}

void printBlue(char *string)
{
    printf("\033[0;36m");
    printf("%s", string);
    printf("\033[0m");
}

void printYellow(char *string)
{
    printf("\033[1;33m");
    printf("%s", string);
    printf("\033[0m");
}
