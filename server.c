#include "server.h"

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printRed("Loi: Thieu so cong tuong ung! (Vi du: ./server 1410)\n");
        return 1;
    }
    if (argc > 2)
    {
        printRed("Loi: Chi nhan 1 tham so la so cong! (Vi du: ./server 1410)\n");
        return 1;
    }

    port_number = atoi(argv[1]);
    if (port_number < 1024)
    {
        printRed("Loi: So cong khong hop le!\n");
        return 1;
    }

    int socket_desc, new_socket, c, *new_sock;
    struct sockaddr_in server, client;

    // Tạo Socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Khong the tao Socket");
    }

    // Khởi tạo sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    // Bind()
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Rang buoc that bai!");
        return 1;
    }

    fp = fopen(output_file_name, "w");
    signal(SIGINT, INThandler);

    printMagenta("~~~ Ban da vao Chat Room ~~~\n");
    fputs("~~~ Ban da vao Chat Room ~~~\n", fp);

    // Listen()
    listen(socket_desc, 5);

    // Chấp nhận kết nối mới từ Socket
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        struct connection_t *new_connection = (struct connection_t *)(struct connection *)malloc(
            sizeof(struct connection_t));
        new_connection->p_sock = new_socket;
        new_connection->status = 1;
        new_connection->auth = 0;
        add_socket(new_connection);

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_connection);
    }

    if (new_socket < 0)
    {
        perror("Chap nhan ket noi tu socket hien dang loi!");
        return 1;
    }

    return 0;
}

// Xử lý kết nối với từng client
void *connection_handler(void *p_connection)
{
    struct connection_t *connection = (struct connection_t *)p_connection;
    int dummy = 0;

    pthread_t receiving_thread;

    if (pthread_create(&receiving_thread, NULL, receiver_handler, (void *)p_connection) > 0)
    {
        perror("Nguoi nhan hien nhan khong duoc!");
        return 0;
    }

    while (connection->status)
    {
        dummy++;
    }

    // Giải phóng con trỏ Socket
    broadcast(p_connection, "", 2);
    delete_socket(p_connection);
    free(p_connection);

    return 0;
}

// Nhận tin nhắn từ client
void *receiver_handler(void *p_connection)
{
    struct connection_t *connection = (struct connection_t *)p_connection;
    // Lấy mô tả trong Socket
    int sock = connection->p_sock;
    char username[20];
    char token[41];
    int read_size;
    char client_message[2000];
    char processed_client_message[2000];

    // Nhận tin nhắn từ Client
    while ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        if (!connection->auth)
        {
            strcpy(token, client_message);

            char ch = token[0];
            int i = 0;
            while (ch != '\n' && ch != '\0')
            {
                username[i] = ch;
                i++;
                ch = token[i];
            }
            username[i] = '\0';
            strcpy(connection->username, username);

            printBoldGreen(connection->username);
            printBoldGreen(" da vao phong!\n");
            fprintf(fp, "%s da vao phong!\n", connection->username);

            broadcast(connection, "", 1);

            connection->auth = 1;
        }
        else
        {
            int ie = 0;
            for (int i = 0; i <= strlen(client_message); i++)
            {
                if (client_message[i] == '\n')
                {
                    client_message[i] = '\0';
                }
                processed_client_message[i] = client_message[i];
                ie = i;
            }

            printf("%s: %s\n", connection->username, processed_client_message);
            fprintf(fp, "%s: %s\n", connection->username, processed_client_message);

            processed_client_message[ie] = '\n';
            processed_client_message[ie + 1] = '\0';

            broadcast(connection, processed_client_message, 0);
        }
    }

    if (read_size == 0)
    {
        printYellow(connection->username);
        printYellow(" da roi phong!\n");
        fprintf(fp, "%s da roi phong!\n", connection->username);

        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    connection->status = 0;

    return 0;
}

void printRed(char *string)
{
    printf("\033[1;31m");
    printf("%s", string);
    printf("\033[0m");
}

void printYellow(char *string)
{
    printf("\033[1;33m");
    printf("%s", string);
    printf("\033[0m");
}

void printMagenta(char *string)
{
    printf("\033[1;35m");
    printf("%s", string);
    printf("\033[0m");
}

void printBoldGreen(char *string)
{
    printf("\033[1;32m");
    printf("%s", string);
    printf("\033[0m");
}

void add_socket(struct connection_t *socket)
{
    if (open_connections == 0)
    {
        socket->next = NULL;
        head = socket;
        tail = socket;
        open_connections++;
    }
    else if (open_connections > 0)
    {
        tail->next = socket;
        socket->next = NULL;
        tail = socket;
        open_connections++;
    }
}

void delete_socket(struct connection_t *socket)
{
    if (open_connections == 1 && socket == head)
    {
        head = NULL;
        open_connections--;
    }
    else if (socket == head)
    {
        head = head->next;
        open_connections--;
    }
    else if (open_connections > 1)
    {

        struct connection_t *currentSocket = head;
        while (currentSocket->next != socket && currentSocket->next != NULL)
        {
            currentSocket = currentSocket->next;
        }

        if (tail == socket)
        {
            tail = currentSocket;
            currentSocket->next = NULL;
        }
        else
        {
            currentSocket->next = currentSocket->next->next;
        }
        open_connections--;
    }
}

void broadcast(struct connection_t *socket, char *message, int type)
{
    char signed_message[strlen(socket->username) + 2 + strlen(message)];
    int size = strlen(socket->username) + 2;
    strcpy(signed_message, socket->username);

    // Message type
    if (type == 0)
    {
        strcat(signed_message, ": ");

        char happy_code[] = ":)\n";
        char sad_code[] = ":(\n";
        char exciting_code[] = ":^^\n";
        char mytime_code[] = ":time\n";
        char extrahour_code[] = ":+1hr\n";
        char exit_code[] = ":exit\n";

        if (strcmp(happy_code, message) == 0)
        {
            strcat(signed_message, "[Dang cam thay vui ve!]\n");
            size += strlen("[Dang cam thay vui ve!]\n");
        }
        else if (strcmp(sad_code, message) == 0)
        {
            strcat(signed_message, "[Dang cam thay buon!]\n");
            size += strlen("[Dang cam thay buon!]\n");
        }
        else if (strcmp(exciting_code, message) == 0)
        {
            strcat(signed_message, "[Dang cam phan khoi!]\n");
            size += strlen("[Dang cam phan khoi!]\n");
        }
        else if (strcmp(mytime_code, message) == 0)
        {
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            char time_string[40] = "";
            sprintf(time_string, "%s\n", asctime(timeinfo));

            strcat(signed_message, time_string);
            size += strlen(time_string);
        }
        else if (strcmp(extrahour_code, message) == 0)
        {
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            timeinfo->tm_hour = timeinfo->tm_hour + 1;

            char time_string[40] = "";
            sprintf(time_string, "%s\n", asctime(timeinfo));

            strcat(signed_message, time_string);
            size += strlen(time_string);
        }
        else if (strcmp(exit_code, message) == 0)
        {
            socket->status = 0;
            shutdown(socket->p_sock, 2);
            return;
        }
        else
        {
            strcat(signed_message, message);
            size += strlen(message);
        }
        strcat(signed_message, "\0");
    }
    else if (type == 1)
    {
        strcat(signed_message, " da vao phong!\n");
        size += strlen(" da vao phong!\n");
    }
    else if (type == 2)
    {
        strcat(signed_message, " da roi phong!\n");
        size += strlen(" da roi phong!\n");
    }

    struct connection_t *currentSocket = head;
    int i = 0;
    while (i < open_connections)
    {

        if (socket != currentSocket)
        {
            write(currentSocket->p_sock, signed_message, size);
        }
        currentSocket = currentSocket->next;
        i++;
    }
}

void INThandler(int sig)
{
    printYellow("\nNgat ket noi\n");
    fclose(fp);
    exit(0);
}
