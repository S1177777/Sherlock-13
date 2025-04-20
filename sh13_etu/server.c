#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

struct _client {
    char ipAddress[40];
    int port;
    char name[40];
} tcpClients[4];

int nbClients;
int fsmServer;
int deck[13] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
int tableCartes[4][8];
char *nomcartes[] = {
    "Sebastian Moran", "irene Adler", "inspector Lestrade",
    "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
    "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
    "Mrs. Hudson", "Mary Morstan", "James Moriarty"
};
int joueurCourant;
int eliminatedCount;
int eliminatedIDs[4];

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void initRandom() {
    srand((unsigned)time(NULL));
}

void melangerDeck() {
    initRandom();
    int i, index1, index2, tmp;
    for (i = 0; i < 1000; i++) {
        index1 = rand() % 13;
        index2 = rand() % 13;
        tmp = deck[index1];
        deck[index1] = deck[index2];
        deck[index2] = tmp;
    }
}

void createTable() {
    int i, j, c;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 8; j++)
            tableCartes[i][j] = 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            c = deck[i*3+j];
            switch (c) {
                case 0:
                    tableCartes[i][7]++;
                    tableCartes[i][2]++;
                    break;
                case 1:
                    tableCartes[i][7]++;
                    tableCartes[i][1]++;
                    tableCartes[i][5]++;
                    break;
                case 2:
                    tableCartes[i][3]++;
                    tableCartes[i][6]++;
                    tableCartes[i][4]++;
                    break;
                case 3:
                    tableCartes[i][3]++;
                    tableCartes[i][2]++;
                    tableCartes[i][4]++;
                    break;
                case 4:
                    tableCartes[i][3]++;
                    tableCartes[i][1]++;
                    break;
                case 5:
                    tableCartes[i][3]++;
                    tableCartes[i][2]++;
                    break;
                case 6:
                    tableCartes[i][3]++;
                    tableCartes[i][0]++;
                    tableCartes[i][6]++;
                    break;
                case 7:
                    tableCartes[i][0]++;
                    tableCartes[i][1]++;
                    tableCartes[i][2]++;
                    break;
                case 8:
                    tableCartes[i][0]++;
                    tableCartes[i][6]++;
                    tableCartes[i][2]++;
                    break;
                case 9:
                    tableCartes[i][0]++;
                    tableCartes[i][1]++;
                    tableCartes[i][4]++;
                    break;
                case 10:
                    tableCartes[i][0]++;
                    tableCartes[i][5]++;
                    break;
                case 11:
                    tableCartes[i][4]++;
                    tableCartes[i][5]++;
                    break;
                case 12:
                    tableCartes[i][7]++;
                    tableCartes[i][1]++;
                    break;
            }
        }
    }
}

void printDeck() {
    int i,j;
    for (i=0;i<13;i++)
        printf("%d %s\n",deck[i],nomcartes[deck[i]]);
    for (i=0;i<4;i++) {
        for (j=0;j<8;j++)
            printf("%2.2d ",tableCartes[i][j]);
        puts("");
    }
}

void printClients() {
    int i;
    for (i=0;i<nbClients;i++)
        printf("%d: %s %5.5d %s\n",i,tcpClients[i].ipAddress,
            tcpClients[i].port,
            tcpClients[i].name);
}

int findClientByName(char *name) {
    int i;
    for (i=0;i<nbClients;i++)
        if (strcmp(tcpClients[i].name,name)==0)
            return i;
    return -1;
}

void sendMessageToClient(char *clientip, int clientport, char *mess) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(clientip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(clientport);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    sprintf(buffer,"%s\n",mess);
    n = write(sockfd,buffer,strlen(buffer));
    close(sockfd);
}

void broadcastMessage(char *mess) {
    int i;
    for (i=0;i<nbClients;i++)
        sendMessageToClient(tcpClients[i].ipAddress,
            tcpClients[i].port,
            mess);
}

int isPlayerEliminated(int id) {
    int i;
    for (i = 0; i < eliminatedCount; i++) {
        if (eliminatedIDs[i] == id) return 1;
    }
    return 0;
}

void updateCurrentPlayer() {
    do {
        joueurCourant = (joueurCourant + 1) % 4;
    } while (isPlayerEliminated(joueurCourant));
}

void handleAccusation(int id, int suspect) {
    char reply[256];
    if (suspect == deck[12]) {
        sprintf(reply, "F %s %s", tcpClients[id].name, nomcartes[suspect]);
        broadcastMessage(reply);
        exit(0);
    } else {
        eliminatedIDs[eliminatedCount++] = id;
        updateCurrentPlayer();
        if (eliminatedCount >= 3) {
            sprintf(reply, "F %s %s", tcpClients[joueurCourant].name, nomcartes[deck[12]]);
            broadcastMessage(reply);
            exit(0);
        }
        sprintf(reply, "M %d", joueurCourant);
        broadcastMessage(reply);
    }
}

void handleSymbolQuery(int id, int symbol) {
    char reply[256];
    int i, j;
    for (i = 0; i < 4; i++) {
        if (i != id) {
            if (tableCartes[i][symbol] == 0) {
                sprintf(reply, "V %d %d 0", i, symbol);
                broadcastMessage(reply);
            } else {
                sprintf(reply, "V %d %d 100", i, symbol);
                for (j = 0; j < 4; j++) {
                    if (j != i) sendMessageToClient(tcpClients[j].ipAddress, tcpClients[j].port, reply);
                }
            }
        }
    }
    updateCurrentPlayer();
    sprintf(reply, "M %d", joueurCourant);
    broadcastMessage(reply);
}

void handleCountQuery(int id, int target, int symbol) {
    char reply[256];
    sprintf(reply, "V %d %d %d", target, symbol, tableCartes[target][symbol]);
    broadcastMessage(reply);
    updateCurrentPlayer();
    sprintf(reply, "M %d", joueurCourant);
    broadcastMessage(reply);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i;
    char com;
    char clientIpAddress[256], clientName[256];
    int clientPort;
    int id;
    char reply[256];

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    printDeck();
    melangerDeck();
    createTable();
    printDeck();
    joueurCourant = 0;
    eliminatedCount = 0;
    memset(eliminatedIDs, -1, sizeof(eliminatedIDs));

    for (i=0;i<4;i++) {
        strcpy(tcpClients[i].ipAddress,"localhost");
        tcpClients[i].port = -1;
        strcpy(tcpClients[i].name,"-");
    }

    while (1) {    
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
            error("ERROR reading from socket");

        printf("Received: %s\n", buffer);

        if (fsmServer == 0) {
            switch (buffer[0]) {
                case 'C':
                    sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                    strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
                    tcpClients[nbClients].port = clientPort;
                    strcpy(tcpClients[nbClients].name,clientName);
                    nbClients++;

                    id = findClientByName(clientName);
                    sprintf(reply,"I %d",id);
                    sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);

                    sprintf(reply,"L %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name);
                    broadcastMessage(reply);

                    if (nbClients == 4) {
                        for (int p = 0; p < 4; p++) {
                            sprintf(reply, "D %d %d %d", deck[3*p], deck[3*p+1], deck[3*p+2]);
                            sendMessageToClient(tcpClients[p].ipAddress, tcpClients[p].port, reply);
                            for (int s = 0; s < 8; s++) {
                                sprintf(reply, "V %d %d %d", p, s, tableCartes[p][s]);
                                sendMessageToClient(tcpClients[p].ipAddress, tcpClients[p].port, reply);
                            }
                        }
                        sprintf(reply, "M %d", joueurCourant);
                        broadcastMessage(reply);
                        fsmServer = 1;
                    }
                    break;
            }
        } else if (fsmServer == 1) {
            switch (buffer[0]) {
                case 'G': {
                    int id, suspect;
                    sscanf(buffer, "G %d %d", &id, &suspect);
                    handleAccusation(id, suspect);
                    break;
                }
                case 'O': {
                    int id, symbol;
                    sscanf(buffer, "O %d %d", &id, &symbol);
                    handleSymbolQuery(id, symbol);
                    break;
                }
                case 'S': {
                    int id, target, symbol;
                    sscanf(buffer, "S %d %d %d", &id, &target, &symbol);
                    handleCountQuery(id, target, symbol);
                    break;
                }
            }
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0; 
}