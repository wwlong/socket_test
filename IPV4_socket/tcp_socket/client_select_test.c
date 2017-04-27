void client_select_test()
{
    int sfd, ret;
	sfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in v;
    char recv_buf[128];
    int lenght = 128;
	v.sin_family = AF_INET;
	v.sin_port = htons(PORT);
	v.sin_addr.s_addr = inet_addr(SRV_IP);
	//inet_pton(AF_INET6,argv[2],&v.sin_addr.s_addr);
	socklen_t len = sizeof(v);

    connect(sfd,(struct sockaddr *)&v,len);
    printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
	printf("Enter Data\n");
    //printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
    
    char *s = "I am select";
	write(sfd,s,strlen(s));
    memset(recv_buf, 0, sizeof(recv_buf));
    recv(sfd, recv_buf, lenght, 0);
    printf("recv : %s\r\n", recv_buf);
    fd_set wset, rset;
    struct timeval timeout;
    while(1) {
        FD_ZERO(&wset);
        FD_SET(sfd, &wset);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        if(FD_ISSET(sfd, &wset)) {
            ret = select(sfd, NULL, &wset, NULL, &timeout);
            if(ret < 0) {
                printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
                return -1;
            }
            else if(0 == ret) {
                printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
            }
            else {
                send(sfd, s, strlen(s), 0);
            }
        }

        char *str = "not select";
        send(sfd, str, strlen(str), 0);
        memset(recv_buf, 0, sizeof(recv_buf));
        recv(sfd, recv_buf, sizeof(recv_buf), 0);
        printf("not select : %s\r\n", recv_buf);
        FD_ZERO(&rset);
        FD_SET(sfd, &rset);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        if(FD_ISSET(sfd, &rset)) {
            ret = select(sfd, &rset, NULL, NULL, &timeout);
            if(ret < 0) {
                printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
                return -1;
            }
            else if(0 == ret) {
                printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
            }
            else {
                recv(sfd, recv_buf, sizeof(recv_buf), 0);
                printf("recv : %s\r\n", recv_buf);
            }
        }
     }

    printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);

}

